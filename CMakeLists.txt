#Minimum CMake version
cmake_minimum_required(VERSION 3.20.0)

list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
#Find Zephyr project
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
#Project name
project(BabbiesTracker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Main app file
target_sources(app PRIVATE src/main.cpp)

# Treat all compiler warnings as errors
add_compile_options(-Werror)

# Disable CONTAINER_OF warning
target_compile_options(app PRIVATE -Wno-invalid-offsetof)

#Custom modules
add_subdirectory(src/services)

