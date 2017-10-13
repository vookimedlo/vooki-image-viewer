#ifndef GIMP_H
#define GIMP_H
/* -*- c++ -*-
 * gimp.h: Header for a Qt 3 plug-in for reading GIMP XCF image files
 * Copyright (C) 2001 lignum Computing, Inc. <allen@lignumcomputing.com>
 * Copyright (C) 2004 Melchior FRANZ <mfranz@kde.org>
 *
 * This plug-in is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

typedef unsigned char uchar;

/*
 * These are the constants and functions I extracted from The GIMP source
 * code. If the reader fails to work, this is probably the place to start
 * looking for discontinuities.
 */

// From GIMP "tile.h" v1.2

const uint TILE_WIDTH = 64; //!< Width of a tile in the XCF file.
const uint TILE_HEIGHT = 64;    //!< Height of a tile in the XCF file.

// From GIMP "paint_funcs.c" v1.2

const int RANDOM_TABLE_SIZE = 4096; //!< Size of dissolve random number table.
const int RANDOM_SEED = 314159265; //!< Seed for dissolve random number table.
const double EPSILON = 0.0001;  //!< Roundup in alpha blending.

// From GIMP "paint_funcs.h" v1.2

const uchar OPAQUE_OPACITY = 255; //!< Opaque value for 8-bit alpha component.

// From GIMP "apptypes.h" v1.2

//! Basic GIMP image type. QImage converter may produce a deeper image
//! than is specified here. For example, a grayscale image with an
//! alpha channel must (currently) use a 32-bit Qt image.

typedef enum {
    RGB,
    GRAY,
    INDEXED
} GimpImageBaseType;

//! Type of individual layers in an XCF file.

typedef enum {
    RGB_GIMAGE,
    RGBA_GIMAGE,
    GRAY_GIMAGE,
    GRAYA_GIMAGE,
    INDEXED_GIMAGE,
    INDEXEDA_GIMAGE
} GimpImageType;

// From GIMP "libgimp/gimpenums.h" v2.4

//! Effect to apply when layers are merged together.

typedef enum {
    NORMAL_MODE,
    DISSOLVE_MODE,
    BEHIND_MODE,
    MULTIPLY_MODE,
    SCREEN_MODE,
    OVERLAY_MODE,
    DIFFERENCE_MODE,
    ADDITION_MODE,
    SUBTRACT_MODE,
    DARKEN_ONLY_MODE,
    LIGHTEN_ONLY_MODE,
    HUE_MODE,
    SATURATION_MODE,
    COLOR_MODE,
    VALUE_MODE,
    DIVIDE_MODE,
    DODGE_MODE,
    BURN_MODE,
    HARDLIGHT_MODE,
    SOFTLIGHT_MODE,
    GRAIN_EXTRACT_MODE,
    GRAIN_MERGE_MODE
} LayerModeEffects;

// From GIMP "xcf.c" v1.2

//! Properties which can be stored in an XCF file.

typedef enum {
    PROP_END = 0,
    PROP_COLORMAP = 1,
    PROP_ACTIVE_LAYER = 2,
    PROP_ACTIVE_CHANNEL = 3,
    PROP_SELECTION = 4,
    PROP_FLOATING_SELECTION = 5,
    PROP_OPACITY = 6,
    PROP_MODE = 7,
    PROP_VISIBLE = 8,
    PROP_LINKED = 9,
    PROP_PRESERVE_TRANSPARENCY = 10,
    PROP_APPLY_MASK = 11,
    PROP_EDIT_MASK = 12,
    PROP_SHOW_MASK = 13,
    PROP_SHOW_MASKED = 14,
    PROP_OFFSETS = 15,
    PROP_COLOR = 16,
    PROP_COMPRESSION = 17,
    PROP_GUIDES = 18,
    PROP_RESOLUTION = 19,
    PROP_TATTOO = 20,
    PROP_PARASITES = 21,
    PROP_UNIT = 22,
    PROP_PATHS = 23,
    PROP_USER_UNIT = 24
} PropType;

// From GIMP "xcf.c" v1.2

//! Compression type used in layer tiles.

typedef enum {
    COMPRESS_NONE = 0,
    COMPRESS_RLE = 1,
    COMPRESS_ZLIB = 2,
    COMPRESS_FRACTAL = 3  /* Unused. */
} CompressionType;

// From GIMP "paint_funcs.c" v1.2

/*!
 * Multiply two color components. Really expects the arguments to be
 * 8-bit quantities.
 * \param a first minuend.
 * \param b second minuend.
 * \return product of arguments.
 */
inline int INT_MULT(int a, int b)
{
    int c = a * b + 0x80;
    return ((c >> 8) + c) >> 8;
}

/*!
 * Blend the two color components in the proportion alpha:
 *
 * result = alpha a + ( 1 - alpha ) b
 *
 * \param a first component.
 * \param b second component.
 * \param alpha blend proportion.
 * \return blended color components.
 */

inline int INT_BLEND(int a, int b, int alpha)
{
    return INT_MULT(a - b, alpha) + b;
}

// From GIMP "gimpcolorspace.c" v1.2

/*!
 * Convert a color in RGB space to HSV space (Hue, Saturation, Value).
 * \param red the red component (modified in place).
 * \param green the green component (modified in place).
 * \param blue the blue component (modified in place).
 */
