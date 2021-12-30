#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

set(ICONS_FOLDER ${PROJECT_SOURCE_DIR}/data/icons)
set(ICON_FILES
    ${ICONS_FOLDER}/sc-apps-haruna.svg
    ${ICONS_FOLDER}/16-apps-haruna.png
    ${ICONS_FOLDER}/22-apps-haruna.png
    ${ICONS_FOLDER}/32-apps-haruna.png
    ${ICONS_FOLDER}/44-apps-haruna.png
    ${ICONS_FOLDER}/48-apps-haruna.png
    ${ICONS_FOLDER}/64-apps-haruna.png
    ${ICONS_FOLDER}/128-apps-haruna.png
    ${ICONS_FOLDER}/150-apps-haruna.png
    ${ICONS_FOLDER}/256-apps-haruna.png
    ${ICONS_FOLDER}/310-apps-haruna.png
    ${ICONS_FOLDER}/512-apps-haruna.png
)

# used in src/CMakeLists.txt
set(DATA_ICONS ${ICON_FILES} PARENT_SCOPE)

ecm_install_icons(
    ICONS ${ICON_FILES}
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)

install(FILES org.kde.haruna.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.haruna.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
