#############################################################################
# VookiImageViewer - a tool for showing images.
# - https://github.com/vookimedlo/vooki-image-viewer
#
# SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileType: SOURCE
#############################################################################

INSTALL(TARGETS ${APPLICATION_NAME}
        RUNTIME DESTINATION ".")
INSTALL(TARGETS ${IMAGE_PLUGINS}
        LIBRARY DESTINATION "imageformats")
INSTALL(FILES ${LIBDE265} ${LIBHEIF}
        DESTINATION ".")
INSTALL(FILES platform/windows/support/package/gpl-3.0.rtf
        DESTINATION ".")

if(INSTALL_SYSTEM_RUNTIME)
    MESSAGE("-- Runtime installation requested")
    SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION ".")
    INCLUDE(InstallRequiredSystemLibraries)
    SET(CMAKE_INSTALL_UCRT_LIBRARIES FALSE) # Windows >= 10 already contains these
endif()

FIND_PROGRAM(WINDEPLOYQT windeployqt HINTS "${_qt_bin_dir}")
CONFIGURE_FILE("platform/Windows/WindeployQt-CPack.cmake.in" "${CMAKE_BINARY_DIR}/WindeployQt-CPack.cmake" @ONLY)

SET(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_BINARY_DIR}/WindeployQt-CPack.cmake)

INCLUDE(CPack.cmake)

if(NOT CPACK_GENERATOR)
    SET(CPACK_GENERATOR 7Z ZIP WIX)
endif()

foreach(GENERATOR IN LISTS CPACK_GENERATOR)
    MESSAGE("-- ${GENERATOR} packaging requested")

    if(GENERATOR MATCHES "WIX")
        GET_FILENAME_COMPONENT(PACKAGE_SUPPORT_DIR "platform/Windows/support/package/" ABSOLUTE)
        GET_FILENAME_COMPONENT(PACKAGE_PRODUCT_ICON "../../src/resource/openclipart/vookiimageviewericon.ico" ABSOLUTE)

        SET(CPACK_WIX_VERSION 4)
        SET(CPACK_WIX_UPGRADE_GUID "91FA8A80-C971-408A-A735-4EB3D230D9F6")
        SET(CPACK_WIX_LICENSE_RTF "${PACKAGE_SUPPORT_DIR}/gpl-3.0.rtf")
        SET(CPACK_WIX_UI_BANNER "${PACKAGE_SUPPORT_DIR}/wix-vookiimageviewer-banner.png")
        SET(CPACK_WIX_UI_DIALOG "${PACKAGE_SUPPORT_DIR}/wix-vookiimageviewer-dialog.png")
        SET(CPACK_WIX_PRODUCT_ICON "${PACKAGE_PRODUCT_ICON}")
        SET(CPACK_WIX_PROGRAM_MENU_FOLDER .)
        SET(CPACK_WIX_ARCHITECTURE "x64")
        SET(CPACK_WIX_PROPERTY_ARPURLINFOABOUT "https://vookiimageviewer.cz/")
        SET(CPACK_WIX_PROPERTY_ARPHELPLINK "https://vookiimageviewer.cz/")
        SET(CPACK_WIX_PATCH_FILE "${PACKAGE_SUPPORT_DIR}/cpack-wix-patches/registry.xml")

        STRING(REGEX REPLACE "^[0-9][0-9]([0-9][0-9]\\.[0-9][0-9]\\.[0-9][0-9]).*" "\\1" VERSION_FOR_WIX "${VERSION}")
        SET(WIX_CUSTOM_XMLNS_EXPANDED "@CPACK_WIX_CUSTOM_XMLNS_EXPANDED@")
        CONFIGURE_FILE("${PACKAGE_SUPPORT_DIR}/WIX.template.in" "${CMAKE_BINARY_DIR}/WIX.template.in" @ONLY)
        SET(CPACK_WIX_TEMPLATE "${CMAKE_BINARY_DIR}//WIX.template.in")
    endif()

endforeach()

include(CPack)
