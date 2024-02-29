/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "MetadataExtractor.h"
#include <QFileInfo>
#include "Exiv2ImageAutoPtrWrapper.h"

std::vector<QString> MetadataExtractor::m_orientationDescriptions {"", // EXIF does not use the 0 for the orientation encoding
                                                                   tr("0°", "Image Description"),
                                                                   tr("0°, mirrored", "Image Description"),
                                                                   tr("180°", "Image Description"),
                                                                   tr("180°, mirrored", "Image Description"),
                                                                   tr("90°", "Image Description"),
                                                                   tr("90°, mirrored", "Image Description"),
                                                                   tr("270°", "Image Description"),
                                                                   tr("270°, mirrored", "Image Description")
};

//: Units: Byte
const QString MetadataExtractor::m_unitByte {tr(" B", "Image Description")};
//: Units: Meter
const QString MetadataExtractor::m_unitMeter {tr(" m", "Image Description")};
//: Units: Pixel
const QString MetadataExtractor::m_unitPixel {tr(" px", "Image Description")};
//: Units: Second
const QString MetadataExtractor::m_unitSecond {tr(" s", "Image Description")};

void MetadataExtractor::extract(const QString &filename, int width, int height)
{
    m_gpsLatitude.clear();
    m_gpsLongitude.clear();
    m_gpsAltitude.clear();

    const QFileInfo fileInfo(filename);
    std::vector<std::pair<QString, QString>> information{ std::make_pair(tr("File name", "Image Properties"), fileInfo.fileName()) };
    addInformation(tr("Size", "Image Properties"), fileInfo.size(), information, MetadataExtractor::m_unitByte);
    addInformation(tr("Width", "Image Properties"), width, information, MetadataExtractor::m_unitPixel);
    addInformation(tr("Height", "Image Properties"), height, information, MetadataExtractor::m_unitPixel);

    emit imageSizeParsed(fileInfo.size());
    emit imageDimensionsParsed(width, height);

    try
    {
        m_exivImage = Exiv2ImageAutoPtrWrapper::open(filename.toStdString());
        m_exivImage->readMetadata();
        const Exiv2::ExifData &exifData = m_exivImage->exifData();
        addInformation<ExivProcessing::Orientation>(tr("Orientation", "Image Properties"), Exiv2::orientation(exifData), information);

        #if EXIV2_TEST_VERSION(0,27,4)
            addInformation(tr("Date", "Image Properties"), Exiv2::dateTimeOriginal(exifData), information);
        #endif

        addInformation<ExivProcessing::GPSLatitude>("", exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitude")), information);
        addInformation<ExivProcessing::GPSLatitudeRef>(tr("GPS Latitude", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitudeRef")), information);
        addInformation<ExivProcessing::GPSLongitude>("", exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude")), information);
        addInformation<ExivProcessing::GPSLongitudeRef>(tr("GPS Longitude", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitudeRef")), information);
        addInformation<ExivProcessing::GPSAltitude>("", exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitude")), information, MetadataExtractor::m_unitMeter);
        addInformation<ExivProcessing::GPSAltitudeRef>(tr("GPS Altitude", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef")), information);

        #if EXIV2_TEST_VERSION(0,27,4)
            addInformation<ExivProcessing::Flash>(tr("Flash", "Image Properties"), Exiv2::flash(exifData), information);
        #endif
        addInformation(tr("ISO", "Image Properties"), Exiv2::isoSpeed(exifData), information);

        addInformation<ExivProcessing::Float>(tr("f-number", "Image Properties"), Exiv2::fNumber(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Exposure time", "Image Properties"), Exiv2::exposureTime(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Focal length", "Image Properties"), Exiv2::focalLength(exifData), information);

        addInformation(tr("Camera maker", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.Image.Make")), information);
        addInformation(tr("Camera model", "Image Properties"), Exiv2::model(exifData), information);
        addInformation(tr("Lens maker", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.Photo.LensMake")), information);
        addInformation(tr("Lens model", "Image Properties"), Exiv2::lensName(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Lens focal length", "Image Properties"), Exiv2::focalLength(exifData), information);

        for (const auto &it : exifData)
            qDebug() << "+ " << it.key().c_str() << " - " << it.value().toString().c_str();
    }
#if EXIV2_TEST_VERSION(0,28,0)
    catch (const Exiv2::Error&)
#else
    catch (const Exiv2::AnyError&)
#endif
    {
        qDebug() << "Cannot parse the EXIF/XMP data. Skipping ...";
    }

    emit imageInformationParsed(information);
}
