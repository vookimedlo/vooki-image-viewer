/*
    This file is part of the KDE project
    SPDX-FileCopyrightText: 2025 Mirco Miranda <mircomir@outlook.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MICROEXIF_P_H
#define MICROEXIF_P_H

#include <QByteArray>
#include <QColorSpace>
#include <QDataStream>
#include <QDateTime>
#include <QImage>
#include <QImageIOHandler>
#include <QMap>
#include <QUuid>
#include <QVariant>

#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
#define EXIF_DEFAULT_BYTEORDER QDataStream::LittleEndian
#else
#define EXIF_DEFAULT_BYTEORDER QDataStream::BigEndian
#endif

/*!
 * \brief The MicroExif class
 * Class to extract / write minimal EXIF data (e.g. resolution, rotation,
 * some strings).
 *
 * This class is a partial (or rather minimal) implementation and is only used
 * to avoid including external libraries when only a few tags are needed.
 *
 * It reads/writes the main IFD only.
 */
class MicroExif
{
public:
    using Tags = QMap<quint16, QVariant>;

    /*!
     * \brief MicroExif
     * Constructs an empty class.
     * \sa isEmpty
     */
    MicroExif();

    MicroExif(const MicroExif &other) = default;
    MicroExif &operator=(const MicroExif &other) = default;

    /*!
     * \brief clear
     * Removes all items.
     */
    void clear();

    /*!
     * \brief isEmpty
     * \return True if it contains no items, otherwise false.
     */
    bool isEmpty() const;

    /*!
     * \brief horizontalResolution
     * \return The horizontal resolution in DPI.
     */
    double horizontalResolution() const;
    void setHorizontalResolution(double hres);

    /*!
     * \brief verticalResolution
     * \return The vertical resolution in DPI.
     */
    double verticalResolution() const;
    void setVerticalResolution(double vres);

    /*!
     * \brief colosSpace
     * \return sRGB color space or an invalid one.
     */
    QColorSpace colosSpace() const;
    void setColorSpace(const QColorSpace& cs);
    void setColorSpace(const QColorSpace::NamedColorSpace& csName);

    /*!
     * \brief width
     * \return The image width.
     */
    qint32 width() const;
    void setWidth(qint32 w);

    /*!
     * \brief height
     * \return The image height.
     */
    qint32 height() const;
    void setHeight(qint32 h);

    /*!
     * \brief orientation
     * The orientation of the image with respect to the rows and columns.
     *
     * Valid orientation values:
     * - 1 = The 0th row is at the visual top of the image, and the 0th column is the visual left-hand side.
     * - 2 = The 0th row is at the visual top of the image, and the 0th column is the visual right-hand side.
     * - 3 = The 0th row is at the visual bottom of the image, and the 0th column is the visual right-hand side.
     * - 4 = The 0th row is at the visual bottom of the image, and the 0th column is the visual left-hand side.
     * - 5 = The 0th row is the visual left-hand side of the image, and the 0th column is the visual top.
     * - 6 = The 0th row is the visual right-hand side of the image, and the 0th column is the visual top.
     * - 7 = The 0th row is the visual right-hand side of the image, and the 0th column is the visual bottom.
     * - 8 = The 0th row is the visual left-hand side of the image, and the 0th column is the visual bottom.
     * \return The orientation value or 0 if none.
     * \sa transformation
     */
    quint16 orientation() const;
    void setOrientation(quint16 orient);

    /*!
     * \brief transformation
     * \return The orientation converted in the equvalent Qt transformation.
     * \sa orientation
     */
    QImageIOHandler::Transformation transformation() const;
    void setTransformation(const QImageIOHandler::Transformation& t);

    /*!
     * \brief software
     * \return Name and version number of the software package(s) used to create the image.
     */
    QString software() const;
    void setSoftware(const QString& s);

    /*!
     * \brief description
     * \return A string that describes the subject of the image.
     */
    QString description() const;
    void setDescription(const QString& s);

    /*!
     * \brief artist
     * \return Person who created the image.
     */
    QString artist() const;
    void setArtist(const QString& s);

    /*!
     * \brief copyright
     * \return Copyright notice of the person or organization that claims the copyright to the image.
     */
    QString copyright() const;
    void setCopyright(const QString& s);

    /*!
     * \brief make
     * \return The manufacturer of the recording equipment.
     */
    QString make() const;
    void setMake(const QString& s);

