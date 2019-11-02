/*
 * xcf.cpp: A Qt 5 plug-in for reading GIMP XCF image files
 * Copyright (C) 2001 lignum Computing, Inc. <allen@lignumcomputing.com>
 * Copyright (C) 2004 Melchior FRANZ <mfranz@kde.org>
 *
 * This plug-in is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "xcf_p.h"

#include <stdlib.h>
#include <QImage>
#include <QPainter>
#include <QIODevice>
#include <QStack>
#include <QVector>
#include <QDebug>

#include <string.h>

#include "gimp_p.h"

const float INCHESPERMETER = (100.0f / 2.54f);

/*!
 * Each layer in an XCF file is stored as a matrix of
 * 64-pixel by 64-pixel images. The GIMP has a sophisticated
 * method of handling very large images as well as implementing
 * parallel processing on a tile-by-tile basis. Here, though,
 * we just read them in en-masse and store them in a matrix.
 */
typedef QVector<QVector<QImage> > Tiles;

class XCFImageFormat
{
public:
    XCFImageFormat();
    bool readXCF(QIODevice *device, QImage *image);

private:
    /*!
     * Each GIMP image is composed of one or more layers. A layer can
     * be one of any three basic types: RGB, grayscale or indexed. With an
     * optional alpha channel, there are six possible types altogether.
     *
     * Note: there is only ever one instance of this structure. The
     * layer info is discarded after it is merged into the final QImage.
     */
    class Layer
    {
    public:
        quint32 width;          //!< Width of the layer
        quint32 height;     //!< Height of the layer
        qint32 type;            //!< Type of the layer (GimpImageType)
        char *name;         //!< Name of the layer
        quint32 hierarchy_offset;   //!< File position of Tile hierarchy
        quint32 mask_offset;        //!< File position of mask image

        uint nrows;         //!< Number of rows of tiles (y direction)
        uint ncols;         //!< Number of columns of tiles (x direction)

        Tiles image_tiles;      //!< The basic image
        //! For Grayscale and Indexed images, the alpha channel is stored
        //! separately (in this data structure, anyway).
        Tiles alpha_tiles;
        Tiles mask_tiles;       //!< The layer mask (optional)

        //! Additional information about a layer mask.
        struct {
            quint32 opacity;
            quint32 visible;
            quint32 show_masked;
            uchar red, green, blue;
            quint32 tattoo;
        } mask_channel;

        bool active;            //!< Is this layer the active layer?
        quint32 opacity = 255;  //!< The opacity of the layer
        quint32 visible = 1;    //!< Is the layer visible?
        quint32 linked;     //!< Is this layer linked (geometrically)
        quint32 preserve_transparency; //!< Preserve alpha when drawing on layer?
        quint32 apply_mask = 9; //!< Apply the layer mask? Use 9 as "uninitilized". Spec says "If the property does not appear for a layer which has a layer mask, it defaults to true (1).
                                //   Robust readers should force this to false if the layer has no layer mask.
        quint32 edit_mask;      //!< Is the layer mask the being edited?
        quint32 show_mask;      //!< Show the layer mask rather than the image?
        qint32 x_offset = 0;    //!< x offset of the layer relative to the image
        qint32 y_offset = 0;    //!< y offset of the layer relative to the image
        quint32 mode = 0;       //!< Combining mode of layer (LayerModeEffects)
        quint32 tattoo;     //!< (unique identifier?)

        //! As each tile is read from the file, it is buffered here.
        uchar tile[TILE_WIDTH *TILE_HEIGHT *sizeof(QRgb)];

        //! The data from tile buffer is copied to the Tile by this
        //! method.  Depending on the type of the tile (RGB, Grayscale,
        //! Indexed) and use (image or mask), the bytes in the buffer are
        //! copied in different ways.
        void (*assignBytes)(Layer &layer, uint i, uint j);

        Layer(void) : name(nullptr) {}
        ~Layer(void)
        {
            delete[] name;
        }

        Layer(const Layer &) = delete;
        Layer &operator=(const Layer &) = delete;
    };

    /*!
     * The in-memory representation of the XCF Image. It contains a few
     * metadata items, but is mostly a container for the layer information.
     */
    class XCFImage
    {
    public:
        quint32 width;          //!< width of the XCF image
        quint32 height;     //!< height of the XCF image
        qint32 type;            //!< type of the XCF image (GimpImageBaseType)

        quint8 compression;     //!< tile compression method (CompressionType)
        float x_resolution = -1;//!< x resolution in dots per inch
        float y_resolution = -1;//!< y resolution in dots per inch
        qint32 tattoo;          //!< (unique identifier?)
        quint32 unit;           //!< Units of The GIMP (inch, mm, pica, etc...)
        qint32 num_colors = 0;  //!< number of colors in an indexed image
        QVector<QRgb> palette;          //!< indexed image color palette

        int num_layers;         //!< number of layers
        Layer layer;            //!< most recently read layer

        bool initialized;       //!< Is the QImage initialized?
        QImage image;           //!< final QImage

        XCFImage(void) : initialized(false) {}
    };

    //! In layer DISSOLVE mode, a random number is chosen to compare to a
    //! pixel's alpha. If the alpha is greater than the random number, the
    //! pixel is drawn. This table merely contains the random number seeds
    //! for each ROW of an image. Therefore, the random numbers chosen
    //! are consistent from run to run.
    static int random_table[RANDOM_TABLE_SIZE];
    static bool random_table_initialized;

    //! This table is used as a shared grayscale ramp to be set on grayscale
    //! images. This is because Qt does not differentiate between indexed and
    //! grayscale images.
    static QVector<QRgb> grayTable;

    //! This table provides the add_pixel saturation values (i.e. 250 + 250 = 255).
    //static int add_lut[256][256]; - this is so lame waste of 256k of memory
    static int add_lut(int, int);

    //! The bottom-most layer is copied into the final QImage by this
    //! routine.
    typedef void (*PixelCopyOperation)(const Layer &layer, uint i, uint j, int k, int l,
                                       QImage &image, int m, int n);

    //! Higher layers are merged into the final QImage by this routine.
    typedef void (*PixelMergeOperation)(const Layer &layer, uint i, uint j, int k, int l,
                                        QImage &image, int m, int n);

    //! Layer mode static data.
    typedef struct {
        bool affect_alpha;      //!< Does this mode affect the source alpha?
    } LayerModes;

    //! Array of layer mode structures for the modes described by
    //! LayerModeEffects.
    static const LayerModes layer_modes[];

    bool loadImageProperties(QDataStream &xcf_io, XCFImage &image);
    bool loadProperty(QDataStream &xcf_io, PropType &type, QByteArray &bytes, quint32 &rawType);
    bool loadLayer(QDataStream &xcf_io, XCFImage &xcf_image);
    bool loadLayerProperties(QDataStream &xcf_io, Layer &layer);
    bool composeTiles(XCFImage &xcf_image);
    void setGrayPalette(QImage &image);
    void setPalette(XCFImage &xcf_image, QImage &image);
    static void assignImageBytes(Layer &layer, uint i, uint j);
    bool loadHierarchy(QDataStream &xcf_io, Layer &layer);
    bool loadLevel(QDataStream &xcf_io, Layer &layer, qint32 bpp);
    static void assignMaskBytes(Layer &layer, uint i, uint j);
    bool loadMask(QDataStream &xcf_io, Layer &layer);
    bool loadChannelProperties(QDataStream &xcf_io, Layer &layer);
    bool initializeImage(XCFImage &xcf_image);
    bool loadTileRLE(QDataStream &xcf_io, uchar *tile, int size,
                     int data_length, qint32 bpp);

    static void copyLayerToImage(XCFImage &xcf_image);
    static void copyRGBToRGB(const Layer &layer, uint i, uint j, int k, int l,
                             QImage &image, int m, int n);
    static void copyGrayToGray(const Layer &layer, uint i, uint j, int k, int l,
                               QImage &image, int m, int n);
    static void copyGrayToRGB(const Layer &layer, uint i, uint j, int k, int l,
                              QImage &image, int m, int n);
    static void copyGrayAToRGB(const Layer &layer, uint i, uint j, int k, int l,
                               QImage &image, int m, int n);
    static void copyIndexedToIndexed(const Layer &layer, uint i, uint j, int k, int l,
                                     QImage &image, int m, int n);
    static void copyIndexedAToIndexed(const Layer &layer, uint i, uint j, int k, int l,
                                      QImage &image, int m, int n);
    static void copyIndexedAToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                  QImage &image, int m, int n);

    static void mergeLayerIntoImage(XCFImage &xcf_image);
    static void mergeRGBToRGB(const Layer &layer, uint i, uint j, int k, int l,
                              QImage &image, int m, int n);
    static void mergeGrayToGray(const Layer &layer, uint i, uint j, int k, int l,
                                QImage &image, int m, int n);
    static void mergeGrayAToGray(const Layer &layer, uint i, uint j, int k, int l,
                                 QImage &image, int m, int n);
    static void mergeGrayToRGB(const Layer &layer, uint i, uint j, int k, int l,
                               QImage &image, int m, int n);
    static void mergeGrayAToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                QImage &image, int m, int n);
    static void mergeIndexedToIndexed(const Layer &layer, uint i, uint j, int k, int l,
                                      QImage &image, int m, int n);
    static void mergeIndexedAToIndexed(const Layer &layer, uint i, uint j, int k, int l,
                                       QImage &image, int m, int n);
    static void mergeIndexedAToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                   QImage &image, int m, int n);

    static void initializeRandomTable();
    static void dissolveRGBPixels(QImage &image, int x, int y);
    static void dissolveAlphaPixels(QImage &image, int x, int y);
};

int XCFImageFormat::random_table[RANDOM_TABLE_SIZE];
bool XCFImageFormat::random_table_initialized;

QVector<QRgb> XCFImageFormat::grayTable;

template <typename T, size_t N>
constexpr size_t countof(T(&)[N])
{
    return N;
}

