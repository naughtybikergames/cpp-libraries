cmake_minimum_required(VERSION 3.16)

project(nbg-cli-args VERSION 0.1)

#Build settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib")

#Global settings
get_filename_component(SOURCE_DIR src ABSOLUTE)
get_filename_component(INCLUDE_DIR include ABSOLUTE)
get_filename_component(RESOURCE_DIR resources ABSOLUTE)

include("${RESOURCE_DIR}/CMakeLists.txt")

include_directories("${INCLUDE_DIR}")

#Modules
set(MODULE ${PROJECT_NAME})
add_library(${MODULE})
target_library_distribution(${MODULE} dist/libnbg-cli-args-dev)
target_file_distribution(${MODULE} "${INCLUDE_DIR}" dist/libnbg-cli-args-dev DESTINATION usr)
target_library_distribution(${MODULE} dist/libnbg-cli-args TYPE SHARED)

#Subdirectories
add_subdirectory("${SOURCE_DIR}/nbg")