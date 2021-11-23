#
# SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

find_program(Ytdlp_EXE yt-dlp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ytdlp
    FOUND_VAR
        Ytdlp_FOUND
    REQUIRED_VARS
        Ytdlp_EXE
)
