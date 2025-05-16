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

    ADD_CUSTOM_COMMAND(TARGET ${APPLICATION_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${_qt_bin_dir}/${host_prefix}" "${DEPLOYQT_EXECUTABLE}" --qtpaths "${QT6_INSTALL_PREFIX}/${QT6_INSTALL_BINS}/qtpaths.bat" --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-opengl-sw "$<TARGET_FILE:${APPLICATION_NAME}>"
            COMMENT "Deploying QT runtime ...")
endfunction()
