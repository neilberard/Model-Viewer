#Author: Neil Berard 
cmake_minimum_required(VERSION 3.8)

# set (CMAKE_CXX_FLAGS -m32)
# set(CMAKE_C_FLAGS -m32)


if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} /DEF:my_defs.def /NODEFAULTLIB")
endif()


# PREPROCESOR
add_definitions(-DGLEW_STATIC -D_MBCS) #-DSTB_IMAGE_IMPLEMENTATION)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ../bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ../bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)


# Model-Viewer properties 
project(Model-Viewer)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Model-Viewer)


# Tip: To list and format file paths use this command in git bash >>>  $ for f in *; do printf '"src/%s"\n' $f; done

# MODEL-VIEWER SOURCES
set(MODEL_VIEWER_SOURCE_LIST
	"src/Camera.h"
	"src/Debugging.cpp"
	"src/Debugging.h"
	"src/GLfunctions.h"
	"src/IndexBuffer.cpp"
	"src/IndexBuffer.h"
	"src/main.cpp"
	"src/Mesh.cpp"
	"src/Mesh.h"
	"src/Model.cpp"
	"src/Model.h"
	"src/Shader.cpp"
	"src/Shader.h"
	"src/Texture.cpp"
	"src/Texture.h"
	"src/Log.h"
	"src/Log.cpp"
	"src/Scene.cpp"
	"src/Scene.h"
	"src/Render.h"
	"src/Render.cpp"
	
)


#file(GLOB SOURCES src/*.cpp extern/imgui/*. INCLUDES src/*.h)
#add_executable(Model-Viewer ${SOURCES})

# IMGUI
set(IMGUI_SOURCE_LIST
	"extern/imgui/imconfig.h"
	"extern/imgui/imgui_internal.h"
	"extern/imgui/imstb_textedit.h"
	"extern/imgui/imgui.h"
    "extern/imgui/imstb_rectpack.h"
	"extern/imgui/imstb_truetype.h"
	"extern/imgui/imgui.cpp"
	"extern/imgui/imgui_demo.cpp"
	"extern/imgui/imgui_draw.cpp"
	"extern/imgui/imgui_widgets.cpp"
	"extern/imgui/examples/imgui_impl_glfw.cpp"
	"extern/imgui/examples/imgui_impl_glfw.h"
	"extern/imgui/examples/imgui_impl_opengl3.h"
	"extern/imgui/examples/imgui_impl_opengl3.cpp"
	)
	
# GLM
set(GLM_SOURCE_LIST
	"extern/glm/glm/glm.hpp"
	)
	
add_executable(Model-Viewer ${IMGUI_SOURCE_LIST} ${MODEL_VIEWER_SOURCE_LIST} ${GLM_SOURCE_LIST})
#add_subdirectory(extern/imgui)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${IMGUI_SOURCE_LIST} ${GLM_SOURCE_LIST})


#  CMAKE BUILD 
add_subdirectory(extern/assimp)
add_subdirectory(extern/glfw)
add_subdirectory(extern/spdlog)

# MODEL-VIEWER 
set_target_properties(	Model-Viewer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Debug")





# GLFW OUTPUT
set_target_properties( glfw
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)


# Dependencies 
add_dependencies(Model-Viewer assimp glfw IrrXML zlibstatic spdlog)

#Link Libraries "general" = both debug/release "optimized" = release
target_link_libraries(Model-Viewer

						# ASSIMP DEBUG LINK 
						debug "${CMAKE_CURRENT_SOURCE_DIR}/build/extern/assimp/lib/Debug/assimp-vc141-mtd.lib"
						debug "${CMAKE_CURRENT_SOURCE_DIR}/build/extern/assimp/lib/Debug/IrrXMLd.lib"
						debug "${CMAKE_CURRENT_SOURCE_DIR}/build/extern/assimp/lib/Debug/zlibstaticd.lib"
						
						
						debug "${CMAKE_CURRENT_SOURCE_DIR}/extern/glew/lib/glew32s.lib"
						debug "${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug/glfw3ddll.lib"
                        debug "${CMAKE_CURRENT_SOURCE_DIR}/build/extern/bin/Debug/spdlogd.lib"
						
						general "opengl32.lib"
						
						# ASSIMP RELEASE LINK
						optimized "${CMAKE_CURRENT_SOURCE_DIR}/build/extern/assimp/lib/Release/assimp-vc141-mt.lib"
						optimized "${CMAKE_CURRENT_SOURCE_DIR}/build/extern/assimp/lib/Release/IrrXML.lib"
						optimized "${CMAKE_CURRENT_SOURCE_DIR}/build/extern/assimp/lib/Release/zlibstatic.lib"							
						
						optimized "${CMAKE_CURRENT_SOURCE_DIR}/extern/glew/lib/glew32s.lib"
						optimized "${CMAKE_CURRENT_SOURCE_DIR}/bin/Release/glfw3dll.lib"	
                        optimized "${CMAKE_CURRENT_SOURCE_DIR}/build/extern/bin/Release/spdlog.lib"
					
						)

#INCLUDES
include_directories(${MyProject_SOURCE_DIR}extern/assimp/include 
					${MyProject_SOURCE_DIR}extern/glm 
					${MyProject_SOURCE_DIR}build/extern/assimp/include
					${MyProject_SOURCE_DIR}extern # IMGUI
					${MyProject_SOURCE_DIR}extern/imgui # IMGUI 					
					${MyProject_SOURCE_DIR}extern/glfw/include
					${MyProject_SOURCE_DIR}extern/glew/include
					${MyProject_SOURCE_DIR}extern/spdlog/include)
					