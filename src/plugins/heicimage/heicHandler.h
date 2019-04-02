#pragma once
/****************************************************************************
VookiImageViewer - tool to showing images.
Copyright(C) 2019  Michal Duda <github@vookimedlo.cz>

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

#include <QImageIOHandler>

#include <libheif/heif.h>
#include <memory>

class HeicHandler : public QImageIOHandler
{
public:
    HeicHandler() = default;
    HeicHandler(const HeicHandler &handler) = delete;
    HeicHandler &operator=(const HeicHandler &handler) = delete;

    bool canRead() const Q_DECL_OVERRIDE;
    bool read(QImage *image) Q_DECL_OVERRIDE;

    static bool canRead(QIODevice *device);

private:
    struct ReadState
    {
        ReadState(QByteArray &&data, std::shared_ptr<heif_context> &&ctx, std::vector<heif_item_id> &&ids, int index);

        const QByteArray fileData;
        const std::shared_ptr<heif_context> context;
        const std::vector<heif_item_id> idList;
        int currentIndex{};
    };

    /**
     * Updates device and associated state upon a device change.
     */
    void updateDevice();

    /**
     * Reads data from device. Creates a read state.
     */
    void loadContext();

    QIODevice *_device = nullptr;
    std::unique_ptr<ReadState> _readState = nullptr;
};