const XCFImageFormat::LayerModes XCFImageFormat::layer_modes[] = {
    {true},     // NORMAL_MODE
    {true},     // DISSOLVE_MODE
    {true},     // BEHIND_MODE
    {false},    // MULTIPLY_MODE
    {false},    // SCREEN_MODE
    {false},    // OVERLAY_MODE
    {false},    // DIFFERENCE_MODE
    {false},    // ADDITION_MODE
    {false},    // SUBTRACT_MODE
    {false},    // DARKEN_ONLY_MODE
    {false},    // LIGHTEN_ONLY_MODE
    {false},    // HUE_MODE
    {false},    // SATURATION_MODE
    {false},    // COLOR_MODE
    {false},    // VALUE_MODE
    {false},    // DIVIDE_MODE
    {false},    // DODGE_MODE
    {false},    // BURN_MODE
    {false},    // HARDLIGHT_MODE
    {false},    // SOFTLIGHT_MODE
    {false},    // GRAIN_EXTRACT_MODE
    {false},    // GRAIN_MERGE_MODE
};

//! Change a QRgb value's alpha only.
inline QRgb qRgba(const QRgb rgb, int a)
{
    return ((a & 0xff) << 24 | (rgb & RGB_MASK));
}

/*!
 * The constructor for the XCF image loader.
 */
XCFImageFormat::XCFImageFormat()
{
}

/*!
 * This initializes the tables used in the layer dissolving routines.
 */
void XCFImageFormat::initializeRandomTable()
{
    // From GIMP "paint_funcs.c" v1.2
    srand(RANDOM_SEED);

    for (int i = 0; i < RANDOM_TABLE_SIZE; i++) {
        random_table[i] = rand();
    }

    for (int i = 0; i < RANDOM_TABLE_SIZE; i++) {
        int tmp;
        int swap = i + rand() % (RANDOM_TABLE_SIZE - i);
        tmp = random_table[i];
        random_table[i] = random_table[swap];
        random_table[swap] = tmp;
    }
}

inline
int XCFImageFormat::add_lut(int a, int b)
{
    return qMin(a + b, 255);
}

bool XCFImageFormat::readXCF(QIODevice *device, QImage *outImage)
{
    XCFImage xcf_image;
    QDataStream xcf_io(device);

    char tag[14];;

    if (xcf_io.readRawData(tag, sizeof(tag)) != sizeof(tag)) {
//             qDebug() << "XCF: read failure on header tag";
        return false;
    }
    if (qstrncmp(tag, "gimp xcf", 8) != 0) {
//             qDebug() << "XCF: read called on non-XCF file";
        return false;
    }

    xcf_io >> xcf_image.width >> xcf_image.height >> xcf_image.type;

//  qDebug() << tag << " " << xcf_image.width << " " << xcf_image.height << " " <<  xcf_image.type;
    if (!loadImageProperties(xcf_io, xcf_image)) {
        return false;
    }

    // The layers appear to be stored in top-to-bottom order. This is
    // the reverse of how a merged image must be computed. So, the layer
    // offsets are pushed onto a LIFO stack (thus, we don't have to load
    // all the data of all layers before beginning to construct the
    // merged image).

    QStack<qint32> layer_offsets;

    while (true) {
        qint32 layer_offset;

        xcf_io >> layer_offset;

        if (layer_offset == 0) {
            break;
        }

        layer_offsets.push(layer_offset);
    }

    xcf_image.num_layers = layer_offsets.size();

    if (layer_offsets.size() == 0) {
//      qDebug() << "XCF: no layers!"; return false;
    }

    // Load each layer and add it to the image
    while (!layer_offsets.isEmpty()) {
        qint32 layer_offset = layer_offsets.pop();

        xcf_io.device()->seek(layer_offset);

        if (!loadLayer(xcf_io, xcf_image)) {
            return false;
        }
    }

    if (!xcf_image.initialized) {
//      qDebug() << "XCF: no visible layers!";
        return false;
    }

    *outImage = xcf_image.image;
    return true;
}

/*!
 * An XCF file can contain an arbitrary number of properties associated
 * with the image (and layer and mask).
 * \param xcf_io the data stream connected to the XCF image
 * \param xcf_image XCF image data.
 * \return true if there were no I/O errors.
 */
bool XCFImageFormat::loadImageProperties(QDataStream &xcf_io, XCFImage &xcf_image)
{
    while (true) {
        PropType type;
        QByteArray bytes;
        quint32 rawType;

        if (!loadProperty(xcf_io, type, bytes, rawType)) {
//          qDebug() << "XCF: error loading global image properties";
            return false;
        }

        QDataStream property(bytes);

        switch (type) {
        case PROP_END:
            return true;

        case PROP_COMPRESSION:
            property >> xcf_image.compression;
            break;

        case PROP_RESOLUTION:
            property.setFloatingPointPrecision(QDataStream::SinglePrecision);
            property >> xcf_image.x_resolution >> xcf_image.y_resolution;
            break;

        case PROP_TATTOO:
            property >> xcf_image.tattoo;
            break;

        case PROP_PARASITES:
            while (!property.atEnd()) {
                char *tag;
                quint32 size;

                property.readBytes(tag, size);

                quint32 flags;
                char *data = nullptr;
                property >> flags >> data;

                if (tag && strncmp(tag, "gimp-comment", strlen("gimp-comment")) == 0) {
                    xcf_image.image.setText(QStringLiteral("Comment"), QString::fromUtf8(data));
                }

                delete[] tag;
                delete[] data;
            }
            break;

        case PROP_UNIT:
            property >> xcf_image.unit;
            break;

        case PROP_PATHS:    // This property is ignored.
            break;

        case PROP_USER_UNIT:    // This property is ignored.
            break;

        case PROP_COLORMAP:
            property >> xcf_image.num_colors;
            if (xcf_image.num_colors < 0 || xcf_image.num_colors > 65535) {
                return false;
            }

            xcf_image.palette = QVector<QRgb>();
            xcf_image.palette.reserve(xcf_image.num_colors);

            for (int i = 0; i < xcf_image.num_colors; i++) {
                uchar r, g, b;
                property >> r >> g >> b;
                xcf_image.palette.push_back(qRgb(r, g, b));
            }
            break;

        default:
//                  qDebug() << "XCF: unimplemented image property" << rawType
//                          << ", size " << bytes.size() << endl;
            break;
        }
    }
}

/*!
 * Read a single property from the image file. The property type is returned
 * in type and the data is returned in bytes.
 * \param xcf the image file data stream.
 * \param type returns with the property type.
 * \param bytes returns with the property data.
 * \return true if there were no IO errors.  */
bool XCFImageFormat::loadProperty(QDataStream &xcf_io, PropType &type, QByteArray &bytes, quint32 &rawType)
{
    quint32 size;

    xcf_io >> rawType;
    if (rawType >= MAX_SUPPORTED_PROPTYPE) {
        type = MAX_SUPPORTED_PROPTYPE;
        // we don't support the property, but we still need to read from the device, assume it's like all the
        // non custom properties that is data_length + data
        xcf_io >> size;
        xcf_io.skipRawData(size);
        // return true because we don't really want to totally fail on an unsupported property since it may not be fatal
        return true;
    }

    type = PropType(rawType);

    char *data = nullptr;

    // The colormap property size is not the correct number of bytes:
    // The GIMP source xcf.c has size = 4 + ncolors, but it should be
    // 4 + 3 * ncolors

    if (type == PROP_COLORMAP) {
        xcf_io >> size;
        quint32 ncolors;
        xcf_io >> ncolors;

        size = 3 * ncolors + 4;

        if (size > 65535 || size < 4) {
            return false;
        }

        data = new char[size];

        // since we already read "ncolors" from the stream, we put that data back
        data[0] = 0;
        data[1] = 0;
        data[2] = ncolors >> 8;
        data[3] = ncolors & 255;

        // ... and read the remaining bytes from the stream
        xcf_io.readRawData(data + 4, size - 4);
    } else if (type == PROP_USER_UNIT) {
        // The USER UNIT property size is not correct. I'm not sure why, though.
        float factor;
        qint32 digits;

        xcf_io >> size >> factor >> digits;

        for (int i = 0; i < 5; i++) {
            char *unit_strings;

            xcf_io >> unit_strings;

            delete[] unit_strings;

            if (xcf_io.device()->atEnd()) {
//              qDebug() << "XCF: read failure on property " << type;
                return false;
            }
        }

        size = 0;
    } else {
        xcf_io >> size;
        if (size > 256000) {
            return false;
        }
        data = new char[size];
        const quint32 dataRead = xcf_io.readRawData(data, size);
        if (dataRead < size) {
//          qDebug() << "XCF: loadProperty read less data than expected" << data_length << dataRead;
            memset(&data[dataRead], 0, size - dataRead);
        }
    }

    if (size != 0 && data) {
        bytes = QByteArray(data, size);
    }

    delete [] data;

    return true;
}

/*!
 * Load a layer from the XCF file. The data stream must be positioned at
 * the beginning of the layer data.
 * \param xcf_io the image file data stream.
 * \param xcf_image contains the layer and the color table
 * (if the image is indexed).
 * \return true if there were no I/O errors.
 */
bool XCFImageFormat::loadLayer(QDataStream &xcf_io, XCFImage &xcf_image)
{
    Layer &layer(xcf_image.layer);
    delete[] layer.name;

    xcf_io >> layer.width >> layer.height >> layer.type >> layer.name;

    if (!loadLayerProperties(xcf_io, layer)) {
        return false;
    }
#if 0
    cout << "layer: \"" << layer.name << "\", size: " << layer.width << " x "
         << layer.height << ", type: " << layer.type << ", mode: " << layer.mode
         << ", opacity: " << layer.opacity << ", visible: " << layer.visible
         << ", offset: " << layer.x_offset << ", " << layer.y_offset << endl;
#endif
    // Skip reading the rest of it if it is not visible. Typically, when
    // you export an image from the The GIMP it flattens (or merges) only
    // the visible layers into the output image.

    if (layer.visible == 0) {
        return true;
    }

    // If there are any more layers, merge them into the final QImage.

    xcf_io >> layer.hierarchy_offset >> layer.mask_offset;

    // Allocate the individual tile QImages based on the size and type
    // of this layer.

    if (!composeTiles(xcf_image)) {
        return false;
    }
    xcf_io.device()->seek(layer.hierarchy_offset);

    // As tiles are loaded, they are copied into the layers tiles by
    // this routine. (loadMask(), below, uses a slightly different
    // version of assignBytes().)

    layer.assignBytes = assignImageBytes;

    if (!loadHierarchy(xcf_io, layer)) {
        return false;
    }

    if (layer.mask_offset != 0) {
        // 9 means its not on the file. Spec says "If the property does not appear for a layer which has a layer mask, it defaults to true (1).
        if (layer.apply_mask == 9) {
            layer.apply_mask = 1;
        }

        xcf_io.device()->seek(layer.mask_offset);

        if (!loadMask(xcf_io, layer)) {
            return false;
        }
    } else {
        // Spec says "Robust readers should force this to false if the layer has no layer mask."
        layer.apply_mask = 0;
    }

    // Now we should have enough information to initialize the final
    // QImage. The first visible layer determines the attributes
    // of the QImage.

    if (!xcf_image.initialized) {
        if (!initializeImage(xcf_image)) {
            return false;
        }
        copyLayerToImage(xcf_image);
        xcf_image.initialized = true;
    } else {
        mergeLayerIntoImage(xcf_image);
    }

    return true;
}

