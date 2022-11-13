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
        GPS,
        Orientation,
        Flash,
    };

    template <typename T>
    inline void addInformation(const QString &name, const T& value, std::vector<std::pair<QString, QString>> &information) {
        if (!name.isEmpty())
            information.push_back(std::pair{ name, QString::number(value) });
    }

    template <>
    inline void addInformation(const QString &name, const QString &value, std::vector<std::pair<QString, QString>> &information) {
        if (!name.isEmpty() && !value.isEmpty())
            information.push_back(std::pair{ name, value });
    }

    template <>
    inline void addInformation(const QString &name, const std::string &value, std::vector<std::pair<QString, QString>> &information) {
        if (!name.isEmpty() && !value.empty())
            information.push_back(std::pair{ name, value.c_str() });
    }

    template <ExivProcessing processing = ExivProcessing::String>
    inline void addInformation(const QString &name, const Exiv2::ExifData::const_iterator &value, std::vector<std::pair<QString, QString>> &information)
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
                case ExivProcessing::GPS:
                    break;
                case ExivProcessing::String: [[fallthrough]];
                default:
                    addInformation(name, value->getValue()->toString(), information);
                    qDebug() << "EXIF - " << name << ": " << value->getValue()->toString().c_str();
            }
        }
    }

private:
    std::unique_ptr<Exiv2::Image> m_exivImage;
};
