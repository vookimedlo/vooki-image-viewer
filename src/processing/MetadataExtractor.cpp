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

MetadataExtractor::MetadataExtractor()
{

}

MetadataExtractor::~MetadataExtractor()
{

}

void MetadataExtractor::extract(const QString &filename, int width, int height)
{
    QFileInfo fileInfo(filename);
    std::vector<std::pair<QString, QString>> information{ std::pair{tr("File name", "Image Properties"), fileInfo.fileName()} };
    addInformation(tr("Size", "Image Properties"), fileInfo.size(), information);
    addInformation(tr("Width", "Image Properties"), width, information);
    addInformation(tr("Height", "Image Properties"), height, information);

    try
    {
        m_exivImage = Exiv2::ImageFactory::open(filename.toStdString());
        m_exivImage->readMetadata();
        const Exiv2::ExifData &exifData = m_exivImage->exifData();
        addInformation(tr("Orientation", "Image Properties"), Exiv2::orientation(exifData), information);
        addInformation(tr("Date", "Image Properties"), Exiv2::dateTimeOriginal(exifData), information);

        addInformation(tr("Flash", "Image Properties"), Exiv2::flash(exifData), information);
        addInformation(tr("ISO", "Image Properties"), Exiv2::isoSpeed(exifData), information);

        addInformation<ExivProcessing::Float>(tr("f-number", "Image Properties"), Exiv2::fNumber(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Exposure time", "Image Properties"), Exiv2::exposureTime(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Shutter speed", "Image Properties"), Exiv2::shutterSpeedValue(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Focal length", "Image Properties"), Exiv2::focalLength(exifData), information);

        addInformation(tr("Camera maker", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.Image.Make")), information);
        addInformation(tr("Camera model", "Image Properties"), Exiv2::model(exifData), information);
        addInformation(tr("Lens maker", "Image Properties"), exifData.findKey(Exiv2::ExifKey("Exif.Photo.LensMake")), information);
        addInformation(tr("Lens model", "Image Properties"), Exiv2::lensName(exifData), information);
        addInformation<ExivProcessing::Float>(tr("Lens focal length", "Image Properties"), Exiv2::focalLength(exifData), information);


        for (const auto &it : exifData)
            qDebug() << "+ " << it.key().c_str() << " - " << it.value().toString().c_str();
    }
    catch (...)
    {
        qDebug() << "Cannot parse the EXIF/XMP data. Skipping ...";
    }

    emit imageInformationParsed(information);
}