/*!
 * An XCF file can contain an arbitrary number of properties associated
 * with a layer.
 * \param xcf_io the data stream connected to the XCF image.
 * \param layer layer to collect the properties.
 * \return true if there were no I/O errors.
 */
bool XCFImageFormat::loadLayerProperties(QDataStream &xcf_io, Layer &layer)
{
    while (true) {
        PropType type;
        QByteArray bytes;
        quint32 rawType;

        if (!loadProperty(xcf_io, type, bytes, rawType)) {
//          qDebug() << "XCF: error loading layer properties";
            return false;
        }

        QDataStream property(bytes);

        switch (type) {
        case PROP_END:
            return true;

        case PROP_ACTIVE_LAYER:
            layer.active = true;
            break;

        case PROP_OPACITY:
            property >> layer.opacity;
            layer.opacity = std::min(layer.opacity, 255u);
            break;

        case PROP_VISIBLE:
            property >> layer.visible;
            break;

        case PROP_LINKED:
            property >> layer.linked;
            break;

        case PROP_PRESERVE_TRANSPARENCY:
            property >> layer.preserve_transparency;
            break;

        case PROP_APPLY_MASK:
            property >> layer.apply_mask;
            break;

        case PROP_EDIT_MASK:
            property >> layer.edit_mask;
            break;

        case PROP_SHOW_MASK:
            property >> layer.show_mask;
            break;

        case PROP_OFFSETS:
            property >> layer.x_offset >> layer.y_offset;
            break;

        case PROP_MODE:
            property >> layer.mode;
            if (layer.mode >= countof(layer_modes)) {
                qWarning() << "Found layer with unsupported mode" << layer.mode << "Defaulting to mode 0";
                layer.mode = 0;
            }
            break;

        case PROP_TATTOO:
            property >> layer.tattoo;
            break;

        default:
//              qDebug() << "XCF: unimplemented layer property " << rawType
//                      << ", size " << bytes.size() << endl;
            break;
        }
    }
}

/*!
 * Compute the number of tiles in the current layer and allocate
 * QImage structures for each of them.
 * \param xcf_image contains the current layer.
 */
bool XCFImageFormat::composeTiles(XCFImage &xcf_image)
{
    Layer &layer(xcf_image.layer);

    layer.nrows = (layer.height + TILE_HEIGHT - 1) / TILE_HEIGHT;
    layer.ncols = (layer.width + TILE_WIDTH - 1) / TILE_WIDTH;

    //qDebug() << "IMAGE: height=" << xcf_image.height << ", width=" << xcf_image.width;
    //qDebug() << "LAYER: height=" << layer.height << ", width=" << layer.width;
    //qDebug() << "LAYER: rows=" << layer.nrows << ", columns=" << layer.ncols;

    // SANITY CHECK: Catch corrupted XCF image file where the width or height
    // of a tile is reported are bogus. See Bug# 234030.
    if (layer.width > 32767 || layer.height > 32767
        || (sizeof(void *) == 4 && layer.width * layer.height > 16384 * 16384)) {
        return false;
    }

    layer.image_tiles.resize(layer.nrows);

    if (layer.type == GRAYA_GIMAGE || layer.type == INDEXEDA_GIMAGE) {
        layer.alpha_tiles.resize(layer.nrows);
    }

    if (layer.mask_offset != 0) {
        layer.mask_tiles.resize(layer.nrows);
    }

    for (uint j = 0; j < layer.nrows; j++) {
        layer.image_tiles[j].resize(layer.ncols);

        if (layer.type == GRAYA_GIMAGE || layer.type == INDEXEDA_GIMAGE) {
            layer.alpha_tiles[j].resize(layer.ncols);
        }

        if (layer.mask_offset != 0) {
            layer.mask_tiles[j].resize(layer.ncols);
        }
    }

    for (uint j = 0; j < layer.nrows; j++) {
        for (uint i = 0; i < layer.ncols; i++) {

            uint tile_width = (i + 1) * TILE_WIDTH <= layer.width
                              ? TILE_WIDTH : layer.width - i * TILE_WIDTH;

            uint tile_height = (j + 1) * TILE_HEIGHT <= layer.height
                               ? TILE_HEIGHT : layer.height - j * TILE_HEIGHT;

            // Try to create the most appropriate QImage (each GIMP layer
            // type is treated slightly differently)

            switch (layer.type) {
            case RGB_GIMAGE:
                layer.image_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_RGB32);
                layer.image_tiles[j][i].setColorCount(0);
                if (layer.image_tiles[j][i].isNull()) {
                    return false;
                }
                break;

            case RGBA_GIMAGE:
                layer.image_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_ARGB32);
                layer.image_tiles[j][i].setColorCount(0);
                if (layer.image_tiles[j][i].isNull()) {
                    return false;
                }
                break;

            case GRAY_GIMAGE:
                layer.image_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_Indexed8);
                layer.image_tiles[j][i].setColorCount(256);
                if (layer.image_tiles[j][i].isNull()) {
                    return false;
                }
                setGrayPalette(layer.image_tiles[j][i]);
                break;

            case GRAYA_GIMAGE:
                layer.image_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_Indexed8);
                layer.image_tiles[j][i].setColorCount(256);
                if (layer.image_tiles[j][i].isNull()) {
                    return false;
                }
                setGrayPalette(layer.image_tiles[j][i]);

                layer.alpha_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_Indexed8);
                layer.alpha_tiles[j][i].setColorCount(256);
                if (layer.alpha_tiles[j][i].isNull()) {
                    return false;
                }
                setGrayPalette(layer.alpha_tiles[j][i]);
                break;

            case INDEXED_GIMAGE:
                layer.image_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_Indexed8);
                layer.image_tiles[j][i].setColorCount(xcf_image.num_colors);
                if (layer.image_tiles[j][i].isNull()) {
                    return false;
                }
                setPalette(xcf_image, layer.image_tiles[j][i]);
                break;

            case INDEXEDA_GIMAGE:
                layer.image_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_Indexed8);
                layer.image_tiles[j][i].setColorCount(xcf_image.num_colors);
                if (layer.image_tiles[j][i].isNull()) {
                    return false;
                }
                setPalette(xcf_image, layer.image_tiles[j][i]);

                layer.alpha_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_Indexed8);
                layer.alpha_tiles[j][i].setColorCount(256);
                if (layer.alpha_tiles[j][i].isNull()) {
                    return false;
                }
                setGrayPalette(layer.alpha_tiles[j][i]);
            }

            if (layer.mask_offset != 0) {
                layer.mask_tiles[j][i] = QImage(tile_width, tile_height, QImage::Format_Indexed8);
                layer.mask_tiles[j][i].setColorCount(256);
                if (layer.mask_tiles[j][i].isNull()) {
                    return false;
                }
                setGrayPalette(layer.mask_tiles[j][i]);
            }
        }
    }
    return true;
}

/*!
 * Apply a grayscale palette to the QImage. Note that Qt does not distinguish
 * between grayscale and indexed images. A grayscale image is just
 * an indexed image with a 256-color, grayscale palette.
 * \param image image to set to a grayscale palette.
 */
void XCFImageFormat::setGrayPalette(QImage &image)
{
    if (grayTable.isEmpty()) {
        grayTable.resize(256);

        for (int i = 0; i < 256; i++) {
            grayTable[i] = qRgb(i, i, i);
        }
    }

    image.setColorTable(grayTable);
}

/*!
 * Copy the indexed palette from the XCF image into the QImage.
 * \param xcf_image XCF image containing the palette read from the data stream.
 * \param image image to apply the palette to.
 */
void XCFImageFormat::setPalette(XCFImage &xcf_image, QImage &image)
{
    Q_ASSERT(xcf_image.num_colors == xcf_image.palette.size());

    image.setColorTable(xcf_image.palette);
}

/*!
 * Copy the bytes from the tile buffer into the image tile QImage, taking into
 * account all the myriad different modes.
 * \param layer layer containing the tile buffer and the image tile matrix.
 * \param i column index of current tile.
 * \param j row index of current tile.
 */
void XCFImageFormat::assignImageBytes(Layer &layer, uint i, uint j)
{
    QImage &image = layer.image_tiles[j][i];
    const uchar *tile = layer.tile;
    const int width = image.width();
    const int height = image.height();
    const int bytesPerLine = image.bytesPerLine();
    uchar *bits = image.bits();

    switch (layer.type) {
    case RGB_GIMAGE:
        for (int y = 0; y < height; y++) {
            QRgb *dataPtr = (QRgb *)(bits + y * bytesPerLine);
            for (int x = 0; x < width; x++) {
                *dataPtr++ = qRgb(tile[0], tile[1], tile[2]);
                tile += sizeof(QRgb);
            }
        }
        break;

    case RGBA_GIMAGE:
        for (int y = 0; y < height; y++) {
            QRgb *dataPtr = (QRgb *)(bits + y * bytesPerLine);
            for (int x = 0; x < width; x++) {
                *dataPtr++ =    qRgba(tile[0], tile[1], tile[2], tile[3]);
                tile += sizeof(QRgb);
            }
        }
        break;

    case GRAY_GIMAGE:
    case INDEXED_GIMAGE:
        for (int y = 0; y < height; y++) {
            uchar *dataPtr = bits + y * bytesPerLine;
            for (int x = 0; x < width; x++) {
                *dataPtr++ = tile[0];
                tile += sizeof(QRgb);
            }
        }
        break;

    case GRAYA_GIMAGE:
    case INDEXEDA_GIMAGE:
        for (int y = 0; y < height; y++) {
            uchar *dataPtr = bits + y * bytesPerLine;
            uchar *alphaPtr = layer.alpha_tiles[j][i].scanLine(y);
            for (int x = 0; x < width; x++) {

                // The "if" here should not be necessary, but apparently there
                // are some cases where the image can contain larger indices
                // than there are colors in the palette. (A bug in The GIMP?)

                if (tile[0] < image.colorCount()) {
                    *dataPtr = tile[0];
                }

                *alphaPtr = tile[1];
                dataPtr += 1;
                alphaPtr += 1;
                tile += sizeof(QRgb);
            }
        }
        break;
    }
}

