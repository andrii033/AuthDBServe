cmake_minimum_required(VERSION 3.28)
project(AuthDBServe)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

# Locate SQLite3
find_package(SQLite3 REQUIRED)

# Include SQLite3 directories if needed
include_directories(${SQLite3_INCLUDE_DIRS})

# Create executable
add_executable(main main.cpp
        sql_create.cpp
        sql_create.h
        HttpServer.cpp
        HttpServer.h)

# Link the SQLite3 library
target_link_libraries(main PUBLIC SQLite::SQLite3)

# Copy resources folder to build directory
add_custom_target(copy_resources ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
)

add_dependencies(main copy_resources)