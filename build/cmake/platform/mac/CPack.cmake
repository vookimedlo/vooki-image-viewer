#############################################################################
# VookiImageViewer - a tool for showing images.
# - https://github.com/vookimedlo/vooki-image-viewer
#
# SPDX-FileCopyrightText: 2023 Michal Duda <github@vookimedlo.cz>
# SPDX-License-Identifier: GPL-3.0-or-later
# SPDX-FileType: SOURCE
#############################################################################

# Plugins
###

SET(PLUGINS "$<TARGET_FILE_DIR:${APPLICATION_NAME}>/../PlugIns/imageformats")

INSTALL(TARGETS ${IMAGE_PLUGINS}
        LIBRARY DESTINATION ${PLUGINS})

# Localization
###

SET(LOCALIZATION "$<TARGET_FILE_DIR:${APPLICATION_NAME}>/../Resources/")

LIST(TRANSFORM TRANSLATION_LANGUAGES PREPEND ${CMAKE_BINARY_DIR}/)
LIST(TRANSFORM TRANSLATION_LANGUAGES APPEND  ".lproj")
FILE(MAKE_DIRECTORY ${TRANSLATION_LANGUAGES})

INSTALL(DIRECTORY ${TRANSLATION_LANGUAGES} DESTINATION ${LOCALIZATION})
