SET(CPACK_PACKAGE_NAME ${APPLICATION_NAME})
SET(CPACK_PACKAGE_VENDOR "Michal Duda")
SET(CPACK_PACKAGE_VERSION "${VERSION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY ${APPLICATION_NAME})
SET(CPACK_PACKAGE_EXECUTABLES "${APPLICATION_NAME}" "${APPLICATION_NAME}")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform lightweight image viewer for a fast image preview.
 Lightweight image viewer for a fast image preview. It has been developed to have
 the same viewer available for all major operating systems - Windows 10/11, MacOS and
 GNU/Linux.
 .
 The main goal is to have a free of charge cross-platform viewer with a simple design
 and minimum functions which are commonly used.")
SET(CPACK_CREATE_DESKTOP_LINKS "${APPLICATION_NAME}")
SET(CPACK_STRIP_FILES TRUE)
SET(CPACK_MONOLITHIC_INSTALL TRUE)
