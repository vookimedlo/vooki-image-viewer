function(deployqt)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET ${APPLICATION_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
            env PATH="${_qt_bin_dir}" "${DEPLOYQT_EXECUTABLE}" --no-quick-import --no-system-d3d-compiler --compiler-runtime --no-opengl-sw "$<TARGET_FILE:${APPLICATION_NAME}>"
            COMMENT "Deploying QT runtime ...")
endfunction()