/*!
 * The GIMP stores images in a "mipmap"-like hierarchy. As far as the QImage
 * is concerned, however, only the top level (i.e., the full resolution image)
 * is used.
 * \param xcf_io the data stream connected to the XCF image.
 * \param layer the layer to collect the image.
 * \return true if there were no I/O errors.
 */
bool XCFImageFormat::loadHierarchy(QDataStream &xcf_io, Layer &layer)
{
    qint32 width;
    qint32 height;
    qint32 bpp;
    quint32 offset;

    xcf_io >> width >> height >> bpp >> offset;

    // make sure bpp is correct and complain if it is not
    switch (layer.type) {
        case RGB_GIMAGE:
            if (bpp != 3) {
                qWarning() << "Found layer of type RGB but with bpp != 3" << bpp;
                bpp = 3;
            }
            break;
        case RGBA_GIMAGE:
            if (bpp != 4) {
                qWarning() << "Found layer of type RGBA but with bpp != 4" << bpp;
                bpp = 4;
            }
            break;
        case GRAY_GIMAGE:
            if (bpp != 1) {
                qWarning() << "Found layer of type Gray but with bpp != 1" << bpp;
                bpp = 1;
            }
            break;
        case GRAYA_GIMAGE:
            if (bpp != 2) {
                qWarning() << "Found layer of type Gray+Alpha but with bpp != 2" << bpp;
                bpp = 2;
            }
            break;
        case INDEXED_GIMAGE:
            if (bpp != 1) {
                qWarning() << "Found layer of type Indexed but with bpp != 1" << bpp;
                bpp = 1;
            }
            break;
        case INDEXEDA_GIMAGE:
            if (bpp != 2) {
                qWarning() << "Found layer of type Indexed+Alpha but with bpp != 2" << bpp;
                bpp = 2;
            }
            break;
    }

    // GIMP stores images in a "mipmap"-like format (multiple levels of
    // increasingly lower resolution). Only the top level is used here,
    // however.

    quint32 junk;
    do {
        xcf_io >> junk;

        if (xcf_io.device()->atEnd()) {
//          qDebug() << "XCF: read failure on layer " << layer.name << " level offsets";
            return false;
        }
    } while (junk != 0);

    qint64 saved_pos = xcf_io.device()->pos();

    xcf_io.device()->seek(offset);
    if (!loadLevel(xcf_io, layer, bpp)) {
        return false;
    }

    xcf_io.device()->seek(saved_pos);
    return true;
}

/*!
 * Load one level of the image hierarchy (but only the top level is ever used).
 * \param xcf_io the data stream connected to the XCF image.
 * \param layer the layer to collect the image.
 * \param bpp the number of bytes in a pixel.
 * \return true if there were no I/O errors.
 * \sa loadTileRLE().
 */
bool XCFImageFormat::loadLevel(QDataStream &xcf_io, Layer &layer, qint32 bpp)
{
    qint32 width;
    qint32 height;
    quint32 offset;

    xcf_io >> width >> height >> offset;

    if (offset == 0) {
        // offset 0 with rowsxcols != 0 is probably an error since it means we have tiles
        // without data but just clear the bits for now instead of returning false
        for (uint j = 0; j < layer.nrows; j++) {
            for (uint i = 0; i < layer.ncols; i++) {
                layer.image_tiles[j][i].fill(Qt::transparent);
                if (layer.type == GRAYA_GIMAGE || layer.type == INDEXEDA_GIMAGE) {
                    layer.alpha_tiles[j][i].fill(Qt::transparent);
                }
            }
        }
        return true;
    }

    for (uint j = 0; j < layer.nrows; j++) {
        for (uint i = 0; i < layer.ncols; i++) {

            if (offset == 0) {
//              qDebug() << "XCF: incorrect number of tiles in layer " << layer.name;
                return false;
            }

            qint64 saved_pos = xcf_io.device()->pos();
            quint32 offset2;
            xcf_io >> offset2;

            // Evidently, RLE can occasionally expand a tile instead of compressing it!

            if (offset2 == 0) {
                offset2 = offset + (uint)(TILE_WIDTH * TILE_HEIGHT * 4 * 1.5);
            }

            xcf_io.device()->seek(offset);
            int size = layer.image_tiles[j][i].width() * layer.image_tiles[j][i].height();

            if (!loadTileRLE(xcf_io, layer.tile, size, offset2 - offset, bpp)) {
                return false;
            }

            // The bytes in the layer tile are juggled differently depending on
            // the target QImage. The caller has set layer.assignBytes to the
            // appropriate routine.

            layer.assignBytes(layer, i, j);

            xcf_io.device()->seek(saved_pos);
            xcf_io >> offset;
        }
    }

    return true;
}

/*!
 * A layer can have a one channel image which is used as a mask.
 * \param xcf_io the data stream connected to the XCF image.
 * \param layer the layer to collect the mask image.
 * \return true if there were no I/O errors.
 */
bool XCFImageFormat::loadMask(QDataStream &xcf_io, Layer &layer)
{
    qint32 width;
    qint32 height;
    char *name;

    xcf_io >> width >> height >> name;

    delete name;

    if (!loadChannelProperties(xcf_io, layer)) {
        return false;
    }

    quint32 hierarchy_offset;
    xcf_io >> hierarchy_offset;

    xcf_io.device()->seek(hierarchy_offset);
    layer.assignBytes = assignMaskBytes;

    if (!loadHierarchy(xcf_io, layer)) {
        return false;
    }

    return true;
}

/*!
 * This is the routine for which all the other code is simply
 * infrastructure. Read the image bytes out of the file and
 * store them in the tile buffer. This is passed a full 32-bit deep
 * buffer, even if bpp is smaller. The caller can figure out what to
 * do with the bytes.
 *
 * The tile is stored in "channels", i.e. the red component of all
 * pixels, then the green component of all pixels, then blue then
 * alpha, or, for indexed images, the color indices of all pixels then
 * the alpha of all pixels.
 *
 * The data is compressed with "run length encoding". Some simple data
 * integrity checks are made.
 *
 * \param xcf_io the data stream connected to the XCF image.
 * \param tile the buffer to expand the RLE into.
 * \param image_size number of bytes expected to be in the image tile.
 * \param data_length number of bytes expected in the RLE.
 * \param bpp number of bytes per pixel.
 * \return true if there were no I/O errors and no obvious corruption of
 * the RLE data.
 */
bool XCFImageFormat::loadTileRLE(QDataStream &xcf_io, uchar *tile, int image_size,
                                 int data_length, qint32 bpp)
{
    uchar *data;

    uchar *xcfdata;
    uchar *xcfodata;
    uchar *xcfdatalimit;

    if (data_length < 0 || data_length > int(TILE_WIDTH * TILE_HEIGHT * 4 * 1.5)) {
//      qDebug() << "XCF: invalid tile data length" << data_length;
        return false;
    }

    xcfdata = xcfodata = new uchar[data_length];

    const int dataRead = xcf_io.readRawData((char *)xcfdata, data_length);
    if (dataRead < data_length) {
//      qDebug() << "XCF: read less data than expected" << data_length << dataRead;
        memset(&xcfdata[dataRead], 0, data_length - dataRead);
    }

    if (!xcf_io.device()->isOpen()) {
        delete[] xcfodata;
//      qDebug() << "XCF: read failure on tile";
        return false;
    }

    xcfdatalimit = &xcfodata[data_length - 1];

    for (int i = 0; i < bpp; ++i) {

        data = tile + i;

        int count = 0;
        int size = image_size;

        while (size > 0) {
            if (xcfdata > xcfdatalimit) {
                goto bogus_rle;
            }

            uchar val = *xcfdata++;
            uint length = val;

            if (length >= 128) {
                length = 255 - (length - 1);
                if (length == 128) {
                    if (xcfdata >= xcfdatalimit) {
                        goto bogus_rle;
                    }

                    length = (*xcfdata << 8) + xcfdata[1];

                    xcfdata += 2;
                }

                count += length;
                size -= length;

                if (size < 0) {
                    goto bogus_rle;
                }

                if (&xcfdata[length - 1] > xcfdatalimit) {
                    goto bogus_rle;
                }

                while (length-- > 0) {
                    *data = *xcfdata++;
                    data += sizeof(QRgb);
                }
            } else {
                length += 1;
                if (length == 128) {
                    if (xcfdata >= xcfdatalimit) {
                        goto bogus_rle;
                    }

                    length = (*xcfdata << 8) + xcfdata[1];
                    xcfdata += 2;
                }

                count += length;
                size -= length;

                if (size < 0) {
                    goto bogus_rle;
                }

                if (xcfdata > xcfdatalimit) {
                    goto bogus_rle;
                }

                val = *xcfdata++;

                while (length-- > 0) {
                    *data = val;
                    data += sizeof(QRgb);
                }
            }
        }
    }

    delete[] xcfodata;
    return true;

bogus_rle:

//  qDebug() << "The run length encoding could not be decoded properly";
    delete[] xcfodata;
    return false;
}

/*!
 * An XCF file can contain an arbitrary number of properties associated
 * with a channel. Note that this routine only reads mask channel properties.
 * \param xcf_io the data stream connected to the XCF image.
 * \param layer layer containing the mask channel to collect the properties.
 * \return true if there were no I/O errors.
 */
