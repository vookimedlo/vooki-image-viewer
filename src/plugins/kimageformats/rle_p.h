/*
 * Run-Length Encoding utilities.
 * Copyright 2014-2015 Alex Merry <alex.merry@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * ----------------------------------------------------------------------------
 */

#ifndef KIMAGEFORMATS_RLE_P_H
#define KIMAGEFORMATS_RLE_P_H

#include <QDebug>
#include <QDataStream>

/**
 * The RLEVariant to use.
 *
 * This mostly concerns what to do values >= 128.
 */
enum class RLEVariant {
    /**
     * PackBits-style RLE
     *
     * Value 128 is ignored, 129 indicates a repetition
     * of size 2, 130 of size 3, up to 255 of size 128.
     */
    PackBits,
    /**
     * PIC-style RLE
     *
     * Value 128 indicates a 16-bit repetition count
     * follows, while 129 indicates a repetition
     * of size 128, 130 of size 127, down to 255 of
     * size 2.
     */
    PIC
};

/**
 * Decodes data written in run-length encoding format.
 *
 * This is intended to be used with lambda functions.
 *
 * Note that this functions expects that, at the current location in @p stream,
 * exactly @p length items have been encoded as a unit (and so it will not be
 * partway through a run when it has decoded @p length items). If this is not
 * the case, it will return @c false.
 *
 * @param variant     The RLE variant to decode.
 * @param stream      The stream to read the data from.
 * @param buf         The location to write the decoded data.
 * @param length      The number of items to read.
 * @param readData    A function that takes a QDataStream reference and reads a
 *                    single value.
 * @param updateItem  A function that takes an item from @p buf and the result
 *                    of a readData call, and produces the item that should be
 *                    written to @p buf.
 *
 * @returns @c true if @p length items in mixed RLE were successfully read
 *          into @p buf, @c false otherwise.
 */
template<typename Item, typename Func1, typename Func2>
static inline bool decodeRLEData(RLEVariant variant,
                                 QDataStream &stream,
                                 Item *dest,
                                 quint32 length,
                                 Func1 readData,
                                 Func2 updateItem)
{
    unsigned offset = 0; // in dest
    while (offset < length) {
        unsigned remaining = length - offset;
        quint8 count1;
        stream >> count1;

        if (count1 >= 128u) {
            unsigned length = 0;
            if (variant == RLEVariant::PIC) {
                if (count1 == 128u) {
                    // If the value is exactly 128, it means that it is more than
                    // 127 repetitions
                    quint16 count2;
                    stream >> count2;
                    length = count2;
                } else {
                    // 2 to 128 repetitions
                    length = count1 - 127u;
                }
            } else if (variant == RLEVariant::PackBits) {
                if (count1 == 128u) {
                    // Ignore value 128
                    continue;
                } else {
                    // 128 to 2 repetitions
                    length = 257u - count1;
                }
            } else {
                Q_ASSERT(false);
            }
            if (length > remaining) {
                qDebug() << "Row overrun:" << length << ">" << remaining;
                return false;
            }
            auto datum = readData(stream);
            for (unsigned i = offset; i < offset + length; ++i) {
                dest[i] = updateItem(dest[i], datum);
            }
            offset += length;
        } else {
            // No repetitions
            unsigned length = count1 + 1u;
            if (length > remaining) {
                qDebug() << "Row overrun:" << length << ">" << remaining;
                return false;
            }
            for (unsigned i = offset; i < offset + length; ++i) {
                auto datum = readData(stream);
                dest[i] = updateItem(dest[i], datum);
            }
            offset += length;
        }
    }
    if (stream.status() != QDataStream::Ok) {
        qDebug() << "DataStream status was" << stream.status();
    }
    return stream.status() == QDataStream::Ok;
}


/**
 * Encodes data in run-length encoding format.
 *
 * This is intended to be used with lambda functions.
 *
 * @param variant     The RLE variant to encode in.
 * @param stream      The stream to write the data to.
 * @param data        The data to be written.
 * @param length      The number of items to write.
 * @param itemsEqual  A function that takes two items and returns whether
 *                    @p writeItem would write them identically.
 * @param writeItem   A function that takes a QDataStream reference and an item
 *                    and writes the item to the data stream.
 */
template<typename Item, typename Func1, typename Func2>
static inline void encodeRLEData(RLEVariant variant,
                                 QDataStream &stream,
		                 const Item *data,
				 unsigned length,
				 Func1 itemsEqual,
				 Func2 writeItem)
{
    unsigned offset = 0;
    const unsigned maxEncodableChunk =
        (variant == RLEVariant::PIC)
        ? 65535u
        : 128;
    while (offset < length) {
        const Item *chunkStart = data + offset;
        unsigned maxChunk = qMin(length - offset, maxEncodableChunk);

        const Item *chunkEnd = chunkStart + 1;
        quint16 chunkLength = 1;
        while (chunkLength < maxChunk && itemsEqual(*chunkStart, *chunkEnd)) {
            ++chunkEnd;
            ++chunkLength;
        }

        if (chunkLength > 128) {
            // Sequence of > 128 identical pixels
            Q_ASSERT(variant == RLEVariant::PIC);
            stream << quint8(128);
            stream << quint16(chunkLength);
            writeItem(stream, *chunkStart);
        } else if (chunkLength > 1) {
            // Sequence of <= 128 identical pixels
            quint8 encodedLength;
            if (variant == RLEVariant::PIC) {
                encodedLength = quint8(chunkLength + 127);
            } else if (variant == RLEVariant::PackBits) {
                encodedLength = quint8(257 - chunkLength);
            } else {
                Q_ASSERT(false);
                encodedLength = 0;
            }
            stream << encodedLength;
            writeItem(stream, *chunkStart);
        } else {
            // find a string of up to 128 values, each different from the one
            // that follows it
            if (maxChunk > 128) {
                maxChunk = 128;
            }
            chunkLength = 1;
            chunkEnd = chunkStart + 1;
            while (chunkLength < maxChunk &&
                    (chunkLength + 1u == maxChunk ||
                     !itemsEqual(*chunkEnd, *(chunkEnd+1))))
            {
                ++chunkEnd;
                ++chunkLength;
            }
            stream << quint8(chunkLength - 1);
            for (unsigned i = 0; i < chunkLength; ++i) {
                writeItem(stream, *(chunkStart + i));
            }
        }
        offset += chunkLength;
    }
}

#endif // KIMAGEFORMATS_RLE_P_H