static void RGBTOHSV(uchar &red, uchar &green, uchar &blue)
{
    int r, g, b;
    double h, s, v;
    int min, max;

    h = 0.;

    r = red;
    g = green;
    b = blue;

    if (r > g) {
        max = qMax(r, b);
        min = qMin(g, b);
    } else {
        max = qMax(g, b);
        min = qMin(r, b);
    }

    v = max;

    if (max != 0) {
        s = ((max - min) * 255) / (double)max;
    } else {
        s = 0;
    }

    if (s == 0) {
        h = 0;
    } else {
        int delta = max - min;
        if (r == max) {
            h = (g - b) / (double)delta;
        } else if (g == max) {
            h = 2 + (b - r) / (double)delta;
        } else if (b == max) {
            h = 4 + (r - g) / (double)delta;
        }
        h *= 42.5;

        if (h < 0) {
            h += 255;
        }
        if (h > 255) {
            h -= 255;
        }
    }

    red   = (uchar)h;
    green = (uchar)s;
    blue  = (uchar)v;
}

/*!
 * Convert a color in HSV space to RGB space.
 * \param hue the hue component (modified in place).
 * \param saturation the saturation component (modified in place).
 * \param value the value component (modified in place).
 */
static void HSVTORGB(uchar &hue, uchar &saturation, uchar &value)
{
    if (saturation == 0) {
        hue        = value;
        saturation = value;
        //value      = value;
    } else {
        double h = hue * 6. / 255.;
        double s = saturation / 255.;
        double v = value / 255.;

        double f = h - (int)h;
        double p = v * (1. - s);
        double q = v * (1. - (s * f));
        double t = v * (1. - (s * (1. - f)));

        // Worth a note here that gcc 2.96 will generate different results
        // depending on optimization mode on i386.

        switch ((int)h) {
        case 0:
            hue        = (uchar)(v * 255);
            saturation = (uchar)(t * 255);
            value      = (uchar)(p * 255);
            break;
        case 1:
            hue        = (uchar)(q * 255);
            saturation = (uchar)(v * 255);
            value      = (uchar)(p * 255);
            break;
        case 2:
            hue        = (uchar)(p * 255);
            saturation = (uchar)(v * 255);
            value      = (uchar)(t * 255);
            break;
        case 3:
            hue        = (uchar)(p * 255);
            saturation = (uchar)(q * 255);
            value      = (uchar)(v * 255);
            break;
        case 4:
            hue        = (uchar)(t * 255);
            saturation = (uchar)(p * 255);
            value      = (uchar)(v * 255);
            break;
        case 5:
            hue        = (uchar)(v * 255);
            saturation = (uchar)(p * 255);
            value      = (uchar)(q * 255);
        }
    }
}

/*!
 * Convert a color in RGB space to HLS space (Hue, Lightness, Saturation).
 * \param red the red component (modified in place).
 * \param green the green component (modified in place).
 * \param blue the blue component (modified in place).
 */
static void RGBTOHLS(uchar &red, uchar &green, uchar &blue)
{
    int r = red;
    int g = green;
    int b = blue;

    int min, max;

    if (r > g) {
        max = qMax(r, b);
        min = qMin(g, b);
    } else {
        max = qMax(g, b);
        min = qMin(r, b);
    }

    double h;
    double l = (max + min) / 2.;
    double s;

    if (max == min) {
        s = 0.;
        h = 0.;
    } else {
        int delta = max - min;

        if (l < 128) {
            s = 255 * (double)delta / (double)(max + min);
        } else {
            s = 255 * (double)delta / (double)(511 - max - min);
        }

        if (r == max) {
            h = (g - b) / (double)delta;
        } else if (g == max) {
            h = 2 + (b - r) / (double)delta;
        } else {
            h = 4 + (r - g) / (double)delta;
        }

        h *= 42.5;

        if (h < 0) {
            h += 255;
        } else if (h > 255) {
            h -= 255;
        }
    }

    red   = (uchar)h;
    green = (uchar)l;
    blue  = (uchar)s;
}

/*!
 * Implement the HLS "double hex-cone".
 * \param n1 lightness fraction (?)
 * \param n2 saturation fraction (?)
 * \param hue hue "angle".
 * \return HLS value.
 */
static int HLSVALUE(double n1, double n2, double hue)
{
    double value;

    if (hue > 255) {
        hue -= 255;
    } else if (hue < 0) {
        hue += 255;
    }

    if (hue < 42.5) {
        value = n1 + (n2 - n1) * (hue / 42.5);
    } else if (hue < 127.5) {
        value = n2;
    } else if (hue < 170) {
        value = n1 + (n2 - n1) * ((170 - hue) / 42.5);
    } else {
        value = n1;
    }

    return (int)(value * 255);
}

/*!
 * Convert a color in HLS space to RGB space.
 * \param hue the hue component (modified in place).
 * \param lightness the lightness component (modified in place).
 * \param saturation the saturation component (modified in place).
 */
static void HLSTORGB(uchar &hue, uchar &lightness, uchar &saturation)
{
    double h = hue;
    double l = lightness;
    double s = saturation;

    if (s == 0) {
        hue        = (uchar)l;
        lightness  = (uchar)l;
        saturation = (uchar)l;
    } else {
        double m1, m2;

        if (l < 128) {
            m2 = (l * (255 + s)) / 65025.;
        } else {
            m2 = (l + s - (l * s) / 255.) / 255.;
        }

        m1 = (l / 127.5) - m2;

        hue        = HLSVALUE(m1, m2, h + 85);
        lightness  = HLSVALUE(m1, m2, h);
        saturation = HLSVALUE(m1, m2, h - 85);
    }
}
#endif
