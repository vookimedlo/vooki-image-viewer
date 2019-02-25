
/*
* KImageIO Routines to read (and perhaps in the future, write) images
* in the high dynamic range EXR format.
* Copyright (c) 2003, Brad Hards <bradh@frogmouth.net>
*
* This library is distributed under the conditions of the GNU LGPL.
*/

#include "exr_p.h"

#include <ImfRgbaFile.h>
#include <ImfStandardAttributes.h>
#include <ImathBox.h>
#include <ImfInputFile.h>
#include <ImfBoxAttribute.h>
#include <ImfChannelListAttribute.h>
#include <ImfCompressionAttribute.h>
#include <ImfFloatAttribute.h>
#include <ImfIntAttribute.h>
#include <ImfLineOrderAttribute.h>
#include <ImfStringAttribute.h>
#include <ImfVecAttribute.h>
#include <ImfArray.h>
#include <ImfConvert.h>
#include <ImfVersion.h>
#include <IexThrowErrnoExc.h>

#include <iostream>

#include <QImage>
#include <QDataStream>
#include <QDebug>
#include <QImageIOPlugin>

class K_IStream: public Imf::IStream
{
public:
    K_IStream(QIODevice *dev, const QByteArray &fileName):
        IStream(fileName.data()), m_dev(dev)
    {
    }

    bool  read(char c[], int n) override;
    Imf::Int64 tellg() override;
    void seekg(Imf::Int64 pos) override;
    void clear() override;

private:
    QIODevice *m_dev;
};

bool K_IStream::read(char c[], int n)
{
    qint64 result = m_dev->read(c, n);
    if (result > 0) {
        return true;
    } else if (result == 0) {
        throw Iex::InputExc("Unexpected end of file");
    } else { // negative value {
        Iex::throwErrnoExc("Error in read", result);
    }
    return false;
}

Imf::Int64 K_IStream::tellg()
{
    return m_dev->pos();
}

void K_IStream::seekg(Imf::Int64 pos)
{
    m_dev->seek(pos);
}

void K_IStream::clear()
{
    // TODO
}

/* this does a conversion from the ILM Half (equal to Nvidia Half)
 * format into the normal 32 bit pixel format. Process is from the
 * ILM code.
 */
QRgb RgbaToQrgba(struct Imf::Rgba imagePixel)
{
    float r, g, b, a;

    //  1) Compensate for fogging by subtracting defog
    //     from the raw pixel values.
    // Response: We work with defog of 0.0, so this is a no-op

    //  2) Multiply the defogged pixel values by
    //     2^(exposure + 2.47393).
    // Response: We work with exposure of 0.0.
    // (2^2.47393) is 5.55555
    r = imagePixel.r * 5.55555;
    g = imagePixel.g * 5.55555;
    b = imagePixel.b * 5.55555;
    a = imagePixel.a * 5.55555;

    //  3) Values, which are now 1.0, are called "middle gray".
    //     If defog and exposure are both set to 0.0, then
    //     middle gray corresponds to a raw pixel value of 0.18.
    //     In step 6, middle gray values will be mapped to an
    //     intensity 3.5 f-stops below the display's maximum
    //     intensity.
    // Response: no apparent content.

    //  4) Apply a knee function.  The knee function has two
    //     parameters, kneeLow and kneeHigh.  Pixel values
    //     below 2^kneeLow are not changed by the knee
    //     function.  Pixel values above kneeLow are lowered
    //     according to a logarithmic curve, such that the
    //     value 2^kneeHigh is mapped to 2^3.5 (in step 6,
    //     this value will be mapped to the display's
    //     maximum intensity).
    // Response: kneeLow = 0.0 (2^0.0 => 1); kneeHigh = 5.0 (2^5 =>32)
    if (r > 1.0) {
        r = 1.0 + Imath::Math<float>::log((r - 1.0) * 0.184874 + 1) / 0.184874;
    }
    if (g > 1.0) {
        g = 1.0 + Imath::Math<float>::log((g - 1.0) * 0.184874 + 1) / 0.184874;
    }
    if (b > 1.0) {
        b = 1.0 + Imath::Math<float>::log((b - 1.0) * 0.184874 + 1) / 0.184874;
    }
    if (a > 1.0) {
        a = 1.0 + Imath::Math<float>::log((a - 1.0) * 0.184874 + 1) / 0.184874;
    }
//
//  5) Gamma-correct the pixel values, assuming that the
//     screen's gamma is 0.4545 (or 1/2.2).
    r = Imath::Math<float>::pow(r, 0.4545);
    g = Imath::Math<float>::pow(g, 0.4545);
    b = Imath::Math<float>::pow(b, 0.4545);
    a = Imath::Math<float>::pow(a, 0.4545);

//  6) Scale the values such that pixels middle gray
//     pixels are mapped to 84.66 (or 3.5 f-stops below
//     the display's maximum intensity).
//
//  7) Clamp the values to [0, 255].
    return qRgba((unsigned char)(Imath::clamp(r * 84.66f, 0.f, 255.f)),
                 (unsigned char)(Imath::clamp(g * 84.66f, 0.f, 255.f)),
                 (unsigned char)(Imath::clamp(b * 84.66f, 0.f, 255.f)),
                 (unsigned char)(Imath::clamp(a * 84.66f, 0.f, 255.f)));
}

EXRHandler::EXRHandler()
{
}

bool EXRHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("exr");
        return true;
    }
    return false;
}

bool EXRHandler::read(QImage *outImage)
{
    try {
        int width, height;

        K_IStream istr(device(), QByteArray());
        Imf::RgbaInputFile file(istr);
        Imath::Box2i dw = file.dataWindow();

        width  = dw.max.x - dw.min.x + 1;
        height = dw.max.y - dw.min.y + 1;

        Imf::Array2D<Imf::Rgba> pixels;
        pixels.resizeErase(height, width);

        file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
        file.readPixels(dw.min.y, dw.max.y);

        QImage image(width, height, QImage::Format_RGB32);
        if (image.isNull()) {
            return false;
        }

        // somehow copy pixels into image
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                // copy pixels(x,y) into image(x,y)
                image.setPixel(x, y, RgbaToQrgba(pixels[y][x]));
            }
        }

        *outImage = image;

        return true;
    } catch (const std::exception &exc) {
//      qDebug() << exc.what();
        return false;
    }
}

bool EXRHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("EXRHandler::canRead() called with no device");
        return false;
    }

    const QByteArray head = device->peek(4);

    return Imf::isImfMagic(head.data());
}

QImageIOPlugin::Capabilities EXRPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "exr") {
        return Capabilities(CanRead);
    }
    if (!format.isEmpty()) {
        return {};
    }
    if (!device->isOpen()) {
        return {};
    }

    Capabilities cap;
    if (device->isReadable() && EXRHandler::canRead(device)) {
        cap |= CanRead;
    }
    return cap;
}

QImageIOHandler *EXRPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new EXRHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}
