/* This file is part of the KDE project
   Copyright (C) 2005 Christoph Hormann <chris_hormann@gmx.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the Lesser GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/

#ifndef KIMG_HDR_P_H
#define KIMG_HDR_P_H

class QImageIO;

extern "C" {
    void kimgio_hdr_read(QImageIO *);
    void kimgio_hdr_write(QImageIO *);
}

#endif

