#
# SPDX-FileCopyrightText: 2021 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

find_program(YouTubeDl_EXE youtube-dl)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YouTubeDl
    FOUND_VAR
        YouTubeDl_FOUND
    REQUIRED_VARS
        YouTubeDl_EXE
)
