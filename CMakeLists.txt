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
SET(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum OS X version to target for deployment (at runtime); newer APIs weak linked.")
SET(ENV{MACOSX_DEPLOYMENT_TARGET} ${CMAKE_OSX_DEPLOYMENT_TARGET})

PROJECT(ApplicationAndDependencies HOMEPAGE_URL "https://vookiimageviewer.cz")

SET(3RD_PARTY_LIBS ${CMAKE_BINARY_DIR}/EPs)
SET(3RD_PARTY_LIBS_INSTALL "${3RD_PARTY_LIBS}/install")

GET_FILENAME_COMPONENT(3RD_PARTY_LIBS_INSTALL_ABSOLUTE "${3RD_PARTY_LIBS_INSTALL}" ABSOLUTE)
LIST(PREPEND CMAKE_PREFIX_PATH "${3RD_PARTY_LIBS_INSTALL}/lib/cmake")
LIST(PREPEND CMAKE_PREFIX_PATH "${3RD_PARTY_LIBS_INSTALL}")

if (APPLE)
    option(APPLE_FAT_ARCH "Requested a fat architecture (x86_64 & arm64)" ON)

    if (APPLE_FAT_ARCH)
        SET(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
        MESSAGE("-- Requested architectures are x86_64 & arm64")
    endif()
endif()

include(CheckCXXSourceCompiles)
# Test for ARM architecture
check_cxx_source_compiles("
#if defined(__aarch64__) || defined(_M_ARM64)
int main() { return 0; }
#else
#error Not ARM64 architecture
#endif
" IS_ARM64)

# Test for x86 architecture
check_cxx_source_compiles("
#if (defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64))
int main() { return 0; }
#else
#error Not x64 architecture
#endif
" IS_X64)


if(IS_ARM64)
    message("ARM64 architecture detected")
elseif(IS_X64)
    message("x64 architecture detected")
else()
    message("Unknown architecture - probably: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

MESSAGE("-- 3RD_PARTY_LIBS: ${3RD_PARTY_LIBS}")

if (BUILD_DEPENDENCIES)
    MESSAGE("-- Building required dependencies ...")
    if (APPLE)
        ADD_SUBDIRECTORY(build/cmake/platform/mac/ExternalProjects)
    elseif (WIN32)
        ADD_SUBDIRECTORY(build/cmake/platform/windows/ExternalProjects)
    endif()
else()
    MESSAGE("-- Building application ...")
    ADD_SUBDIRECTORY(build/cmake)
endif()
