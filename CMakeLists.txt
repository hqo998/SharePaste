cmake_minimum_required(VERSION 3.14)
project(ShareNote)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)
set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)
file(MAKE_DIRECTORY ${EXTERNAL_DIR})

# Fetch cpp-httplib v0.28.0
set(HTTPLIB_URL "https://raw.githubusercontent.com/yhirose/cpp-httplib/v0.28.0/httplib.h")
set(HTTPLIB_FILE "${EXTERNAL_DIR}/httplib.h")


message(STATUS "Downloading cpp-httplib v0.28.0...")
file(DOWNLOAD ${HTTPLIB_URL} ${HTTPLIB_FILE} SHOW_PROGRESS)

include_directories(${EXTERNAL_DIR})

# sqlite3
FetchContent_Declare(
    sqlite3
    URL https://www.sqlite.org/2025/sqlite-amalgamation-3510100.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(sqlite3)

add_library(sqlite3_lib STATIC
    ${sqlite3_SOURCE_DIR}/sqlite3.c
)

add_executable(sharenote
    src/backend/main.cpp
    # ... your source files
)

set_target_properties(sharenote PROPERTIES UNITY_BUILD ON) #Use unity builds which lump files together to compile quicker
target_precompile_headers(sharenote PRIVATE <vector> <string> <sqlite3.h> <httplib.h>) #precompile headers to speed up compilation.

target_link_libraries(sharenote PRIVATE sqlite3_lib)

# Make build static on Linux
if (UNIX AND NOT APPLE)
    target_link_options(sharenote PRIVATE -static)
endif()

target_include_directories(sqlite3_lib PUBLIC ${sqlite3_SOURCE_DIR})
target_compile_definitions(sqlite3_lib PRIVATE SQLITE_THREADSAFE=1 SQLITE_ENABLE_COLUMN_METADATA=1)