bool XCFImageFormat::loadChannelProperties(QDataStream &xcf_io, Layer &layer)
{
    while (true) {
        PropType type;
        QByteArray bytes;
        quint32 rawType;

        if (!loadProperty(xcf_io, type, bytes, rawType)) {
//          qDebug() << "XCF: error loading channel properties";
            return false;
        }

        QDataStream property(bytes);

        switch (type) {
        case PROP_END:
            return true;

        case PROP_OPACITY:
            property >> layer.mask_channel.opacity;
            layer.mask_channel.opacity = std::min(layer.mask_channel.opacity, 255u);
            break;

        case PROP_VISIBLE:
            property >> layer.mask_channel.visible;
            break;

        case PROP_SHOW_MASKED:
            property >> layer.mask_channel.show_masked;
            break;

        case PROP_COLOR:
            property >> layer.mask_channel.red >> layer.mask_channel.green
                     >> layer.mask_channel.blue;
            break;

        case PROP_TATTOO:
            property >> layer.mask_channel.tattoo;
            break;

        default:
//              qDebug() << "XCF: unimplemented channel property " << rawType
//                      << ", size " << bytes.size() << endl;
            break;
        }
    }
}

/*!
 * Copy the bytes from the tile buffer into the mask tile QImage.
 * \param layer layer containing the tile buffer and the mask tile matrix.
 * \param i column index of current tile.
 * \param j row index of current tile.
 */
void XCFImageFormat::assignMaskBytes(Layer &layer, uint i, uint j)
{
    QImage &image = layer.mask_tiles[j][i];
    uchar *tile = layer.tile;
    const int width = image.width();
    const int height = image.height();
    const int bytesPerLine = image.bytesPerLine();
    uchar *bits = image.bits();

    for (int y = 0; y < height; y++) {
        uchar *dataPtr = bits + y * bytesPerLine;
        for (int x = 0; x < width; x++) {
            *dataPtr++ = tile[0];
            tile += sizeof(QRgb);
        }
    }
}

/*!
 * Construct the QImage which will eventually be returned to the QImage
 * loader.
 *
 * There are a couple of situations which require that the QImage is not
 * exactly the same as The GIMP's representation. The full table is:
 * \verbatim
 *  Grayscale  opaque      :  8 bpp indexed
 *  Grayscale  translucent : 32 bpp + alpha
 *  Indexed    opaque      :  1 bpp if num_colors <= 2
 *                         :  8 bpp indexed otherwise
 *  Indexed    translucent :  8 bpp indexed + alpha if num_colors < 256
 *                         : 32 bpp + alpha otherwise
 *  RGB        opaque      : 32 bpp
 *  RGBA       translucent : 32 bpp + alpha
 * \endverbatim
 * Whether the image is translucent or not is determined by the bottom layer's
 * alpha channel. However, even if the bottom layer lacks an alpha channel,
 * it can still have an opacity < 1. In this case, the QImage is promoted
 * to 32-bit. (Note this is different from the output from the GIMP image
 * exporter, which seems to ignore this attribute.)
 *
 * Independently, higher layers can be translucent, but the background of
 * the image will not show through if the bottom layer is opaque.
 *
 * For indexed images, translucency is an all or nothing effect.
 * \param xcf_image contains image info and bottom-most layer.
 */
bool XCFImageFormat::initializeImage(XCFImage &xcf_image)
{
    // (Aliases to make the code look a little better.)
    Layer &layer(xcf_image.layer);
    QImage &image(xcf_image.image);

    switch (layer.type) {
    case RGB_GIMAGE:
        if (layer.opacity == OPAQUE_OPACITY) {
            image = QImage(xcf_image.width, xcf_image.height, QImage::Format_RGB32);
            if (image.isNull()) {
                return false;
            }
            image.fill(qRgb(255, 255, 255));
            break;
        } // else, fall through to 32-bit representation
        Q_FALLTHROUGH();
    case RGBA_GIMAGE:
        image = QImage(xcf_image.width, xcf_image.height, QImage::Format_ARGB32);
        if (image.isNull()) {
            return false;
        }
        image.fill(qRgba(255, 255, 255, 0));
        break;

    case GRAY_GIMAGE:
        if (layer.opacity == OPAQUE_OPACITY) {
            image = QImage(xcf_image.width, xcf_image.height, QImage::Format_Indexed8);
            image.setColorCount(256);
            if (image.isNull()) {
                return false;
            }
            setGrayPalette(image);
            image.fill(255);
            break;
        } // else, fall through to 32-bit representation
        Q_FALLTHROUGH();
    case GRAYA_GIMAGE:
        image = QImage(xcf_image.width, xcf_image.height, QImage::Format_ARGB32);
        if (image.isNull()) {
            return false;
        }
        image.fill(qRgba(255, 255, 255, 0));
        break;

    case INDEXED_GIMAGE:
        // As noted in the table above, there are quite a few combinations
        // which are possible with indexed images, depending on the
        // presence of transparency (note: not translucency, which is not
        // supported by The GIMP for indexed images) and the number of
        // individual colors.

        // Note: Qt treats a bitmap with a Black and White color palette
        // as a mask, so only the "on" bits are drawn, regardless of the
        // order color table entries. Otherwise (i.e., at least one of the
        // color table entries is not black or white), it obeys the one-
        // or two-color palette. Have to ask about this...

        if (xcf_image.num_colors <= 2) {
            image = QImage(xcf_image.width, xcf_image.height, QImage::Format_MonoLSB);
            image.setColorCount(xcf_image.num_colors);
            if (image.isNull()) {
                return false;
            }
            image.fill(0);
            setPalette(xcf_image, image);
        } else if (xcf_image.num_colors <= 256) {
            image = QImage(xcf_image.width, xcf_image.height, QImage::Format_Indexed8);
            image.setColorCount(xcf_image.num_colors);
            if (image.isNull()) {
                return false;
            }
            image.fill(0);
            setPalette(xcf_image, image);
        }
        break;

    case INDEXEDA_GIMAGE:
        if (xcf_image.num_colors == 1) {
            // Plenty(!) of room to add a transparent color
            xcf_image.num_colors++;
            xcf_image.palette.resize(xcf_image.num_colors);
            xcf_image.palette[1] = xcf_image.palette[0];
            xcf_image.palette[0] = qRgba(255, 255, 255, 0);

            image = QImage(xcf_image.width, xcf_image.height, QImage::Format_MonoLSB);
            image.setColorCount(xcf_image.num_colors);
            if (image.isNull()) {
                return false;
            }
            image.fill(0);
            setPalette(xcf_image, image);
        } else if (xcf_image.num_colors < 256) {
            // Plenty of room to add a transparent color
            xcf_image.num_colors++;
            xcf_image.palette.resize(xcf_image.num_colors);
            for (int c = xcf_image.num_colors - 1; c >= 1; c--) {
                xcf_image.palette[c] = xcf_image.palette[c - 1];
            }

            xcf_image.palette[0] = qRgba(255, 255, 255, 0);
            image = QImage(xcf_image.width, xcf_image.height, QImage::Format_Indexed8);
            image.setColorCount(xcf_image.num_colors);
            if (image.isNull()) {
                return false;
            }
            image.fill(0);
            setPalette(xcf_image, image);
        } else {
            // No room for a transparent color, so this has to be promoted to
            // true color. (There is no equivalent PNG representation output
            // from The GIMP as of v1.2.)
            image = QImage(xcf_image.width, xcf_image.height, QImage::Format_ARGB32);
            if (image.isNull()) {
                return false;
            }
            image.fill(qRgba(255, 255, 255, 0));
        }
        break;
    }

    if (xcf_image.x_resolution > 0 && xcf_image.y_resolution > 0) {
        const float dpmx = xcf_image.x_resolution * INCHESPERMETER;
        if (dpmx > std::numeric_limits<int>::max())
            return false;
        const float dpmy = xcf_image.y_resolution * INCHESPERMETER;
        if (dpmy > std::numeric_limits<int>::max())
            return false;
        image.setDotsPerMeterX((int)dpmx);
        image.setDotsPerMeterY((int)dpmy);
    }
    return true;
}

/*!
 * Copy a layer into an image, taking account of the manifold modes. The
 * contents of the image are replaced.
 * \param xcf_image contains the layer and image to be replaced.
 */
void XCFImageFormat::copyLayerToImage(XCFImage &xcf_image)
{
    Layer &layer(xcf_image.layer);
    QImage &image(xcf_image.image);
    PixelCopyOperation copy = nullptr;

    switch (layer.type) {
    case RGB_GIMAGE:
    case RGBA_GIMAGE:
        copy = copyRGBToRGB;
        break;
    case GRAY_GIMAGE:
        if (layer.opacity == OPAQUE_OPACITY) {
            copy = copyGrayToGray;
        } else {
            copy = copyGrayToRGB;
        }
        break;
    case GRAYA_GIMAGE:
        copy = copyGrayAToRGB;
        break;
    case INDEXED_GIMAGE:
        copy = copyIndexedToIndexed;
        break;
    case INDEXEDA_GIMAGE:
        if (xcf_image.image.depth() <= 8) {
            copy = copyIndexedAToIndexed;
        } else {
            copy = copyIndexedAToRGB;
        }
    }

    if (!copy) {
        return;
    }

    // For each tile...

    for (uint j = 0; j < layer.nrows; j++) {
        uint y = j * TILE_HEIGHT;

        for (uint i = 0; i < layer.ncols; i++) {
            uint x = i * TILE_WIDTH;

            // This seems the best place to apply the dissolve because it
            // depends on the global position of each tile's
            // pixels. Apparently it's the only mode which can apply to a
            // single layer.

            if (layer.mode == DISSOLVE_MODE) {
                if (!random_table_initialized) {
                    initializeRandomTable();
                    random_table_initialized = true;
                }
                if (layer.type == RGBA_GIMAGE) {
                    dissolveRGBPixels(layer.image_tiles[j][i], x, y);
                }

                else if (layer.type == GRAYA_GIMAGE) {
                    dissolveAlphaPixels(layer.alpha_tiles[j][i], x, y);
                }
            }

            // Shortcut for common case
            if (copy == copyRGBToRGB && layer.apply_mask != 1) {
                QPainter painter(&image);
                painter.setOpacity(layer.opacity / 255.0);
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.drawImage(x + layer.x_offset, y + layer.y_offset, layer.image_tiles[j][i]);
                continue;
            }

            for (int l = 0; l < layer.image_tiles[j][i].height(); l++) {
                for (int k = 0; k < layer.image_tiles[j][i].width(); k++) {

                    int m = x + k + layer.x_offset;
                    int n = y + l + layer.y_offset;

                    if (m < 0 || m >= image.width() || n < 0 || n >= image.height()) {
                        continue;
                    }

                    (*copy)(layer, i, j, k, l, image, m, n);
                }
            }
        }
    }
}

