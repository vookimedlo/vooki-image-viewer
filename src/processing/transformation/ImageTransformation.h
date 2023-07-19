#pragma once
/****************************************************************************
VookiImageViewer - a tool for showing images.
- https://github.com/vookimedlo/vooki-image-viewer

  SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
  SPDX-License-Identifier: GPL-3.0-or-later
  SPDX-FileType: SOURCE

****************************************************************************/

class ImageTransformation
{
public:
    virtual ~ImageTransformation() = default;

    [[nodiscard]] virtual QVariant transform() = 0;

    [[nodiscard]] inline bool isCacheDirty() const { return m_isCacheDirty; }
    inline void setIsCacheDirty(bool isCacheDirty) { m_isCacheDirty = isCacheDirty; }

    virtual void resetProperties() { m_isCacheDirty = true; }

private:
    bool m_isCacheDirty {true};
};
