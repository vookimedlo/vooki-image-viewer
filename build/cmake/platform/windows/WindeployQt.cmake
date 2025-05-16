#############################################################################
# VookiImageViewer - a tool for showing images.
# - https://github.com/vookimedlo/vooki-image-viewer
#
# SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileType: SOURCE
#############################################################################

function(deployqt)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if (DEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${APPLICATION_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E
                env PATH="${_qt_bin_dir}" "${DEPLOYQT_EXECUTABLE}" --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-opengl-sw "$<TARGET_FILE:${APPLICATION_NAME}>"
                COMMENT "Deploying QT runtime ...")
    else ()
        FILE(STRINGS "${_qt_bin_dir}/target_qt.conf" host_prefix_pair REGEX '^HostPrefix=')
        STRING(REGEX REPLACE '^HostPrefix=' '' host_prefix host_prefix_pair)
        find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}/${host_prefix}" REQUIRED)
        add_custom_command(TARGET ${APPLICATION_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E
                env PATH="${_qt_bin_dir}/${host_prefix}" "${DEPLOYQT_EXECUTABLE}" --qtpaths "${_qt_bin_dir}/qtpaths.bat" --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-opengl-sw "$<TARGET_FILE:${APPLICATION_NAME}>"
                COMMENT "Deploying QT runtime ...")
    endif ()
endfunction()