/*!
 * Copy an RGB pixel from the layer to the RGB image. Straight-forward.
 * The only thing this has to take account of is the opacity of the
 * layer. Evidently, the GIMP exporter itself does not actually do this.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::copyRGBToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                  QImage &image, int m, int n)
{
    QRgb src = layer.image_tiles[j][i].pixel(k, l);
    uchar src_a = layer.opacity;

    if (layer.type == RGBA_GIMAGE) {
        src_a = INT_MULT(src_a, qAlpha(src));
    }

    // Apply the mask (if any)

    if (layer.apply_mask == 1 && layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    image.setPixel(m, n, qRgba(src, src_a));
}

/*!
 * Copy a Gray pixel from the layer to the Gray image. Straight-forward.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::copyGrayToGray(const Layer &layer, uint i, uint j, int k, int l,
                                    QImage &image, int m, int n)
{
    int src = layer.image_tiles[j][i].pixelIndex(k, l);
    image.setPixel(m, n, src);
}

/*!
 * Copy a Gray pixel from the layer to an RGB image. Straight-forward.
 * The only thing this has to take account of is the opacity of the
 * layer. Evidently, the GIMP exporter itself does not actually do this.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::copyGrayToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                   QImage &image, int m, int n)
{
    QRgb src = layer.image_tiles[j][i].pixel(k, l);
    uchar src_a = layer.opacity;
    image.setPixel(m, n, qRgba(src, src_a));
}

/*!
 * Copy a GrayA pixel from the layer to an RGB image. Straight-forward.
 * The only thing this has to take account of is the opacity of the
 * layer. Evidently, the GIMP exporter itself does not actually do this.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::copyGrayAToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                    QImage &image, int m, int n)
{
    QRgb src = layer.image_tiles[j][i].pixel(k, l);
    uchar src_a = layer.alpha_tiles[j][i].pixelIndex(k, l);
    src_a = INT_MULT(src_a, layer.opacity);

    // Apply the mask (if any)

    if (layer.apply_mask == 1 && layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    image.setPixel(m, n, qRgba(src, src_a));
}

/*!
 * Copy an Indexed pixel from the layer to the Indexed image. Straight-forward.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::copyIndexedToIndexed(const Layer &layer, uint i, uint j, int k, int l,
        QImage &image, int m, int n)
{
    int src = layer.image_tiles[j][i].pixelIndex(k, l);
    image.setPixel(m, n, src);
}

/*!
 * Copy an IndexedA pixel from the layer to the Indexed image. Straight-forward.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::copyIndexedAToIndexed(const Layer &layer, uint i, uint j, int k, int l,
        QImage &image, int m, int n)
{
    uchar src = layer.image_tiles[j][i].pixelIndex(k, l);
    uchar src_a = layer.alpha_tiles[j][i].pixelIndex(k, l);
    src_a = INT_MULT(src_a, layer.opacity);

    if (layer.apply_mask == 1 &&
            layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    if (src_a > 127) {
        src++;
    } else {
        src = 0;
    }

    image.setPixel(m, n, src);
}

/*!
 * Copy an IndexedA pixel from the layer to an RGB image. Straight-forward.
 * The only thing this has to take account of is the opacity of the
 * layer. Evidently, the GIMP exporter itself does not actually do this.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::copyIndexedAToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                       QImage &image, int m, int n)
{
    QRgb src = layer.image_tiles[j][i].pixel(k, l);
    uchar src_a = layer.alpha_tiles[j][i].pixelIndex(k, l);
    src_a = INT_MULT(src_a, layer.opacity);

    // Apply the mask (if any)
    if (layer.apply_mask == 1 && layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    // This is what appears in the GIMP window
    if (src_a <= 127) {
        src_a = 0;
    } else {
        src_a = OPAQUE_OPACITY;
    }

    image.setPixel(m, n, qRgba(src, src_a));
}

/*!
 * Merge a layer into an image, taking account of the manifold modes.
 * \param xcf_image contains the layer and image to merge.
 */
void XCFImageFormat::mergeLayerIntoImage(XCFImage &xcf_image)
{
    Layer &layer(xcf_image.layer);
    QImage &image(xcf_image.image);

    PixelMergeOperation merge = nullptr;

    if (!layer.opacity) {
        return;    // don't bother doing anything
    }

    switch (layer.type) {
    case RGB_GIMAGE:
    case RGBA_GIMAGE:
        merge = mergeRGBToRGB;
        break;
    case GRAY_GIMAGE:
        if (layer.opacity == OPAQUE_OPACITY) {
            merge = mergeGrayToGray;
        } else {
            merge = mergeGrayToRGB;
        }
        break;
    case GRAYA_GIMAGE:
        if (xcf_image.image.depth() <= 8) {
            merge = mergeGrayAToGray;
        } else {
            merge = mergeGrayAToRGB;
        }
        break;
    case INDEXED_GIMAGE:
        merge = mergeIndexedToIndexed;
        break;
    case INDEXEDA_GIMAGE:
        if (xcf_image.image.depth() <= 8) {
            merge = mergeIndexedAToIndexed;
        } else {
            merge = mergeIndexedAToRGB;
        }
    }

    if (!merge) {
        return;
    }

    for (uint j = 0; j < layer.nrows; j++) {
        uint y = j * TILE_HEIGHT;

        for (uint i = 0; i < layer.ncols; i++) {
            uint x = i * TILE_WIDTH;

            // This seems the best place to apply the dissolve because it
            // depends on the global position of each tile's
            // pixels. Apparently it's the only mode which can apply to a
            // single layer.

            if (layer.mode == DISSOLVE_MODE) {
                if (!random_table_initialized) {
                    initializeRandomTable();
                    random_table_initialized = true;
                }
                if (layer.type == RGBA_GIMAGE) {
                    dissolveRGBPixels(layer.image_tiles[j][i], x, y);
                }

                else if (layer.type == GRAYA_GIMAGE) {
                    dissolveAlphaPixels(layer.alpha_tiles[j][i], x, y);
                }
            }

            // Shortcut for common case
            if (merge == mergeRGBToRGB && layer.apply_mask != 1
                    && layer.mode == NORMAL_MODE) {
                QPainter painter(&image);
                painter.setOpacity(layer.opacity / 255.0);
                painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
                painter.drawImage(x + layer.x_offset, y + layer.y_offset, layer.image_tiles[j][i]);
                continue;
            }

            for (int l = 0; l < layer.image_tiles[j][i].height(); l++) {
                for (int k = 0; k < layer.image_tiles[j][i].width(); k++) {

                    int m = x + k + layer.x_offset;
                    int n = y + l + layer.y_offset;

                    if (m < 0 || m >= image.width() || n < 0 || n >= image.height()) {
                        continue;
                    }

                    (*merge)(layer, i, j, k, l, image, m, n);
                }
            }
        }
    }
}

