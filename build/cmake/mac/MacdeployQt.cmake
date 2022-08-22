function(deployqt)
    get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(DEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")

    add_custom_command(TARGET ${APPLICATION_NAME} POST_BUILD
            COMMAND "${DEPLOYQT_EXECUTABLE}"
            "$<TARGET_FILE_DIR:${APPLICATION_NAME}>/../.."
            -always-overwrite
            -appstore-compliant
            COMMENT "Deploying QT runtime ...")
endfunction()
