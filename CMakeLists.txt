#
# jimmy - A multipurpose Discord bot
# =========================================
#

cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(jimmy
        VERSION 0.1.0
        DESCRIPTION "A Discord bot that will conquer the world one day..."
        HOMEPAGE_URL "https://github.com/TheMonHub/jimmy"
        LANGUAGES C
)

set(CMAKE_PROJECT_VERSION "${CMAKE_PROJECT_VERSION}-INDEV")

set(CMAKE_C_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(Concord REQUIRED IMPORTED_TARGET concord)

find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)

add_executable(jimmy
        src/main.c
        src/token.c
        src/token.h
        src/main.h
        src/command.c
        src/command.h
        src/commands/ping.c
        src/commands/ping.h
        src/commands/index.c
        src/commands/index.h
        src/database.c
        src/database.h
        src/commands/about.c
        src/commands/about.h
        src/colors.h
        src/images.h
        src/statements/database_init.h
        src/statements/helper.h
        src/statements/check_version.h
        src/statements/set_version.h
        src/statements/guild_dat.h
        src/statements/conf.h
        src/statements/add_rule.h
        src/statements/add_case.h
        src/statements/add_tban.h
        src/statements/remove_expired_tbans.h
        src/statements/member.h
        src/statements/get_guild_dat.h
        src/statements/get_rule.h
        src/statements/get_case.h
        src/statements/get_conf.h
        src/statements/get_tban.h
)

target_link_libraries(jimmy PRIVATE
        Threads::Threads
        CURL::libcurl
        SQLite::SQLite3
        PkgConfig::Concord
)

target_compile_definitions(jimmy PRIVATE PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")