/*!
 * Merge an RGB pixel from the layer to the RGB image. Straight-forward.
 * The only thing this has to take account of is the opacity of the
 * layer. Evidently, the GIMP exporter itself does not actually do this.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::mergeRGBToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                   QImage &image, int m, int n)
{
    QRgb src = layer.image_tiles[j][i].pixel(k, l);
    QRgb dst = image.pixel(m, n);

    uchar src_r = qRed(src);
    uchar src_g = qGreen(src);
    uchar src_b = qBlue(src);
    uchar src_a = qAlpha(src);

    uchar dst_r = qRed(dst);
    uchar dst_g = qGreen(dst);
    uchar dst_b = qBlue(dst);
    uchar dst_a = qAlpha(dst);

    if (!src_a) {
        return;    // nothing to merge
    }

    switch (layer.mode) {
    case MULTIPLY_MODE: {
        src_r = INT_MULT(src_r, dst_r);
        src_g = INT_MULT(src_g, dst_g);
        src_b = INT_MULT(src_b, dst_b);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case DIVIDE_MODE: {
        src_r = qMin((dst_r * 256) / (1 + src_r), 255);
        src_g = qMin((dst_g * 256) / (1 + src_g), 255);
        src_b = qMin((dst_b * 256) / (1 + src_b), 255);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case SCREEN_MODE: {
        src_r = 255 - INT_MULT(255 - dst_r, 255 - src_r);
        src_g = 255 - INT_MULT(255 - dst_g, 255 - src_g);
        src_b = 255 - INT_MULT(255 - dst_b, 255 - src_b);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case OVERLAY_MODE: {
        src_r = INT_MULT(dst_r, dst_r + INT_MULT(2 * src_r, 255 - dst_r));
        src_g = INT_MULT(dst_g, dst_g + INT_MULT(2 * src_g, 255 - dst_g));
        src_b = INT_MULT(dst_b, dst_b + INT_MULT(2 * src_b, 255 - dst_b));
        src_a = qMin(src_a, dst_a);
    }
    break;
    case DIFFERENCE_MODE: {
        src_r = dst_r > src_r ? dst_r - src_r : src_r - dst_r;
        src_g = dst_g > src_g ? dst_g - src_g : src_g - dst_g;
        src_b = dst_b > src_b ? dst_b - src_b : src_b - dst_b;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case ADDITION_MODE: {
        src_r = add_lut(dst_r, src_r);
        src_g = add_lut(dst_g, src_g);
        src_b = add_lut(dst_b, src_b);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case SUBTRACT_MODE: {
        src_r = dst_r > src_r ? dst_r - src_r : 0;
        src_g = dst_g > src_g ? dst_g - src_g : 0;
        src_b = dst_b > src_b ? dst_b - src_b : 0;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case DARKEN_ONLY_MODE: {
        src_r = dst_r < src_r ? dst_r : src_r;
        src_g = dst_g < src_g ? dst_g : src_g;
        src_b = dst_b < src_b ? dst_b : src_b;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case LIGHTEN_ONLY_MODE: {
        src_r = dst_r < src_r ? src_r : dst_r;
        src_g = dst_g < src_g ? src_g : dst_g;
        src_b = dst_b < src_b ? src_b : dst_b;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case HUE_MODE: {
        uchar new_r = dst_r;
        uchar new_g = dst_g;
        uchar new_b = dst_b;

        RGBTOHSV(src_r, src_g, src_b);
        RGBTOHSV(new_r, new_g, new_b);

        new_r = src_r;

        HSVTORGB(new_r, new_g, new_b);

        src_r = new_r;
        src_g = new_g;
        src_b = new_b;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case SATURATION_MODE: {
        uchar new_r = dst_r;
        uchar new_g = dst_g;
        uchar new_b = dst_b;

        RGBTOHSV(src_r, src_g, src_b);
        RGBTOHSV(new_r, new_g, new_b);

        new_g = src_g;

        HSVTORGB(new_r, new_g, new_b);

        src_r = new_r;
        src_g = new_g;
        src_b = new_b;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case VALUE_MODE: {
        uchar new_r = dst_r;
        uchar new_g = dst_g;
        uchar new_b = dst_b;

        RGBTOHSV(src_r, src_g, src_b);
        RGBTOHSV(new_r, new_g, new_b);

        new_b = src_b;

        HSVTORGB(new_r, new_g, new_b);

        src_r = new_r;
        src_g = new_g;
        src_b = new_b;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case COLOR_MODE: {
        uchar new_r = dst_r;
        uchar new_g = dst_g;
        uchar new_b = dst_b;

        RGBTOHLS(src_r, src_g, src_b);
        RGBTOHLS(new_r, new_g, new_b);

        new_r = src_r;
        new_b = src_b;

        HLSTORGB(new_r, new_g, new_b);

        src_r = new_r;
        src_g = new_g;
        src_b = new_b;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case DODGE_MODE: {
        uint tmp;

        tmp = dst_r << 8;
        tmp /= 256 - src_r;
        src_r = (uchar) qMin(tmp, 255u);

        tmp = dst_g << 8;
        tmp /= 256 - src_g;
        src_g = (uchar) qMin(tmp, 255u);

        tmp = dst_b << 8;
        tmp /= 256 - src_b;
        src_b = (uchar) qMin(tmp, 255u);

        src_a = qMin(src_a, dst_a);
    }
    break;
    case BURN_MODE: {
        uint tmp;

        tmp = (255 - dst_r) << 8;
        tmp /= src_r + 1;
        src_r = (uchar) qMin(tmp, 255u);
        src_r = 255 - src_r;

        tmp = (255 - dst_g) << 8;
        tmp /= src_g + 1;
        src_g = (uchar) qMin(tmp, 255u);
        src_g = 255 - src_g;

        tmp = (255 - dst_b) << 8;
        tmp /= src_b + 1;
        src_b = (uchar) qMin(tmp, 255u);
        src_b = 255 - src_b;

        src_a = qMin(src_a, dst_a);
    }
    break;
    case HARDLIGHT_MODE: {
        uint tmp;
        if (src_r > 128) {
            tmp = ((int)255 - dst_r) * ((int) 255 - ((src_r - 128) << 1));
            src_r = (uchar) qMin(255 - (tmp >> 8), 255u);
        } else {
            tmp = (int) dst_r * ((int) src_r << 1);
            src_r = (uchar) qMin(tmp >> 8, 255u);
        }

        if (src_g > 128) {
            tmp = ((int)255 - dst_g) * ((int) 255 - ((src_g - 128) << 1));
            src_g = (uchar) qMin(255 - (tmp >> 8), 255u);
        } else {
            tmp = (int) dst_g * ((int) src_g << 1);
            src_g = (uchar) qMin(tmp >> 8, 255u);
        }

        if (src_b > 128) {
            tmp = ((int)255 - dst_b) * ((int) 255 - ((src_b - 128) << 1));
            src_b = (uchar) qMin(255 - (tmp >> 8), 255u);
        } else {
            tmp = (int) dst_b * ((int) src_b << 1);
            src_b = (uchar) qMin(tmp >> 8, 255u);
        }
        src_a = qMin(src_a, dst_a);
    }
    break;
    case SOFTLIGHT_MODE: {
        uint tmpS, tmpM;

        tmpM = INT_MULT(dst_r, src_r);
        tmpS = 255 - INT_MULT((255 - dst_r), (255 - src_r));
        src_r = INT_MULT((255 - dst_r), tmpM)
                + INT_MULT(dst_r, tmpS);

        tmpM = INT_MULT(dst_g, src_g);
        tmpS = 255 - INT_MULT((255 - dst_g), (255 - src_g));
        src_g = INT_MULT((255 - dst_g), tmpM)
                + INT_MULT(dst_g, tmpS);

        tmpM = INT_MULT(dst_b, src_b);
        tmpS = 255 - INT_MULT((255 - dst_b), (255 - src_b));
        src_b = INT_MULT((255 - dst_b), tmpM)
                + INT_MULT(dst_b, tmpS);

        src_a = qMin(src_a, dst_a);
    }
    break;
    case GRAIN_EXTRACT_MODE: {
        int tmp;

        tmp = dst_r - src_r + 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);
        src_r = (uchar) tmp;

        tmp = dst_g - src_g + 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);
        src_g = (uchar) tmp;

        tmp = dst_b - src_b + 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);
        src_b = (uchar) tmp;

        src_a = qMin(src_a, dst_a);
    }
    break;
    case GRAIN_MERGE_MODE: {
        int tmp;

        tmp = dst_r + src_r - 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);
        src_r = (uchar) tmp;

        tmp = dst_g + src_g - 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);
        src_g = (uchar) tmp;

        tmp = dst_b + src_b - 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);
        src_b = (uchar) tmp;

        src_a = qMin(src_a, dst_a);
    }
    break;
    }

    src_a = INT_MULT(src_a, layer.opacity);

    // Apply the mask (if any)

    if (layer.apply_mask == 1 && layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    uchar new_r, new_g, new_b, new_a;
    new_a = dst_a + INT_MULT(OPAQUE_OPACITY - dst_a, src_a);

    const float src_ratio = new_a == 0 ? 1.0 : (float)src_a / new_a;
    float dst_ratio = 1.0 - src_ratio;

    new_r = (uchar)(src_ratio * src_r + dst_ratio * dst_r + EPSILON);
    new_g = (uchar)(src_ratio * src_g + dst_ratio * dst_g + EPSILON);
    new_b = (uchar)(src_ratio * src_b + dst_ratio * dst_b + EPSILON);

    if (!layer_modes[layer.mode].affect_alpha) {
        new_a = dst_a;
    }

    image.setPixel(m, n, qRgba(new_r, new_g, new_b, new_a));
}

/*!
 * Merge a Gray pixel from the layer to the Gray image. Straight-forward.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::mergeGrayToGray(const Layer &layer, uint i, uint j, int k, int l,
                                     QImage &image, int m, int n)
{
    int src = layer.image_tiles[j][i].pixelIndex(k, l);
    image.setPixel(m, n, src);
}

/*!
 * Merge a GrayA pixel from the layer to the Gray image. Straight-forward.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::mergeGrayAToGray(const Layer &layer, uint i, uint j, int k, int l,
                                      QImage &image, int m, int n)
{
    int src = qGray(layer.image_tiles[j][i].pixel(k, l));
    int dst = image.pixelIndex(m, n);

    uchar src_a = layer.alpha_tiles[j][i].pixelIndex(k, l);

    if (!src_a) {
        return;    // nothing to merge
    }

    switch (layer.mode) {
    case MULTIPLY_MODE: {
        src = INT_MULT(src, dst);
    }
    break;
    case DIVIDE_MODE: {
        src = qMin((dst * 256) / (1 + src), 255);
    }
    break;
    case SCREEN_MODE: {
        src = 255 - INT_MULT(255 - dst, 255 - src);
    }
    break;
    case OVERLAY_MODE: {
        src = INT_MULT(dst, dst + INT_MULT(2 * src, 255 - dst));
    }
    break;
    case DIFFERENCE_MODE: {
        src = dst > src ? dst - src : src - dst;
    }
    break;
    case ADDITION_MODE: {
        src = add_lut(dst, src);
    }
    break;
    case SUBTRACT_MODE: {
        src = dst > src ? dst - src : 0;
    }
    break;
    case DARKEN_ONLY_MODE: {
        src = dst < src ? dst : src;
    }
    break;
    case LIGHTEN_ONLY_MODE: {
        src = dst < src ? src : dst;
    }
    break;
    case DODGE_MODE: {
        uint tmp = dst << 8;
        tmp /= 256 - src;
        src = (uchar) qMin(tmp, 255u);
    }
    break;
    case BURN_MODE: {
        uint tmp = (255 - dst) << 8;
        tmp /= src + 1;
        src = (uchar) qMin(tmp, 255u);
        src = 255 - src;
    }
    break;
    case HARDLIGHT_MODE: {
        uint tmp;
        if (src > 128) {
            tmp = ((int)255 - dst) * ((int) 255 - ((src - 128) << 1));
            src = (uchar) qMin(255 - (tmp >> 8), 255u);
        } else {
            tmp = (int) dst * ((int) src << 1);
            src = (uchar) qMin(tmp >> 8, 255u);
        }
    }
    break;
    case SOFTLIGHT_MODE: {
        uint tmpS, tmpM;

        tmpM = INT_MULT(dst, src);
        tmpS = 255 - INT_MULT((255 - dst), (255 - src));
        src = INT_MULT((255 - dst), tmpM)
              + INT_MULT(dst, tmpS);

    }
    break;
    case GRAIN_EXTRACT_MODE: {
        int tmp;

        tmp = dst - src + 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);

        src = (uchar) tmp;
    }
    break;
    case GRAIN_MERGE_MODE: {
        int tmp;

        tmp = dst + src - 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);

        src = (uchar) tmp;
    }
    break;
    }

    src_a = INT_MULT(src_a, layer.opacity);

    // Apply the mask (if any)

    if (layer.apply_mask == 1 && layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    uchar new_a = OPAQUE_OPACITY;

    const float src_ratio = new_a == 0 ? 1.0 : (float)src_a / new_a;
    float dst_ratio = 1.0 - src_ratio;

    uchar new_g = (uchar)(src_ratio * src + dst_ratio * dst + EPSILON);

    image.setPixel(m, n, new_g);
}

/*!
 * Merge a Gray pixel from the layer to an RGB image. Straight-forward.
 * The only thing this has to take account of is the opacity of the
 * layer. Evidently, the GIMP exporter itself does not actually do this.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::mergeGrayToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                    QImage &image, int m, int n)
{
    QRgb src = layer.image_tiles[j][i].pixel(k, l);
    uchar src_a = layer.opacity;
    image.setPixel(m, n, qRgba(src, src_a));
}

/*!
 * Merge a GrayA pixel from the layer to an RGB image. Straight-forward.
 * The only thing this has to take account of is the opacity of the
 * layer. Evidently, the GIMP exporter itself does not actually do this.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::mergeGrayAToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                     QImage &image, int m, int n)
{
    int src = qGray(layer.image_tiles[j][i].pixel(k, l));
    int dst = qGray(image.pixel(m, n));

    uchar src_a = layer.alpha_tiles[j][i].pixelIndex(k, l);
    uchar dst_a = qAlpha(image.pixel(m, n));

    if (!src_a) {
        return;    // nothing to merge
    }

    switch (layer.mode) {
    case MULTIPLY_MODE: {
        src = INT_MULT(src, dst);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case DIVIDE_MODE: {
        src = qMin((dst * 256) / (1 + src), 255);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case SCREEN_MODE: {
        src = 255 - INT_MULT(255 - dst, 255 - src);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case OVERLAY_MODE: {
        src = INT_MULT(dst, dst + INT_MULT(2 * src, 255 - dst));
        src_a = qMin(src_a, dst_a);
    }
    break;
    case DIFFERENCE_MODE: {
        src = dst > src ? dst - src : src - dst;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case ADDITION_MODE: {
        src = add_lut(dst, src);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case SUBTRACT_MODE: {
        src = dst > src ? dst - src : 0;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case DARKEN_ONLY_MODE: {
        src = dst < src ? dst : src;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case LIGHTEN_ONLY_MODE: {
        src = dst < src ? src : dst;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case DODGE_MODE: {
        uint tmp = dst << 8;
        tmp /= 256 - src;
        src = (uchar) qMin(tmp, 255u);
        src_a = qMin(src_a, dst_a);
    }
    break;
    case BURN_MODE: {
        uint tmp = (255 - dst) << 8;
        tmp /= src + 1;
        src = (uchar) qMin(tmp, 255u);
        src = 255 - src;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case HARDLIGHT_MODE: {
        uint tmp;
        if (src > 128) {
            tmp = ((int)255 - dst) * ((int) 255 - ((src - 128) << 1));
            src = (uchar) qMin(255 - (tmp >> 8), 255u);
        } else {
            tmp = (int) dst * ((int) src << 1);
            src = (uchar) qMin(tmp >> 8, 255u);
        }
        src_a = qMin(src_a, dst_a);
    }
    break;
    case SOFTLIGHT_MODE: {
        uint tmpS, tmpM;

        tmpM = INT_MULT(dst, src);
        tmpS = 255 - INT_MULT((255 - dst), (255 - src));
        src = INT_MULT((255 - dst), tmpM)
              + INT_MULT(dst, tmpS);

        src_a = qMin(src_a, dst_a);
    }
    break;
    case GRAIN_EXTRACT_MODE: {
        int tmp;

        tmp = dst - src + 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);

        src = (uchar) tmp;
        src_a = qMin(src_a, dst_a);
    }
    break;
    case GRAIN_MERGE_MODE: {
        int tmp;

        tmp = dst + src - 128;
        tmp = qMin(tmp, 255);
        tmp = qMax(tmp, 0);

        src = (uchar) tmp;
        src_a = qMin(src_a, dst_a);
    }
    break;
    }

    src_a = INT_MULT(src_a, layer.opacity);

    // Apply the mask (if any)
    if (layer.apply_mask == 1 && layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    uchar new_a = dst_a + INT_MULT(OPAQUE_OPACITY - dst_a, src_a);

    const float src_ratio = new_a == 0 ? 1.0 : (float)src_a / new_a;
    float dst_ratio = 1.0 - src_ratio;

    uchar new_g = (uchar)(src_ratio * src + dst_ratio * dst + EPSILON);

    if (!layer_modes[layer.mode].affect_alpha) {
        new_a = dst_a;
    }

    image.setPixel(m, n, qRgba(new_g, new_g, new_g, new_a));
}

/*!
 * Merge an Indexed pixel from the layer to the Indexed image. Straight-forward.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::mergeIndexedToIndexed(const Layer &layer, uint i, uint j, int k, int l,
        QImage &image, int m, int n)
{
    int src = layer.image_tiles[j][i].pixelIndex(k, l);
    image.setPixel(m, n, src);
}

/*!
 * Merge an IndexedA pixel from the layer to the Indexed image. Straight-forward.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::mergeIndexedAToIndexed(const Layer &layer, uint i, uint j, int k, int l,
        QImage &image, int m, int n)
{
    uchar src = layer.image_tiles[j][i].pixelIndex(k, l);
    uchar src_a = layer.alpha_tiles[j][i].pixelIndex(k, l);
    src_a = INT_MULT(src_a, layer.opacity);

    if (layer.apply_mask == 1 &&
            layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    if (src_a > 127) {
        src++;
        image.setPixel(m, n, src);
    }
}

/*!
 * Merge an IndexedA pixel from the layer to an RGB image. Straight-forward.
 * The only thing this has to take account of is the opacity of the
 * layer. Evidently, the GIMP exporter itself does not actually do this.
 * \param layer source layer.
 * \param i x tile index.
 * \param j y tile index.
 * \param k x pixel index of tile i,j.
 * \param l y pixel index of tile i,j.
 * \param image destination image.
 * \param m x pixel of destination image.
 * \param n y pixel of destination image.
 */
