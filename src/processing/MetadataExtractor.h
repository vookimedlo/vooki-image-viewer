#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2022 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

#include <QDebug>
#include <QObject>
#include <QString>
#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <qcorotask.h>
#include "AutoPtrWrapper.h"
#include <exiv2/exiv2.hpp>
#include "../util/compiler.h"

class MetadataExtractor : public QObject
{
    Q_OBJECT

public:
    MetadataExtractor() = default;
    ~MetadataExtractor() override = default;
    DISABLE_COPY_MOVE(MetadataExtractor);

    virtual QCoro::Task<void> extract(const QString &filename, int width, int height);

signals:
    void imageInformationParsed(const std::vector<std::pair<QString, QString>>& information);
    void imageDimensionsParsed(int width, int height);
    void imageSizeParsed(uint64_t size);

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
            information.emplace_back(name, QString::number(value) + units );
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
                    qDebug() << "EXIF - " << name << ": " << toLong(value->getValue());
                    break;
                case ExivProcessing::Orientation:
                    if (toLong(value->getValue()) == 0 || toLong(value->getValue()) >= static_cast<long>(m_orientationDescriptions.size()))
                        return;

                    addInformation(name, m_orientationDescriptions[toLong(value->getValue())], information);
                    qDebug() << "EXIF - " << name << ": " << value->getValue()->toString().c_str();
                    break;
                case ExivProcessing::Flash:
                    addInformation(name, toLong(value->getValue()) & 0x01 ? tr("Flash fired", "Image Description") : tr("Flash didn't fired", "Image Description"), information);
                    break;
                case ExivProcessing::GPSLatitude:
                    m_gpsLatitude = decodeGps(value);
                    break;
                case ExivProcessing::GPSLatitudeRef:
                    m_gpsLatitude.append(" ");
                    m_gpsLatitude.append(value->getValue()->toString().c_str());
                    addInformation(name, m_gpsLatitude, information);
                    break;
                case ExivProcessing::GPSLongitude:
                    m_gpsLongitude = decodeGps(value);
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
                    m_gpsAltitude.append(toLong(value->getValue()) ? tr(" (below sea level)", "Image Description") : tr(" (above sea level)", "Image Description"));
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
    [[nodiscard]] int64_t toLong(std::unique_ptr<Exiv2::Value> value) const
    {
#if EXIV2_TEST_VERSION(0,28,0)
        return value->toInt64();
#else
        return value->toLong();
#endif
    }

    template<std::size_t... I1>
    [[nodiscard]] inline QString decodeGpsImpl(const Exiv2::ExifData::const_iterator &value,
                                 std::index_sequence<I1...>)
    {
        return QString{"%1° %2′ %3″"}.arg(QString::number(value->getValue()->toFloat(I1))...);
    }

    template<int count = 3>
    [[nodiscard]] inline QString decodeGps(const Exiv2::ExifData::const_iterator &value)
    {
        return count <= value->count() ? decodeGpsImpl(value, std::make_index_sequence<count>{}) : "";
    }

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

template <>
inline void MetadataExtractor::addInformation<QString>(const QString &name, const QString &value, std::vector<std::pair<QString, QString>> &information, const QString &units) {
    if (!name.isEmpty() && !value.isEmpty())
        information.emplace_back( name, value + units );
}

template <>
inline void MetadataExtractor::addInformation<std::string>(const QString &name, const std::string &value, std::vector<std::pair<QString, QString>> &information, const QString &units) {
    if (!name.isEmpty() && !value.empty())
        information.emplace_back( name, value.c_str() + units );
}
