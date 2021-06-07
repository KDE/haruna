#
# SPDX-FileCopyrightText: 2020 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

#add_subdirectory(color-schemes)

install(FILES org.kde.haruna.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.haruna.metainfo.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
ecm_install_icons(ICONS 256-apps-org.kde.haruna.svg DESTINATION ${KDE_INSTALL_ICONDIR})
