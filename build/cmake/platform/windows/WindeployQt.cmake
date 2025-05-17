#############################################################################
# VookiImageViewer - a tool for showing images.
# - https://github.com/vookimedlo/vooki-image-viewer
#
# SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileType: SOURCE
#############################################################################

function(deployqt)
    GET_TARGET_PROPERTY(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    GET_FILENAME_COMPONENT(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    FIND_PROGRAM(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}" REQUIRED)

    SET(WINDEPLOYQT_EXECUTABLE "${DEPLOYQT_EXECUTABLE}" PARENT_SCOPE)

    SET(DEPLOYQT_QTPATH_LOCATION "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}/qtpaths.bat")
    IF (EXISTS "${DEPLOYQT_QTPATH_LOCATION}")
        SET(DEPLOYQT_QTPATH_OPTION "--qtpaths ${DEPLOYQT_QTPATH_LOCATION}" PARENT_SCOPE)
    ELSE()
        SET(DEPLOYQT_QTPATH_OPTION "" PARENT_SCOPE)
    ENDIF()

    ADD_CUSTOM_COMMAND(TARGET ${APPLICATION_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${_qt_bin_dir}" "${DEPLOYQT_EXECUTABLE}" ${DEPLOYQT_QTPATH_OPTION} --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-opengl-sw "$<TARGET_FILE:${APPLICATION_NAME}>"
            COMMENT "Deploying QT runtime ...")
endfunction()
