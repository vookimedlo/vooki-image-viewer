INCLUDE(GNUInstallDirs)
INCLUDE(CPack.cmake)

SET(PLUGINS "${LIB_INSTALL_DIR}/vookiimageviewer/imageformats")

INSTALL(TARGETS ${APPLICATION_NAME} ${IMAGE_PLUGINS}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${PLUGINS})

GET_FILENAME_COMPONENT(TOP_LEVEL_ABSOLUTE_PATH ../../ ABSOLUTE)
INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE
        ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-IMAGES
        ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-OPENCLIPART
        ${TOP_LEVEL_ABSOLUTE_PATH}/LICENSE-PLUGINS-KIMAGEFORMATS
        ${TOP_LEVEL_ABSOLUTE_PATH}/README.md
        DESTINATION share/doc/vookiimageviewer)

INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/build/cmake/platform/unix/support/vookiimageviewer.desktop
        DESTINATION share/applications)

INSTALL(FILES ${TOP_LEVEL_ABSOLUTE_PATH}/src/resource/openclipart/vookiimageviewericon.png
        DESTINATION share/pixmaps)

FILE(STRINGS /etc/os-release DISTRIBUTION REGEX "^ID=")
STRING(REGEX REPLACE "ID=\"?(.+)\"?" "\\1" DISTRIBUTION "${DISTRIBUTION}")
MESSAGE("-- Distribution ${DISTRIBUTION} found")

if(NOT CPACK_GENERATOR)
    if (DISTRIBUTION MATCHES "^(debian|ubuntu)$")
        SET(CPACK_GENERATOR DEB)
    elseif(${DISTRIBUTION} MATCHES "^(fedora|redhat)$")
        SET(CPACK_GENERATOR RPM)
    else()
        SET(CPACK_GENERATOR ZIP)
    endif()
endif()

foreach(GENERATOR IN LISTS CPACK_GENERATOR)
    MESSAGE("-- ${GENERATOR} packaging requested")

    if(GENERATOR MATCHES "DEB")
        SET(CPACK_VERBATIM_VARIABLES YES)

        SET(CPACK_PACKAGE_CONTACT "github@vookimedlo.cz")
        SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Michal Duda")
        SET(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
        SET(CPACK_DEBIAN_PACKAGE_SECTION "contrib")
        if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.22.0")
            SET(CPACK_DEBIAN_COMPRESSION_TYPE "zstd")
        elseif()
            SET(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
        endif()
        SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        SET(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
        SET(CPACK_DEBIAN_PACKAGE_RECOMMENDS "qt6-image-formats-plugins")
        SET(CPACK_DEBIAN_DEBUGINFO_PACKAGE OFF)

        STRING(TOLOWER ${APPLICATION_NAME} APPLICATION_NAME_LOWER)
        CONFIGURE_FILE(${TOP_LEVEL_ABSOLUTE_PATH}/build/cmake/platform/unix/support/package/deb/create-deb-changelog.sh.in "${CMAKE_BINARY_DIR}/create-deb-changelog.sh" @ONLY)

        ADD_CUSTOM_COMMAND(
                OUTPUT "${CMAKE_BINARY_DIR}/changelog.gz"
                COMMAND /bin/sh create-deb-changelog.sh
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                DEPENDS "${CMAKE_BINARY_DIR}/create-deb-changelog.sh"
                COMMENT "Creating changelog")

        ADD_CUSTOM_TARGET(make_changelog ALL DEPENDS "${CMAKE_BINARY_DIR}/changelog.gz")

        INSTALL(FILES "${CMAKE_BINARY_DIR}/changelog.gz"
                DESTINATION "share/doc/vookiimageviewer")

    elseif(GENERATOR MATCHES "RPM")
        SET(CPACK_VERBATIM_VARIABLES YES)

        SET(CPACK_RPM_FILE_NAME RPM-DEFAULT)
        SET(CPACK_RPM_PACKAGE_RELEASE_DIST ON)
        SET(CPACK_RPM_PACKAGE_LICENSE "GPLv3+")
        SET(CPACK_RPM_PACKAGE_GROUP Applications/Multimedia)
        SET(CPACK_RPM_PACKAGE_SUMMARY "Cross-platform lightweight image viewer for a fast image preview.")
        SET(CPACK_RPM_PACKAGE_DESCRIPTION "Lightweight image viewer for a fast image preview. It has been developed to have
 the same viewer available for all major operating systems - Windows 11, MacOS and
 GNU/Linux.

 The main goal is to have a free of charge cross-platform viewer with a simple design
 and minimum functions which are commonly used.")
        SET(CPACK_RPM_PACKAGE_AUTOREQ ON)
        SET(CPACK_RPM_PACKAGE_RELOCATABLE ON)
        SET(CPACK_RPM_PACKAGE_REQUIRES_POST "qt6-qtimageformats")
        SET(CPACK_RPM_SPEC_MORE_DEFINE "%global __provides_exclude_from /%{name}/imageformats/.*\\\\.so.*$")

        SET(CPACK_RPM_BUILDREQUIRES "LibRaw-devel, cmake, git, make, qt6-qtbase, qt6-qtbase-devel, desktop-file-utils")
        SET(CPACK_RPM_PACKAGE_SOURCES OFF)

        CONFIGURE_FILE(${TOP_LEVEL_ABSOLUTE_PATH}/build/cmake/platform/unix/support/package/rpm/changelog/git2changelog/scripts/git2changelog "${CMAKE_BINARY_DIR}/" COPYONLY)
        CONFIGURE_FILE(${TOP_LEVEL_ABSOLUTE_PATH}/build/cmake/platform/unix/support/package/rpm/changelog/git2changelog/src/git2changelog.py "${CMAKE_BINARY_DIR}/" COPYONLY)
        CONFIGURE_FILE(${TOP_LEVEL_ABSOLUTE_PATH}/build/cmake/platform/unix/support/package/rpm/create-rpm-changelog.sh.in "${CMAKE_BINARY_DIR}/create-rpm-changelog.sh" @ONLY)

        ADD_CUSTOM_COMMAND(
                OUTPUT "${CMAKE_BINARY_DIR}/changelog"
                COMMAND /bin/sh create-rpm-changelog.sh
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
                DEPENDS "${CMAKE_BINARY_DIR}/create-rpm-changelog.sh"
                COMMENT "Creating changelog")

        ADD_CUSTOM_TARGET(make_changelog ALL DEPENDS "${CMAKE_BINARY_DIR}/changelog")

        INSTALL(FILES "${CMAKE_BINARY_DIR}/changelog"
                DESTINATION "share/doc/vookiimageviewer")

        SET(CPACK_RPM_CHANGELOG_FILE "${CMAKE_BINARY_DIR}/changelog")
    endif()

endforeach()

include(CPack)
