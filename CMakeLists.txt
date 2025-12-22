cmake_minimum_required(VERSION 3.14)
project(SharePastee)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_COMPILE_WARNING_AS_ERROR TRUE)

include(FetchContent)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(FETCHCONTENT_BASE_DIR ${CMAKE_BINARY_DIR}/_deps)
set(EXTERNAL_DIR ${CMAKE_BINARY_DIR}/external)
file(MAKE_DIRECTORY ${EXTERNAL_DIR})

# Fetch cpp-httplib v0.28.0
set(HTTPLIB_URL "https://raw.githubusercontent.com/yhirose/cpp-httplib/v0.28.0/httplib.h")
set(HTTPLIB_FILE "${EXTERNAL_DIR}/httplib.h")


message(STATUS "Downloading cpp-httplib v0.28.0...")
file(DOWNLOAD ${HTTPLIB_URL} ${HTTPLIB_FILE} SHOW_PROGRESS)

set(HTTPLIB_URL "https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp")
set(HTTPLIB_FILE "${EXTERNAL_DIR}/nlohmann/json.hpp")

message(STATUS "Downloading json for cpp v3.12.0...")
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

target_include_directories(sqlite3_lib PUBLIC ${sqlite3_SOURCE_DIR})
target_compile_definitions(sqlite3_lib PRIVATE SQLITE_THREADSAFE=1 ) #SQLITE_ENABLE_COLUMN_METADATA=1

# need since g++ has fun errors that other compilers dont have. hopeful fix by telling g++ to ignore warning
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(sqlite3_lib PRIVATE -Wno-stringop-overread)
endif()

add_executable(sharepaste
    src/backend/main.cpp
    src/backend/dbmanager/dbmanager.cpp
    src/backend/utility/utility.cpp
    src/backend/testtools/testtools.cpp
    # ... your source files
)

target_include_directories(sharepaste PUBLIC
${CMAKE_SOURCE_DIR}/src/backend/dbmanager
${CMAKE_SOURCE_DIR}/src/backend/utility
${CMAKE_SOURCE_DIR}/src/backend/testtools
)

# optimisation stuff
set_target_properties(sharepaste PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
set_target_properties(sharepaste PROPERTIES UNITY_BUILD OFF) #Use unity builds which lump files together to compile quicker but makes debugger harder
target_precompile_headers(sharepaste PRIVATE <vector> <string> <sqlite3.h> <httplib.h> <nlohmann/json.hpp>) #precompile headers to speed up compilation.

target_link_libraries(sharepaste PRIVATE sqlite3_lib)



# Make build static on Linux
if (UNIX AND NOT APPLE)
    target_link_options(sharepaste PRIVATE -static)
endif()





#copy frontend to build folder regardless of configuration
add_custom_target(copy_frontend ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/frontend
        $<TARGET_FILE_DIR:sharepaste>/www
)


