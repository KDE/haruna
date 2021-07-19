#
# SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if (KF5DocTools_FOUND)
    KDOCTOOLS_CREATE_HANDBOOK(
        index.docbook
            INSTALL_DESTINATION ${KDE_INSTALL_DOCBUNDLEDIR}/en
            SUBDIR haruna
    )
endif()
