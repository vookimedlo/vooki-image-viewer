# VookiImageViewer
Lightweight image viewer for a fast image preview. It has been developed to have the same viewer available for all major operating systems - Microsoft Windows 10, MacOS and GNU/Linux.

The main goal is to have free of charge cross-platform viewer with a simple design and minimum functions which are commonly used.

- Transformations
  - Rotation _(clockwise and counterclockwise in 90° steps)._
  - Flip _(horizontal, vertical)._
  - Zoom _(in, out, original size, fit to window)._
- Fullscreen or Window mode.
- Remembers recent files.
- Supports Apple high definition trackpad gestures.
- Custom background color
- Custom image border color

Supported image formats covers the very common formats as well as the rare ones. Viewer is not intentionally [demosaicing][1] the RAW images, but only displays the embedded thumbnail.

<center>
| FORMAT             | DESCRIPTION                                      |
|--------------------|--------------------------------------------------|
| GIF                | Graphic Interchange Format                       |
| JPG                | Joint Photographic Experts Group                 |
| PNG                | Portable Network Graphics                        |
| PBM                | Portable Bitmap                                  |
| PGM                | Portable Graymap                                 |
| PPM                | Portable Pixmap                                  |
| XBM                | X11 Bitmap                                       |
| XPM                | X11 Pixmap                                       |
| SVG                | Scalable Vector Graphics                         |
| BMP                | Windows Bitmap                                   |
| XCF                | Gimp                                             |
| PSD                | Photoshop Documents                              |
| RAS                | Sun Raster                                       |
| PCX                | Personal Computer Exchange                       |
| RGB, RGBA, SGI, BW | SGI Images                                       |
| PIC                | Softimage                                        |
| TGA                | Targa                                            |
| ARW                | Sony Alpha Raw [Inner thumbnail only]            |
| CR2                | Canon Raw Version 2 [Inner thumbnail only]       |
| DNG                | Digital Negative [Inner thumbnail only]          |
| ERF                | Epson RAW File [Inner thumbnail only]            |
| MOS                | Leaf Raw Image File [Inner thumbnail only]       |
| MRW                | Konica Minolta RAW [Inner thumbnail only]        |
| NEF                | Nikon Raw Image File [Inner thumbnail only]      |
| ORF                | Olympus Raw Image File [Inner thumbnail only]    |
| PEF                | Pentax Raw Image File [Inner thumbnail only]     |
| RAF                | Fuji Raw Image File [Inner thumbnail only]       |
| RW2                | Panasonic RAW Image File [Inner thumbnail only]  |
| SRW                | Samsung RAW Image File [Inner thumbnail only]    |
| X3F                | SIGMA X3F Camera RAW File [Inner thumbnail only] |
</center>

All operations have assigned shortcuts and all of them can be user re-assigned except the one which is used for Preferences on MacOS.



[1]: https://en.wikipedia.org/wiki/Demosaicing
