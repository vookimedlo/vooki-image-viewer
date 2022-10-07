/*
    This file is part of the KDE project

    SPDX-FileCopyrightText: 2022 Mirco Miranda <mircomir@outlook.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KIMG_RAW_P_H
#define KIMG_RAW_P_H

#include <QImageIOPlugin>

class RAWHandler : public QImageIOHandler
{
public:
    RAWHandler();

    bool canRead() const override;
    bool read(QImage *image) override;

    void setOption(ImageOption option, const QVariant &value) override;
    bool supportsOption(QImageIOHandler::ImageOption option) const override;
    QVariant option(QImageIOHandler::ImageOption option) const override;

    bool jumpToNextImage() override;
    bool jumpToImage(int imageNumber) override;
    int imageCount() const override;
    int currentImageNumber() const override;

    static bool canRead(QIODevice *device);

private:
    qint32 m_imageNumber;

    mutable qint32 m_imageCount;

    /**
     * @brief m_quality
     * Change the quality of the conversion. If -1, default quality is used.
     * @note Verify that the quality change support has been compiled with supportsOption()
     *
     *   3                   2                 1           0
     * 1 0 9 8 7 6 5 4 3 2 1 0 9 87 6 5 4 3 2 1098 7654 3210
     * _ _ _ _ _ _ _ _ _ _ _ S F NN E H B A W CCCC IIII PPPP
     *
     * Where:
     *
     * _: reserved
     * P: preset values: *** if set, other flags are ignored! ***
     * -  0: Use other flags (no preset)
     * -  1: I =  0, C = 1, B = 0, W = 1, A = 1, H = 1 (Linear, sRGB, 8-bits, Camera White, Auto White, Half-size)
     * -  2: I =  0, C = 1, B = 0, W = 1, A = 1, H = 0 (Linear, sRGB, 8-bits, Camera White, Auto White)
     * -  3: I =  3, C = 1, B = 0, W = 1, A = 1, H = 0 (AHD, sRGB, 8-bits, Camera White, Auto White)
     * -  4: I =  3, C = 1, B = 1, W = 1, A = 1, H = 0 (AHD, sRGB, 16-bits, Camera White, Auto White)
     * -  5: I =  3, C = 2, B = 1, W = 1, A = 1, H = 0 (AHD, Adobe, 16-bits, Camera White, Auto White)
     * -  6: I =  3, C = 4, B = 1, W = 1, A = 1, H = 0 (AHD, ProPhoto, 16-bits, Camera White, Auto White)
     * -  7: I = 11, C = 1, B = 0, W = 1, A = 1, H = 0 (DHT, sRGB, 8-bits, Camera White, Auto White)
     * -  8: I = 11, C = 1, B = 1, W = 1, A = 1, H = 0 (DHT, sRGB, 16-bits, Camera White, Auto White)
     * -  9: I = 11, C = 2, B = 1, W = 1, A = 1, H = 0 (DHT, Adobe, 16-bits, Camera White, Auto White)
     * - 10: I = 11, C = 4, B = 1, W = 1, A = 1, H = 0 (DHT, ProPhoto, 16-bits, Camera White, Auto White)
     * - 11: reserved
     * - 12: reserved
     * - 13: reserved
     * - 14: reserved
     * - 15: reserved
     * I: interpolation quality (0 - linear, 1 - VNG, 2 - PPG, 3 - AHD, 4 - DCB, 11 - DHT, 12 - AAHD)
     * C: output colorspace (0 - raw, 1 - sRGB, 2 - Adobe, 3 - Wide, 4 - ProPhoto, 5 - XYZ, 6 - ACES, 7 - DCI-P3, 8 - Rec2020)
     * W: use camera white balace (0 - off, 1 - on)
     * A: use auto white balance (0 - off, 1 - on)
     * B: output bit per sample (0 - 8-bits, 1 - 16-bits)
     * H: half size image (0 - off, 1 - on)
     * E: DCB color enhance (0 - off, 1 - on)
     * N: FBDD noise reduction (0 - off, 1 - light, 2 - full)
     * F: Interpolate RGGB as four colors (0 - off, 1 - on)
     * S: Don't stretch or rotate raw pixels (0 - rotate and stretch, 1 - don't rotate and stretch)
     *
     * @note It is safe to set both W and A: W is used if camera white balance is found, otherwise A is used.
     */
    qint32 m_quality;
};

class RAWPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "raw.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const override;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const override;
};

#endif // KIMG_RAW_P_H
