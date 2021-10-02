cmake_minimum_required(VERSION 3.16)

project(hello-all)

#External Libraries
set(LIBRARIES "")

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

link_libraries(${LIBRARIES})

#Modules
set(MODULE hello)
set(MODULE_DIR "")
add_executable(${MODULE} "${SOURCE_DIR}/${MODULE}.cpp")
add_resources(${MODULE} "")

#Modules are different sections of an application for different
#purposes, separated by library or executable. There is usually
#a global module that shares code with lesser submodules. There
#can be N number of submodules.

#Include more modules here if needed.

# set(MODULE hello2)
# set(MODULE_DIR "")
# add_library(${MODULE} "${SOURCE_DIR}/${MODULE}.cpp")
# add_resources(${MODULE} "")

#Subdirectories
add_subdirectory("${SOURCE_DIR}")
add_subdirectory("${INCLUDE_DIR}")

#Subdirectories point to folders with source code to build, typically
#cpp or h files. Each level is a module which adds sources to its library
#or executable, before adding its submodules as subdirectories. The 
#subdirectories at this level are typically the cpp and h files for the
#global module.