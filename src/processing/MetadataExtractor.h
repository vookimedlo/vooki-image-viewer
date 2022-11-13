#pragma once
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

#include <QDebug>
#include <QObject>
#include <QString>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace std
{
    template<typename T>
    using auto_ptr = std::unique_ptr<T>;
}
#include <exiv2/exiv2.hpp>

class MetadataExtractor : public QObject
{
    Q_OBJECT

public:
    MetadataExtractor();
    virtual ~MetadataExtractor();

    virtual void extract(const QString &filename, int width, int height);

signals:
    void imageInformationParsed(const std::vector<std::pair<QString, QString>>& information);

protected:
    enum class ExivProcessing
    {
        String,
        Float,
        Int,
        GPSLatitude,
        GPSLongitude,
        GPSAltitude,
        GPSLatitudeRef,
        GPSLongitudeRef,
        GPSAltitudeRef,
        Orientation,
        Flash,
    };

    template <typename T>
    inline void addInformation(const QString &name, const T& value, std::vector<std::pair<QString, QString>> &information, const QString &units = QString("")) {
        if (!name.isEmpty())
            information.push_back(std::pair{ name, QString::number(value) + units });
    }

    template <>
    inline void addInformation(const QString &name, const QString &value, std::vector<std::pair<QString, QString>> &information, const QString &units) {
        if (!name.isEmpty() && !value.isEmpty())
            information.push_back(std::pair{ name, value + units });
    }

    template <>
    inline void addInformation(const QString &name, const std::string &value, std::vector<std::pair<QString, QString>> &information, const QString &units) {
        if (!name.isEmpty() && !value.empty())
            information.push_back(std::pair{ name, value.c_str() + units });
    }

    template <ExivProcessing processing = ExivProcessing::String>
    inline void addInformation(const QString &name, const Exiv2::ExifData::const_iterator &value, std::vector<std::pair<QString, QString>> &information, const QString &units = QString(""))
    {
        if (value != m_exivImage->exifData().end())
        {
            switch (processing)
            {
                case ExivProcessing::Float:
                    addInformation(name, value->getValue()->toFloat(), information);
                    qDebug() << "EXIF - " << name << ": " << value->getValue()->toFloat();
                    break;
                case ExivProcessing::Int:
                    addInformation(name, value->getValue()->toFloat(), information);
                    qDebug() << "EXIF - " << name << ": " << value->getValue()->toLong();
                    break;
                case ExivProcessing::Orientation:
                    if (value->getValue()->toLong() == 0 || value->getValue()->toLong() >= static_cast<long>(m_orientationDescriptions.size()))
                        return;

                    addInformation(name, m_orientationDescriptions[value->getValue()->toLong()], information);
                    qDebug() << "EXIF - " << name << ": " << value->getValue()->toString().c_str();
                    break;
                case ExivProcessing::Flash:
                    addInformation(name, value->getValue()->toLong() & 0x01 ? tr("Flash fired", "Image Description") : tr("Flash didn't fired", "Image Description"), information);
                    break;
                case ExivProcessing::GPSLatitude: {
                    QString floats[3];
                    for (int i = 0; i < 3; ++i)
                        floats[i] = QString::number(value->getValue()->toFloat(i));
                    m_gpsLatitude = QString("%1° %2′ %3″").arg(floats[0], floats[1], floats[2]);
                    }
                    break;
                case ExivProcessing::GPSLatitudeRef:
                    m_gpsLatitude.append(" ");
                    m_gpsLatitude.append(value->getValue()->toString().c_str());
                    addInformation(name, m_gpsLatitude, information);
                    break;
                case ExivProcessing::GPSLongitude: {
                        QString floats[3];
                        for (int i = 0; i < 3; ++i)
                            floats[i] = QString::number(value->getValue()->toFloat(i));
                        m_gpsLongitude = QString("%1° %2′ %3″").arg(floats[0], floats[1], floats[2]);
                    }
                    break;
                case ExivProcessing::GPSLongitudeRef:
                    m_gpsLongitude.append(" ");
                    m_gpsLongitude.append(value->getValue()->toString().c_str());
                    addInformation(name, m_gpsLongitude, information);
                    break;
                case ExivProcessing::GPSAltitude:
                    m_gpsAltitude = QString::number(value->getValue()->toFloat()) + units;
                    break;
                case ExivProcessing::GPSAltitudeRef:
                    m_gpsAltitude.append(value->getValue()->toLong() ? tr(" (below sea level)", "Image Description") : tr(" (above sea level)", "Image Description"));
                    addInformation(name, m_gpsAltitude, information);
                    break;
                case ExivProcessing::String: [[fallthrough]];
                default:
                    addInformation(name, value->getValue()->toString(), information);
                    qDebug() << "EXIF - " << name << ": " << value->getValue()->toString().c_str();
            }
        }
    }

private:
    QString m_gpsLatitude;
    QString m_gpsLongitude;
    QString m_gpsAltitude;

    std::unique_ptr<Exiv2::Image> m_exivImage;
    static std::vector<QString> m_orientationDescriptions;
    static const QString m_unitByte;
    static const QString m_unitMeter;
    static const QString m_unitPixel;
    static const QString m_unitSecond;
};