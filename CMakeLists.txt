cmake_minimum_required(VERSION 3.14)
project(ShareNote)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static")

include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)
set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)
file(MAKE_DIRECTORY ${EXTERNAL_DIR})

# # Fetch cpp-httplib
set(HTTPLIB_URL "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h")
set(HTTPLIB_FILE "${EXTERNAL_DIR}/httplib.h")

if(NOT EXISTS ${HTTPLIB_FILE})
    message(STATUS "Downloading httplib.h...")
    file(DOWNLOAD ${HTTPLIB_URL} ${HTTPLIB_FILE} SHOW_PROGRESS)
endif()

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
    src/hello.cpp
    # ... your source files
)



target_link_libraries(sharenote PRIVATE sqlite3_lib)

target_include_directories(sqlite3_lib PUBLIC ${sqlite3_SOURCE_DIR})
target_compile_definitions(sqlite3_lib PRIVATE SQLITE_THREADSAFE=1 SQLITE_ENABLE_COLUMN_METADATA=1)