    /*!
     * \brief model
     * \return The model name or model number of the equipment.
     */
    QString model() const;
    void setModel(const QString& s);

    /*!
     * \brief serialNumber
     * \return The serial number of the recording equipment.
     */
    QString serialNumber() const;
    void setSerialNumber(const QString &s);

    /*!
     * \brief lensMake
     * \return The manufacturer of the interchangeable lens that was used.
     */
    QString lensMake() const;
    void setLensMake(const QString &s);

    /*!
     * \brief lensModel
     * \return The model name or model number of the lens that was used.
     */
    QString lensModel() const;
    void setLensModel(const QString &s);

    /*!
     * \brief lensSerialNumber
     * \return The serial number of the interchangeable lens that was used.
     */
    QString lensSerialNumber() const;
    void setLensSerialNumber(const QString &s);

    /*!
     * \brief dateTime
     * \return Creation date and time.
     */
    QDateTime dateTime() const;
    void setDateTime(const QDateTime& dt);

    /*!
     * \brief title
     * \return The title of the image.
     */
    QString title() const;
    void setImageTitle(const QString &s);

    /*!
     * \brief uniqueId
     * \return An identifier assigned uniquely to each image or null one if none.
     */
    QUuid uniqueId() const;
    void setUniqueId(const QUuid &uuid);

    /*!
     * \brief latitude
     * \return Floating-point number indicating the latitude in degrees north of the equator (e.g. 27.717) or NaN if not set.
     */
    double latitude() const;
    void setLatitude(double degree);

    /*!
     * \brief longitude
     * \return Floating-point number indicating the longitude in degrees east of Greenwich (e.g. 85.317) or NaN if not set.
     */
    double longitude() const;
    void setLongitude(double degree);

    /*!
     * \brief altitude
     * \return Floating-point number indicating the GPS altitude in meters above sea level or ellipsoidal surface (e.g. 35.4) or NaN if not set.
     * \note It makes no distinction between an 'ellipsoidal surface' and 'sea level'.
     */
    double altitude() const;
    void setAltitude(double meters);

    /*!
     * \brief toByteArray
     * \param byteOrder Sets the serialization byte order for EXIF data.
     * \return A byte array containing the serialized data.
     */
    QByteArray toByteArray(const QDataStream::ByteOrder &byteOrder = EXIF_DEFAULT_BYTEORDER) const;

    /*!
     * \brief write
     * Serialize the class on a device.
     * \param device A random access device.
     * \param byteOrder Sets the serialization byte order for EXIF data.
     * \return True on success, otherwise false.
     */
    bool write(QIODevice *device, const QDataStream::ByteOrder &byteOrder = EXIF_DEFAULT_BYTEORDER) const;

    /*!
     * \brief toImageMetadata
     * Helper to set metadata in an image.
     * \param targetImage The image to set metadata on.
     * \param replaceExisting Replaces any existing metadata.
     */
    void toImageMetadata(QImage& targetImage, bool replaceExisting = false) const;

    /*!
     * \brief fromByteArray
     * Creates the class from RAW EXIF data.
     * \return The created class (empty on error).
     * \sa isEmpty
     */
    static MicroExif fromByteArray(const QByteArray &ba);

    /*!
     * \brief fromDevice
     * Creates the class from a device.
     * \param device A random access device.
     * \return The created class (empty on error).
     * \sa isEmpty
     */
    static MicroExif fromDevice(QIODevice *device);

    /*!
     * \brief fromImage
     * Creates the class and fill it with image info (e.g. resolution).
     */
    static MicroExif fromImage(const QImage &image);

private:
    void setTiffString(quint16 tagId, const QString &s);
    QString tiffString(quint16 tagId) const;
    void setExifString(quint16 tagId, const QString& s);
    QString exifString(quint16 tagId) const;
    void setGpsString(quint16 tagId, const QString& s);
    QString gpsString(quint16 tagId) const;
    bool writeHeader(QDataStream &ds) const;
    bool writeIfds(QDataStream &ds) const;
    void updateTags(Tags &tiffTags, Tags &exifTags, Tags &gpsTags) const;

    static void setString(Tags &tags, quint16 tagId, const QString &s);
    static QString string(const Tags &tags, quint16 tagId);

private:
    Tags m_tiffTags;
    Tags m_exifTags;
    Tags m_gpsTags;
};

#endif // MICROEXIF_P_H
