#
# SPDX-FileCopyrightText: 2025 George Florea Bănuș <georgefb899@gmail.com>
#
# SPDX-License-Identifier: BSD-3-Clause
#

qt_add_library(database STATIC)
add_library(Haruna::Database ALIAS database)
qt_add_qml_module(database
    URI org.kde.haruna.database
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src/org/kde/haruna/database
    IMPORT_PATH ${CMAKE_BINARY_DIR}
    SOURCES
        database.h
        database.cpp
        migrationmanager.h
        migrationmanager.cpp
        recentfile.h
)

qt_add_resources(database sql_files
    FILES
        sql/v1/create-metadata_cache-table.sql
        sql/v1/create-recent_files-table.sql
        sql/v1/create-playback_position-table.sql

        sql/v2/create-metadata_cache-table.sql
        sql/v2/create-recent_files-table.sql
        sql/v2/create-urls-table.sql
)

ecm_qt_declare_logging_category(database
    HEADER "logging/_database.h"
    IDENTIFIER "HarunaDatabase"
    CATEGORY_NAME "org.kde.haruna.database"
    DEFAULT_SEVERITY Info
)

target_link_libraries(database
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Sql

        KF6::FileMetaData
        KF6::I18n

        Haruna::Utilities
)
