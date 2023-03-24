# VookiImageViewer - a tool for showing images.
# Copyright(C) 2023 Michal Duda <github@vookimedlo.cz>
#
# https://github.com/vookimedlo/vooki-image-viewer
#
# This program is free software : you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 3.18.4)

project(ApplicationAndDependencies HOMEPAGE_URL "https://vookiimageviewer.cz")

SET(3RD_PARTY_LIBS ${CMAKE_BINARY_DIR}/3rd-party)
SET(3RD_PARTY_LIBS_INSTALL "${3RD_PARTY_LIBS}/install")

GET_FILENAME_COMPONENT(3RD_PARTY_LIBS_INSTALL_ABSOLUTE "${3RD_PARTY_LIBS_INSTALL}" ABSOLUTE)
LIST(PREPEND CMAKE_PREFIX_PATH "${3RD_PARTY_LIBS_INSTALL}/lib/cmake")
LIST(PREPEND CMAKE_PREFIX_PATH "${3RD_PARTY_LIBS_INSTALL}")

MESSAGE("-- 3RD_PARTY_LIBS: ${3RD_PARTY_LIBS}")

if (BUILD_DEPENDENCIES)
    MESSAGE("Building required dependencies ...")
    if (APPLE)
        add_subdirectory(build/cmake/platform/mac/3rdPartyLibs)
    elseif (WIN32)
        add_subdirectory(build/cmake/platform/windows/3rdPartyLibs)
    endif()
else()
    MESSAGE("Building application ...")
    add_subdirectory(build/cmake)
endif()
