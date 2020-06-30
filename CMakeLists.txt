#Auther: Neil Berard
cmake_minimum_required(VERSION 3.10)

add_definitions(-DASSIMP_BUILD_STATIC_LIB=ON)
set(ZLIB_LIBRARIES zlibstatic)
set(ENABLE_BOOST_WORKAROUND ON)
set(BUILD_STATIC_LIB ON)
set(BUILD_ASSIMP_TOOLS  ON)
set(ASSIMP_BUILD_STATIC_LIB ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)



# set the project name 
project(Model-Viewer)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Model-Viewer)

file(GLOB SOURCES src/*.cpp INCLUDES src/*.h)
add_executable(Model-Viewer ${SOURCES})


#  SPDLOG
#add_subdirectory(vendor/spdlog bin)

#  ASSIMP
add_subdirectory(extern/assimp)

set_target_properties( assimp
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/extern/assimp"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/extern/assimp"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/extern/assimp"
)

# Dependencies 
add_dependencies(Model-Viewer assimp)

#Link Libraries
target_link_libraries(Model-Viewer debug "${CMAKE_CURRENT_SOURCE_DIR}/bin/extern/assimp/Debug/assimp-vc141-mtd.lib")


include_directories(${MyProject_SOURCE_DIR}extern/assimp/include ${MyProject_SOURCE_DIR}extern/glm ${MyProject_SOURCE_DIR}build/extern/assimp/include)