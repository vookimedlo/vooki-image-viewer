/****************************************************************************
VookiImageViewer - a tool for showing images.
Copyright(C) 2022  Michal Duda <github@vookimedlo.cz>

https://github.com/vookimedlo/vooki-image-viewer

This program is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "MetadataExtractor.h"
#include <QFileInfo>
#include "Exiv2ImageAutoPtrWrapper.h"

std::vector<QString> MetadataExtractor::m_orientationDescriptions = { "", // EXIF does not use the 0 for the orientation encoding
                                                                      "0°",
                                                                      tr("0°, mirrored", "Image Description"),
                                                                      "180°",
                                                                      tr("180°, mirrored", "Image Description"),
                                                                      "90°",
                                                                      tr("90°, mirrored", "Image Description"),
                                                                      "270°",
                                                                      tr("270°, mirrored", "Image Description")
};

const QString MetadataExtractor::m_unitByte = tr(" b", "Image Description - Units: byte");
const QString MetadataExtractor::m_unitMeter = tr(" m", "Image Description - Units: meter");
const QString MetadataExtractor::m_unitPixel = tr(" px", "Image Description - Units: pixel");
const QString MetadataExtractor::m_unitSecond = tr(" s", "Image Description - Units: second");

void MetadataExtractor::extract(const QString &filename, int width, int height)
{
    m_gpsLatitude.clear();
    m_gpsLongitude.clear();
    m_gpsAltitude.clear();

    QFileInfo fileInfo(filename);
    std::vector<std::pair<QString, QString>> information{ std::pair{tr("File name", "Image Properties"), fileInfo.fileName()} };
    addInformation(tr("Size", "Image Properties"), fileInfo.size(), information, MetadataExtractor::m_unitByte);
    addInformation(tr("Width", "Image Properties"), width, information, MetadataExtractor::m_unitPixel);
    addInformation(tr("Height", "Image Properties"), height, information, MetadataExtractor::m_unitPixel);

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
        addInformation<ExivProcessing::GPSLatitudeRef>("GPS Latitude", exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitudeRef")), information);
        addInformation<ExivProcessing::GPSLongitude>("", exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude")), information);
        addInformation<ExivProcessing::GPSLongitudeRef>("GPS Longitude", exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitudeRef")), information);
        addInformation<ExivProcessing::GPSAltitude>("", exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitude")), information, MetadataExtractor::m_unitMeter);
        addInformation<ExivProcessing::GPSAltitudeRef>("GPS Altitude", exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef")), information);

        #if EXIV2_TEST_VERSION(0,27,4)
            addInformation<ExivProcessing::Flash>(tr("Flash", "Image Properties"), Exiv2::flash(exifData), information);
        #endif
        addInformation(tr("ISO", "Image Properties"), Exiv2::isoSpeed(exifData), information);

        addInformation<ExivProcessing::Float>(tr("f-number", "Image Properties"), Exiv2::fNumber(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Exposure time", "Image Properties"), Exiv2::exposureTime(exifData), information);

        #if EXIV2_TEST_VERSION(0,27,4)
            addInformation<ExivProcessing::Float>(tr("Shutter speed", "Image Properties"), Exiv2::shutterSpeedValue(exifData), information);
        #endif
        addInformation<ExivProcessing::Float>(tr("Focal length", "Image Properties"), Exiv2::focalLength(exifData), information);

        addInformation(tr("Camera maker", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.Image.Make")), information);
        addInformation(tr("Camera model", "Image Properties"), Exiv2::model(exifData), information);
        addInformation(tr("Lens maker", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.Photo.LensMake")), information);
        addInformation(tr("Lens model", "Image Properties"), Exiv2::lensName(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Lens focal length", "Image Properties"), Exiv2::focalLength(exifData), information);

        for (const auto &it : exifData)
            qDebug() << "+ " << it.key().c_str() << " - " << it.value().toString().c_str();
    }
    catch (const Exiv2::AnyError&)
    {
        qDebug() << "Cannot parse the EXIF/XMP data. Skipping ...";
    }

    emit imageInformationParsed(information);
}
