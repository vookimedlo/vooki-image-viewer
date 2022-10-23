# VookiImageViewer

![alt text](https://img.shields.io/static/v1?label=Platforms&message=linux|macos|windows&color=blueviolet "Supported Platforms")


[![Linux Builds](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/linux-build.yml/badge.svg)](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/linux-build.yml)
[![Windows Builds](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/win-build.yml/badge.svg)](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/win-build.yml)
[![MacOS Builds](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/macos-build.yml/badge.svg)](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/macos-build.yml)

A lightweight image viewer for a fast image preview. It has been developed to have the same viewer available for all major operating systems - Microsoft Windows 11, macOS and GNU/Linux.

The main goal is to have a free of charge cross-platform viewer with a simple design and minimum functions that are commonly used.

- Transformations
  - Flip _(horizontal, vertical)._
  - Rotation _(clockwise and counterclockwise in 90° steps)._
  - Zoom _(in, out, original size, fit to window)._
- Fullscreen or Window mode.
- Custom background color.
- Custom image border color.
- Remembers recent files.
- Supports Apple's high definition trackpad gestures.

![Application screenshot](src/resource/readme/screenshot_2022-10-07_21-59-01.png?raw=true "")

Supported image formats cover the very common formats as well as the rare ones. The viewer does not intentionally [demosaic][1] the RAW images, but only displays the embedded thumbnail.


| FORMAT                    | DESCRIPTION                                      |
|---------------------------|--------------------------------------------------|
| ANI                       | Windows Animated Cursor                          |
| GIF                       | Graphic Interchange Format                       |
| JPG                       | Joint Photographic Experts Group                 |
| PNG                       | Portable Network Graphics                        |
| PBM                       | Portable Bitmap                                  |
| PGM                       | Portable Graymap                                 |
| PPM                       | Portable Pixmap                                  |
| XBM                       | X11 Bitmap                                       |
| XPM                       | X11 Pixmap                                       |
| SVG                       | Scalable Vector Graphics                         |
| BMP                       | Windows Bitmap                                   |
| XCF                       | Gimp                                             |
| PSD                       | Photoshop Documents                              |
| RAS                       | Sun Raster                                       |
| PCX                       | Personal Computer Exchange                       |
| RGB, RGBA, SGI, BW        | SGI Images                                       |
| PIC                       | Softimage                                        |
| TGA                       | Targa                                            |
| TIFF                      | Tagged Image File Format                         |
| ARW                       | Sony Alpha Raw [Inner thumbnail only]            |
| CR2                       | Canon Raw Version 2 [Inner thumbnail only]       |
| DNG                       | Digital Negative [Inner thumbnail only]          |
| ERF                       | Epson RAW File [Inner thumbnail only]            |
| MOS                       | Leaf Raw Image File [Inner thumbnail only]       |
| MRW                       | Konica Minolta RAW [Inner thumbnail only]        |
| NEF                       | Nikon Raw Image File [Inner thumbnail only]      |
| ORF                       | Olympus Raw Image File [Inner thumbnail only]    |
| PEF                       | Pentax Raw Image File [Inner thumbnail only]     |
| RAF                       | Fuji Raw Image File [Inner thumbnail only]       |
| RW2                       | Panasonic RAW Image File [Inner thumbnail only]  |
| SRW                       | Samsung RAW Image File [Inner thumbnail only]    |
| X3F                       | SIGMA X3F Camera RAW File [Inner thumbnail only] |
| [MacOS/Windows only] HEIC | High-Efficiency Image File Format                |


All operations have assigned shortcuts and all of them can be user re-assigned except the one which is used for Preferences on macOS.


| MacOS            | Windows                     | GNU/Linux                   | OPERATION                                |
|------------------|-----------------------------|-----------------------------|------------------------------------------|
| ﻿<kbd>﻿⌥c</kbd>  | <kbd>ALT</kbd>+<kbd>c</kbd> | <kbd>ALT</kbd>+<kbd>c</kbd> | Clear recent file                        |
| ﻿<kbd>﻿q</kbd>   | <kbd>q</kbd>                | <kbd>q</kbd>                | Quit application                         |
| ﻿<kbd>﻿﻿⌘,</kbd> | <kbd>p</kbd>                | <kbd>p</kbd>                | Preferences                              |
| ﻿<kbd>﻿f</kbd>   | <kbd>f</kbd>                | <kbd>f</kbd>                | Show image in fullscreen mode            |
| ﻿<kbd>﻿,</kbd>   | <kbd>,</kbd>                | <kbd>,</kbd>                | Rotate left                              |
| ﻿<kbd>﻿.</kbd>   | <kbd>.</kbd>                | <kbd>.</kbd>                | Rotate right                             |
| ﻿<kbd>﻿h</kbd>   | <kbd>h</kbd>                | <kbd>h</kbd>                | Flip horizontally                        |
| ﻿<kbd>﻿v</kbd>   | <kbd>v</kbd>                | <kbd>v</kbd>                | Flip vertically                          |
| ﻿<kbd>﻿+</kbd>   | <kbd>+</kbd>                | <kbd>+</kbd>                | Zoom in                                  |
| ﻿<kbd>﻿-</kbd>   | <kbd>-</kbd>                | <kbd>-</kbd>                | Zoom out                                 |
| ﻿<kbd>﻿*</kbd>   | <kbd>*</kbd>                | <kbd>*</kbd>                | Zoom reset                               |
| ﻿<kbd>﻿w</kbd>   | <kbd>w</kbd>                | <kbd>w</kbd>                | Fit to window                            |
| ﻿<kbd>﻿[</kbd>   | <kbd>[</kbd>                | <kbd>[</kbd>                | Previous image                           |
| ﻿<kbd>﻿]</kbd>   | <kbd>]</kbd>                | <kbd>]</kbd>                | Next image                               |
| ﻿<kbd>﻿i</kbd>   | <kbd>i</kbd>                | <kbd>i</kbd>                | Scroll up                                |
| ﻿<kbd>﻿k</kbd>   | <kbd>k</kbd>                | <kbd>k</kbd>                | Scroll down                              |
| ﻿<kbd>﻿j</kbd>   | <kbd>j</kbd>                | <kbd>j</kbd>                | Scroll left                              |
| ﻿<kbd>﻿l</kbd>   | <kbd>l</kbd>                | <kbd>l</kbd>                | Scroll right                             |
| ﻿<kbd>﻿s</kbd>   | <kbd>s</kbd>                | <kbd>s</kbd>                | Toggle statusbar                         |
| ﻿<kbd>﻿n</kbd>   | <kbd>n</kbd>                | <kbd>n</kbd>                | Toggle filesystem navigation             |
| ﻿<kbd>﻿t</kbd>   | <kbd>t</kbd>                | <kbd>t</kbd>                | Toggle toolbar                           |
| ﻿<kbd>﻿F1</kbd>  | <kbd>F1</kbd>               | <kbd>F1</kbd>               | About application                        |
| ﻿none            | none                        | none                        | About components                         |
| ﻿none            | none                        | none                        | About Qt                                 |
| ﻿none            | none                        | none                        | About supported formats                  |

All code developed during this project is [GPLv3][2] licensed. Images as well as the 3rd-party components have their own licenses, see proper LICENSE files in a [GitHub Source Tree][3].

-----------------

## Releases
[Prebuilt binaries][5] for Windows 11, Debian, Fedora, Ubuntu, and macOS, as well as sources, are available [here.][5]

Mac users can install the VookiImageViewer by the Homebrew.
```
brew tap vookimedlo/homebrew-vookiimageviewer
brew install --cask vookiimageviewer-macos
```

Debian Bullseye users can install the VookiImageViewer from the APT repository hosted by the [cloudsmith.io][6] for free.
```
apt-get install -y debian-keyring
apt-get install -y debian-archive-keyring
apt-get install -y apt-transport-https
keyring_location=/usr/share/keyrings/michal-duda-vookiimageviewer-archive-keyring.gpg
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/gpg.EF5E62B51DE78AFF.key' |  gpg --dearmor > ${keyring_location}
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/config.deb.txt?distro=debian&codename=bullseye' > /etc/apt/sources.list.d/michal-duda-vookiimageviewer.list
apt-get update
apt-get install vookiimageviewer
```

Debian Bookworm users can install the VookiImageViewer from the APT repository hosted by the [cloudsmith.io][6] for free.
```
apt-get install -y debian-keyring
apt-get install -y debian-archive-keyring
apt-get install -y apt-transport-https
keyring_location=/usr/share/keyrings/michal-duda-vookiimageviewer-archive-keyring.gpg
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/gpg.EF5E62B51DE78AFF.key' |  gpg --dearmor > ${keyring_location}
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/config.deb.txt?distro=debian&codename=bookworm' > /etc/apt/sources.list.d/michal-duda-vookiimageviewer.list
apt-get update
apt-get install vookiimageviewer
```

Ubuntu Jammy users can install the VookiImageViewer from the APT repository hosted by the [cloudsmith.io][6] for free.
```
apt-get install -y apt-transport-https
keyring_location=/usr/share/keyrings/michal-duda-vookiimageviewer-archive-keyring.gpg
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/gpg.EF5E62B51DE78AFF.key' |  gpg --dearmor > ${keyring_location}
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/config.deb.txt?distro=ubuntu&codename=jammy' > /etc/apt/sources.list.d/michal-duda-vookiimageviewer.list
apt-get update
apt-get install vookiimageviewer
```

Ubuntu Kinetic users can install the VookiImageViewer from the APT repository hosted by the [cloudsmith.io][6] for free.
```
apt-get install -y apt-transport-https
keyring_location=/usr/share/keyrings/michal-duda-vookiimageviewer-archive-keyring.gpg
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/gpg.EF5E62B51DE78AFF.key' |  gpg --dearmor > ${keyring_location}
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/config.deb.txt?distro=ubuntu&codename=kinetic' > /etc/apt/sources.list.d/michal-duda-vookiimageviewer.list
apt-get update
apt-get install vookiimageviewer
```

Fedora 36 users can install the VookiImageViewer from the repository hosted by the [cloudsmith.io][6] for free.
```
dnf install yum-utils pygpgme
rpm --import 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/gpg.EF5E62B51DE78AFF.key'
curl -1sLf 'https://dl.cloudsmith.io/public/michal-duda/vookiimageviewer/config.rpm.txt?distro=fedora&codename=36' > /tmp/michal-duda-vookiimageviewer.repo
dnf config-manager --add-repo '/tmp/michal-duda-vookiimageviewer.repo'
dnf -q makecache -y --disablerepo='*' --enablerepo='michal-duda-vookiimageviewer' --enablerepo='michal-duda-vookiimageviewer-source'
dnf install vookiimageviewer
```

-----------------

## Builds
All binaries could be built easily. For your convenience, GitHub's Actions are used to continuously check if the current source code is buildable on Linux and macOS.

[![Linux Builds](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/linux-build.yml/badge.svg)](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/linux-build.yml)
[![Windows Builds](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/win-build.yml/badge.svg)](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/win-build.yml)
[![MacOS Builds](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/macos-build.yml/badge.svg)](https://github.com/vookimedlo/vooki-image-viewer/actions/workflows/macos-build.yml)
 
-----------------

Homepage: [https://vookiimageviewer.cz/][4]

[1]: https://en.wikipedia.org/wiki/Demosaicing
[2]: ./LICENSE
[3]: https://github.com/vookimedlo/vooki-image-viewer/
[4]: https://vookiimageviewer.cz/
[5]: https://github.com/vookimedlo/vooki-image-viewer/releases/latest
[6]: https://cloudsmith.io/
