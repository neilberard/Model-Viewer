#Auther: Neil Berard
cmake_minimum_required(VERSION 3.10)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)



# set the project name 
project(Model-Viewer)

file(GLOB SOURCES src/*.cpp)
add_executable(Model-Viewer ${SOURCES})


#  SPDLOG
#add_subdirectory(vendor/spdlog bin)

#  ASSIMP
add_subdirectory(extern/assimp)

