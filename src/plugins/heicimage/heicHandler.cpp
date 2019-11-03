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

// Based on https://github.com/jakar/qt-heif-image-plugin - LGPL3

#include "heicHandler.h"
#include <QImage>
#include <QSize>
#include <QVariant>

#include <algorithm>

static_assert(heif_error_Ok == 0, "heif_error_Ok assumed to be 0");

template<class T, class D>
std::unique_ptr<T, D> wrapPointer(T *ptr, D deleter)
{
    return std::unique_ptr<T, D>(ptr, deleter);
}

void HeicHandler::updateDevice()
{
    Q_ASSERT(_device || !_readState);

    if (device() != _device)
    {
        // new device; re-read data
        _device = device();
        _readState.reset();
    }
}
bool HeicHandler::canRead(QIODevice *device)
{
    if (!device)
        return false;

    // read beginning of ftyp box at beginning of file
    constexpr int headerSize = 12;
    const QByteArray header = device->peek(headerSize);

    if (header.size() != headerSize)
        return false;

    // skip first four bytes, which contain box size
    const QByteArray w1 = header.mid(4, 4);
    const QByteArray w2 = header.mid(8, 4);

    // check the box type
    if (w1 != "ftyp")
        return false;

    // brand follows box name, determines format
    return (w2 == "heic" || w2 == "heix");
}

bool HeicHandler::canRead() const
{
    return canRead(device());
}

HeicHandler::ReadState::ReadState(QByteArray &&data, std::shared_ptr<heif_context> &&ctx, std::vector<heif_item_id> &&ids, int index)
                                        : fileData(std::move(data))
                                        , context(std::move(ctx))
                                        , idList(std::move(ids))
                                        , currentIndex(index)
{
}

void HeicHandler::loadContext()
{
    updateDevice();

    if (!device())
        return;

    if (_readState)
    {
        // context already loaded
        return;
    }

    // read file
    auto fileData = device()->readAll();

    if (fileData.isEmpty())
    {
        qDebug("HeicHandler::loadContext() failed to read file data");
        return;
    }

    // set up new context
    std::shared_ptr<heif_context> context(heif_context_alloc(), heif_context_free);
    if (!context)
    {
        qDebug("HeicHandler::loadContext() failed to alloc context");
        return;
    }

    auto error = heif_context_read_from_memory_without_copy(context.get(), fileData.constData(), fileData.size(), nullptr);
    if (error.code)
    {
        qDebug("HeicHandler::loadContext() failed to read context: %s", error.message);
        return;
    }

    const int numImages = heif_context_get_number_of_top_level_images(context.get());
    std::vector<heif_item_id> idList(numImages, 0);
    const int numIdsStored = heif_context_get_list_of_top_level_image_IDs(context.get(), idList.data(), numImages);
    Q_UNUSED(numIdsStored);
    Q_ASSERT(numIdsStored == numImages);

    // find primary image in sequence; no ordering guaranteed for id values
    heif_item_id id{};
    error = heif_context_get_primary_image_ID(context.get(), &id);
    if (error.code)
    {
        qDebug("HeicHandler::loadContext() failed to get primary ID: %s", error.message);
        return;
    }

    auto it = std::find(idList.begin(), idList.end(), id);
    if (it == idList.end())
    {
        qDebug("HeicHandler::loadContext() primary image not found in id list");
        return;
    }

    const auto currentIndex = static_cast<int>(it - idList.begin());
    _readState.reset(new ReadState{ std::move(fileData), std::move(context), std::move(idList), currentIndex });
}

bool HeicHandler::read(QImage *destImage)
{
    if (!destImage)
        return false;

    loadContext();

    if (!_readState)
    {
        qWarning("HeicHandler::read() failed to create context");
        return false;
    }

    const auto idIndex = _readState->currentIndex;
    Q_ASSERT(idIndex >= 0 && static_cast<size_t>(idIndex) < _readState->idList.size());

    const auto id = _readState->idList[idIndex];

    // get image handle
    heif_image_handle *handlePtr = nullptr;
    auto error = heif_context_get_image_handle(_readState->context.get(), id, &handlePtr);

    auto handle = wrapPointer(handlePtr, heif_image_handle_release);
    if (error.code || !handle)
    {
        qDebug("HeicHandler::read() failed to get image handle: %s", error.message);
        return false;
    }

    // decode image
    heif_image *srcImagePtr = nullptr;
    error = heif_decode_image(handle.get(), &srcImagePtr, heif_colorspace_RGB, heif_chroma_interleaved_RGB, nullptr);

    auto srcImage = wrapPointer(srcImagePtr, heif_image_release);
    if (error.code || !srcImage)
    {
        qDebug("HeicHandler::read() failed to decode image: %s", error.message);
        return false;
    }

    const auto channel = heif_channel_interleaved;
    const QSize imgSize(heif_image_get_width(srcImage.get(), channel), heif_image_get_height(srcImage.get(), channel));

    if (!imgSize.isValid())
    {
        qWarning("HeicHandler::read() invalid image size: %d x %d", imgSize.width(), imgSize.height());
        return false;
    }

    auto stride = 0;
    const auto *data = heif_image_get_plane_readonly(srcImage.get(), channel, &stride);

    if (!data)
    {
        qWarning("HeicHandler::read() pixel data not found");
        return false;
    }

    if (stride <= 0)
    {
        qWarning("HeicHandler::read() invalid stride: %d", stride);
        return false;
    }

    // move data ownership to QImage
    heif_image *dataImage = srcImage.release();

    *destImage = QImage(
      data,
      imgSize.width(),
      imgSize.height(),
      stride,
      QImage::Format_RGB888,
      [](void *img) { heif_image_release(static_cast<heif_image *>(img)); },
      dataImage);

    return true;
}