void XCFImageFormat::mergeIndexedAToRGB(const Layer &layer, uint i, uint j, int k, int l,
                                        QImage &image, int m, int n)
{
    QRgb src = layer.image_tiles[j][i].pixel(k, l);
    uchar src_a = layer.alpha_tiles[j][i].pixelIndex(k, l);
    src_a = INT_MULT(src_a, layer.opacity);

    // Apply the mask (if any)
    if (layer.apply_mask == 1 && layer.mask_tiles.size() > (int)j &&
            layer.mask_tiles[j].size() > (int)i) {
        src_a = INT_MULT(src_a, layer.mask_tiles[j][i].pixelIndex(k, l));
    }

    // This is what appears in the GIMP window
    if (src_a <= 127) {
        src_a = 0;
    } else {
        src_a = OPAQUE_OPACITY;
    }

    image.setPixel(m, n, qRgba(src, src_a));
}

/*!
 * Dissolving pixels: pick a random number between 0 and 255. If the pixel's
 * alpha is less than that, make it transparent.
 * \param image the image tile to dissolve.
 * \param x the global x position of the tile.
 * \param y the global y position of the tile.
 */
void XCFImageFormat::dissolveRGBPixels(QImage &image, int x, int y)
{
    // The apparently spurious rand() calls are to wind the random
    // numbers up to the same point for each tile.

    for (int l = 0; l < image.height(); l++) {
        srand(random_table[(l + y) % RANDOM_TABLE_SIZE]);

        for (int k = 0; k < x; k++) {
            rand();
        }

        for (int k = 0; k < image.width(); k++) {
            int rand_val = rand() & 0xff;
            QRgb pixel = image.pixel(k, l);

            if (rand_val > qAlpha(pixel)) {
                image.setPixel(k, l, qRgba(pixel, 0));
            }
        }
    }
}

/*!
 * Dissolving pixels: pick a random number between 0 and 255. If the pixel's
 * alpha is less than that, make it transparent. This routine works for
 * the GRAYA and INDEXEDA image types where the pixel alpha's are stored
 * separately from the pixel themselves.
 * \param image the alpha tile to dissolve.
 * \param x the global x position of the tile.
 * \param y the global y position of the tile.
 */
void XCFImageFormat::dissolveAlphaPixels(QImage &image, int x, int y)
{
    // The apparently spurious rand() calls are to wind the random
    // numbers up to the same point for each tile.

    for (int l = 0; l < image.height(); l++) {
        srand(random_table[(l + y) % RANDOM_TABLE_SIZE]);

        for (int k = 0; k < x; k++) {
            rand();
        }

        for (int k = 0; k < image.width(); k++) {
            int rand_val = rand() & 0xff;
            uchar alpha = image.pixelIndex(k, l);

            if (rand_val > alpha) {
                image.setPixel(k, l, 0);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

XCFHandler::XCFHandler()
{
}

bool XCFHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("xcf");
        return true;
    }
    return false;
}

bool XCFHandler::read(QImage *image)
{
    XCFImageFormat xcfif;
    return xcfif.readXCF(device(), image);
}

bool XCFHandler::write(const QImage &)
{
    return false;
}

bool XCFHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("DDSHandler::canRead() called with no device");
        return false;
    }

    qint64 oldPos = device->pos();

    char head[8];
    qint64 readBytes = device->read(head, sizeof(head));
    if (readBytes != sizeof(head)) {
        if (device->isSequential()) {
            while (readBytes > 0) {
                device->ungetChar(head[readBytes-- - 1]);
            }
        } else {
            device->seek(oldPos);
        }
        return false;
    }

    if (device->isSequential()) {
        while (readBytes > 0) {
            device->ungetChar(head[readBytes-- - 1]);
        }
    } else {
        device->seek(oldPos);
    }

    return qstrncmp(head, "gimp xcf", 8) == 0;
}

QImageIOPlugin::Capabilities XCFPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "xcf") {
        return Capabilities(CanRead);
    }
    if (!format.isEmpty()) {
        return {};
    }
    if (!device->isOpen()) {
        return {};
    }

    Capabilities cap;
    if (device->isReadable() && XCFHandler::canRead(device)) {
        cap |= CanRead;
    }
    return cap;
}

QImageIOHandler *XCFPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new XCFHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}
