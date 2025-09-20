#############################################################################
# VookiImageViewer - a tool for showing images.
# - https://github.com/vookimedlo/vooki-image-viewer
#
# SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileType: SOURCE
#############################################################################


CMAKE_MINIMUM_REQUIRED(VERSION 3.18.4)
SET(CMAKE_OSX_DEPLOYMENT_TARGET "14.0" CACHE STRING "Minimum OS X version to target for deployment (at runtime); newer APIs weak linked.")
SET(ENV{MACOSX_DEPLOYMENT_TARGET} ${CMAKE_OSX_DEPLOYMENT_TARGET})

PROJECT(ApplicationAndDependencies HOMEPAGE_URL "https://vookiimageviewer.cz")

MESSAGE("-- CMake version: ${CMAKE_CACHE_MAJOR_VERSION}.${CMAKE_CACHE_MINOR_VERSION}.${CMAKE_CACHE_PATCH_VERSION}")

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

message("-- System processor: ${CMAKE_SYSTEM_PROCESSOR}")

include(CheckCXXSourceCompiles)
# Test for ARM architecture
check_cxx_source_compiles("
#if defined(__aarch64__) || defined(_M_ARM64)
int main() { return 0; }
#else
#error Not ARM64 architecture
#endif
" IS_ARM64)



if(IS_ARM64)
    message("-- Target architecture: ARM64 detected")
else ()
    # Test for x86 architecture
    check_cxx_source_compiles("
#if (defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64))
int main() { return 0; }
#else
#error Not x64 architecture
#endif
" IS_X64)
    if(IS_X64)
        message("-- Target architecture: x64 detected")
    else()
        message("-- Target architecture: Unknown - probably: ${CMAKE_SYSTEM_PROCESSOR}")
    endif()
endif ()

MESSAGE("-- 3RD_PARTY_LIBS: ${3RD_PARTY_LIBS}")

if(NOT DEFINED BUILD_DEPENDENCIES)
  set(BUILD_DEPS_AND_APP ON)
else()
  set(BUILD_DEPS_AND_APP OFF)
endif()

if(BUILD_DEPS_AND_APP OR (DEFINED BUILD_DEPENDENCIES AND BUILD_DEPENDENCIES))
    MESSAGE("-- Building required dependencies ...")
    if (APPLE)
        ADD_SUBDIRECTORY(build/cmake/platform/mac/ExternalProjects)
    elseif (WIN32)
        ADD_SUBDIRECTORY(build/cmake/platform/windows/ExternalProjects)
    endif()
endif()

if(BUILD_DEPS_AND_APP OR (DEFINED BUILD_DEPENDENCIES AND NOT BUILD_DEPENDENCIES))
    MESSAGE("-- Building application ...")
    ADD_SUBDIRECTORY(build/cmake)
endif()
