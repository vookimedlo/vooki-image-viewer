/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include "MetadataExtractor.h"
#include "Exiv2ImageAutoPtrWrapper.h"
#include <QtConcurrent>
#include <qcorofuture.h>

std::vector<QString> MetadataExtractor::m_orientationDescriptions{ "", // EXIF does not use the 0 for the orientation encoding
                                                                   tr("0°", "Image Description"),
                                                                   tr("0°, mirrored", "Image Description"),
                                                                   tr("180°", "Image Description"),
                                                                   tr("180°, mirrored", "Image Description"),
                                                                   tr("90°", "Image Description"),
                                                                   tr("90°, mirrored", "Image Description"),
                                                                   tr("270°", "Image Description"),
                                                                   tr("270°, mirrored", "Image Description") };

//: Units: Byte
const QString MetadataExtractor::m_unitByte{ tr(" B", "Image Description") };
//: Units: Meter
const QString MetadataExtractor::m_unitMeter{ tr(" m", "Image Description") };
//: Units: Pixel
const QString MetadataExtractor::m_unitPixel{ tr(" px", "Image Description") };
//: Units: Second
const QString MetadataExtractor::m_unitSecond{ tr(" s", "Image Description") };

QCoro::Task<void> MetadataExtractor::extract(const QString &filename, const int width, const int height)
{
    m_gpsLatitude.clear();
    m_gpsLongitude.clear();
    m_gpsAltitude.clear();

    const QFileInfo fileInfo(filename);
    InformationMap information{ std::make_pair(tr("File name", "Image Properties"), fileInfo.fileName()) };
    addInformation(tr("Size", "Image Properties"), fileInfo.size(), information, MetadataExtractor::m_unitByte);
    addInformation(tr("Width", "Image Properties"), width, information, MetadataExtractor::m_unitPixel);
    addInformation(tr("Height", "Image Properties"), height, information, MetadataExtractor::m_unitPixel);

    emit imageSizeParsed(fileInfo.size());
    emit imageDimensionsParsed(width, height);

    // Use co_await to make the potentially blocking operations asynchronous
    co_await QtConcurrent::run([this, filename, &information]() {
        try
        {
            m_exivImage = Exiv2ImageAutoPtrWrapper::open(filename.toStdString());
            m_exivImage->readMetadata();
            const Exiv2::ExifData &exifData = m_exivImage->exifData();

            // Extract basic image properties
            extractBasicProperties(exifData, information);

            // Extract GPS information
            extractGPSInformation(exifData, information);

            // Extract camera and lens information
            extractCameraInformation(exifData, information);

            // Debug output
            logAllExifTags(exifData);
        }
#if EXIV2_TEST_VERSION(0, 28, 0)
        catch (const Exiv2::Error &error)
#else
        catch (const Exiv2::AnyError &error)
#endif
        {
            qDebug() << "Cannot parse the EXIF/XMP data:" << error.what() << "Skipping...";
        }
    });

    emit imageInformationParsed(information);
    co_return;
}

void MetadataExtractor::extractBasicProperties(const Exiv2::ExifData &exifData, InformationMap &information) {
    using enum ExivProcessing;
    addInformation<Orientation>(tr("Orientation", "Image Properties"),
                                              Exiv2::orientation(exifData), information);
#if EXIV2_TEST_VERSION(0, 27, 4)
    addInformation(tr("Date", "Image Properties"), 
                  Exiv2::dateTimeOriginal(exifData), information);
    
    addInformation<Flash>(tr("Flash", "Image Properties"),
                                        Exiv2::flash(exifData), information);
#endif
    addInformation(tr("ISO", "Image Properties"), 
                  Exiv2::isoSpeed(exifData), information);
    
    addInformation<Float>(tr("f-number", "Image Properties"),
                                        Exiv2::fNumber(exifData), information);
    
    addInformation<Float>(tr("Exposure time", "Image Properties"),
                                        Exiv2::exposureTime(exifData), information);
    
    addInformation<Float>(tr("Focal length", "Image Properties"),
                                        Exiv2::focalLength(exifData), information);
}

void MetadataExtractor::extractGPSInformation(const Exiv2::ExifData &exifData, InformationMap &information) {
    // GPS constants
    constexpr const char* GPS_LATITUDE_TAG = "Exif.GPSInfo.GPSLatitude";
    constexpr const char* GPS_LATITUDE_REF_TAG = "Exif.GPSInfo.GPSLatitudeRef";
    constexpr const char* GPS_LONGITUDE_TAG = "Exif.GPSInfo.GPSLongitude";
    constexpr const char* GPS_LONGITUDE_REF_TAG = "Exif.GPSInfo.GPSLongitudeRef";
    constexpr const char* GPS_ALTITUDE_TAG = "Exif.GPSInfo.GPSAltitude";
    constexpr const char* GPS_ALTITUDE_REF_TAG = "Exif.GPSInfo.GPSAltitudeRef";

    using enum ExivProcessing;

    // Add GPS latitude information
    addInformation<GPSLatitude>("",
                                              exifData.findKey(Exiv2::ExifKey(GPS_LATITUDE_TAG)), 
                                              information);
    
    addInformation<GPSLatitudeRef>(tr("GPS Latitude", "Image Properties"),
                                                 exifData.findKey(Exiv2::ExifKey(GPS_LATITUDE_REF_TAG)), 
                                                 information);
    
    // Add GPS longitude information
    addInformation<GPSLongitude>("",
                                               exifData.findKey(Exiv2::ExifKey(GPS_LONGITUDE_TAG)), 
                                               information);
    
    addInformation<GPSLongitudeRef>(tr("GPS Longitude", "Image Properties"),
                                                  exifData.findKey(Exiv2::ExifKey(GPS_LONGITUDE_REF_TAG)), 
                                                  information);
    
    // Add GPS altitude information
    addInformation<GPSAltitude>("",
                                              exifData.findKey(Exiv2::ExifKey(GPS_ALTITUDE_TAG)), 
                                              information, 
                                              MetadataExtractor::m_unitMeter);
    
    addInformation<GPSAltitudeRef>(tr("GPS Altitude", "Image Properties"),
                                                 exifData.findKey(Exiv2::ExifKey(GPS_ALTITUDE_REF_TAG)), 
                                                 information);
}

void MetadataExtractor::extractCameraInformation(const Exiv2::ExifData &exifData, InformationMap &information) {
    // Camera and lens constants
    constexpr const char* CAMERA_MAKE_TAG = "Exif.Image.Make";
    constexpr const char* LENS_MAKE_TAG = "Exif.Photo.LensMake";

    using enum ExivProcessing;

    // Add camera information
    addInformation(tr("Camera maker", "Image Properties"), 
                  exifData.findKey(Exiv2::ExifKey(CAMERA_MAKE_TAG)), 
                  information);
    
    addInformation(tr("Camera model", "Image Properties"), 
                  Exiv2::model(exifData), 
                  information);
    
    // Add lens information
    addInformation(tr("Lens maker", "Image Properties"), 
                  exifData.findKey(Exiv2::ExifKey(LENS_MAKE_TAG)), 
                  information);
    
    addInformation(tr("Lens model", "Image Properties"), 
                  Exiv2::lensName(exifData), 
                  information);
    
    addInformation<Float>(tr("Lens focal length", "Image Properties"),
                                        Exiv2::focalLength(exifData), 
                                        information);
}

void MetadataExtractor::logAllExifTags(const Exiv2::ExifData &exifData) {
    for (const auto &it : exifData) {
        qDebug() << "+ " << it.key().c_str() << " - " << it.value().toString().c_str();
    }
}
