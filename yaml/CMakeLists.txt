cmake_minimum_required(VERSION 3.16)

project(nbg-yaml)

#Build settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

#Global settings
get_filename_component(SOURCE_DIR src ABSOLUTE)
get_filename_component(INCLUDE_DIR include ABSOLUTE)

include_directories("${INCLUDE_DIR}")

#Modules
set(MODULE ${PROJECT_NAME})
add_library(${MODULE} STATIC | SHARED)
target_link_libraries(${MODULE}
    yaml-cpp
)

#Subdirectories
add_subdirectory("${SOURCE_DIR}/nbg")