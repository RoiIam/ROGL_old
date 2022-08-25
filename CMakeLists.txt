cmake_minimum_required(VERSION 3.20)
project(ROGL )
#set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules/")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_FIND_DEBUG_MODE TRUE) #to write searched paths in console


#ASSIMP
#set(assimp_DIR "C:/Libs/AssimpV/Assimp/lib/cmake/assimp-5.2")
#set(assimp_DIR "C:/Program Files (x86)/Assimp/lib/cmake/assimp-5.2/")
#set(assimp_ROOT_DIR "C:/Libs/AssimpV/Assimp/")
#set(assimp_ROOT_DIR "C:/Program Files (x86)/Assimp")
#idk why it ignores paths, if i delete msys64 fodler it works, or better specify 5.2 and no need to delete
#better use just PATH and modules dir, no need to add version then
find_package(assimp 5.2 REQUIRED PATHS C:/Libs/AssimpV/Assimp/lib/cmake/assimp-5.2/)
#find_package(assimp PATHS "C:/Program Files (x86)/Assimp/")


include_directories("C:/Libs/AssimpV/Assimp/")
#include_directories("C:/Program Files (x86)/Assimp")

#GLEW
#set(GLEW_LIBRARIES_DIR "C:/Libs/glew-2.2.0/lib/Release/x64/")
#set(GLEW_LIBRARIES "C:/Libs/glew-2.2.0/lib/Release/x64/libglew32.a")
#set(GLEW_LIBRARIES "C:/msys64/usr/bin/libglew32.a")
#set(GLEW_LIBRARIES "C:/msys64/mingw64/lib/libglew32.a")

#set(GLEW_INCLUDE_DIRS "C:/msys64/mingw64/include/GL")
#set(GLEW_INCLUDE_DIRS "C:/msys64/usr/include/GL/")
#find_package(GLEW REQUIRED )
#set(glew_DIR "C:/Libs/glew-2.2.0/")
#add_library(GLEW  STATIC GLEW_LIBRARIES) #works only on source files
#include_directories(${GLEW_INCLUDE_DIRS})


#GLAD
set(GLAD_INCLUDE_DIR C:/Libs/GLAD_c_46/include/)
add_library(GLAD_S_LIB  C:/Libs/GLAD_c_46/src/glad.c )
include_directories(${GLAD_INCLUDE_DIR})


#link_libraries(${GLEW_LIBRARIES})
#link_directories(GLEW_LIBRARIES_DIR)


#GLFW
set(GLFW3_INCLUDE_DIR  "C:/Libs/GLFW338/include/")
set(GLFW3_LIBRARY  "C:/Libs/GLFW338/lib-mingw-w64/libglfw3.a")
set(GLFW3_LIB C:/Libs/GLFW338/lib-mingw-w64)

include_directories(${GLFW3_INCLUDE_DIR})
link_directories(${GLFW3_LIB})

#GLM
set(GLM_INCLUDE_DIR "C:/Libs/GLM")
include_directories(${GLM_INCLUDE_DIR})

#IMGUI
set(IMGUI_LIB_NAME C:/Libs/imgui_187/) #1.87

set(IMGUI_SOURCES
        "${IMGUI_LIB_NAME}/imgui.cpp"
        "${IMGUI_LIB_NAME}/imgui_draw.cpp"
        "${IMGUI_LIB_NAME}/imgui_widgets.cpp"
        "${IMGUI_LIB_NAME}/imgui_tables.cpp" #new in 187
        #"${IMGUI_LIB_NAME}/imgui_impl_glfw.cpp" #1.79
        #"${IMGUI_LIB_NAME}/imgui_impl_opengl3.cpp" #1.79
        "${IMGUI_LIB_NAME}/backends/imgui_impl_glfw.cpp"
        "${IMGUI_LIB_NAME}/backends/imgui_impl_opengl3.cpp"
        )

set(IMGUI_HEADERS
        "${IMGUI_LIB_NAME}/imconfig.h"
        "${IMGUI_LIB_NAME}/imgui.h"
        "${IMGUI_LIB_NAME}/imgui_internal.h"
        "${IMGUI_LIB_NAME}/imstb_rectpack.h"
        "${IMGUI_LIB_NAME}/imstb_textedit.h"
        "${IMGUI_LIB_NAME}/imstb_truetype.h"
        #"${IMGUI_LIB_NAME}/imgui_impl_glfw.h" #1.79
        #"${IMGUI_LIB_NAME}/imgui_impl_opengl3.h" #1.79
        "${IMGUI_LIB_NAME}/backends/imgui_impl_glfw.h"
        "${IMGUI_LIB_NAME}/backends/imgui_impl_opengl3.h"
        )
add_library(IMGUI_LIB ${IMGUI_SOURCES} ${IMGUI_HEADERS})
include_directories(${IMGUI_LIB_NAME})

#mozno este FileDialog a stb_image
# opisat co sa zmenilo
#set all libs that we want to be linked
set(LINK_LIBS assimp::assimp  ${GLFW3_LIBRARY}  GLAD_S_LIB  ${IMGUI_INCLUDE_DIR} ${IMGUI_LIB}  )
#trying to get rid of needing dynamic dlls,didnt work easily sadly


#find_package(IMGUI REQUIRED )
#find_package(GLFW REQUIRED )

#target_sources(ROGL PRIVATE main.cpp)
add_executable(ROGL main.cpp)


include_directories(Source)
add_subdirectory("Source")

#target_link_libraries(ROGL assimp::assimp)
#target_link_libraries(ROGL  ${GLEW_LIBRARIES})
#or
#target_link_libraries(ROGL  assimp::assimp ${GLEW_LIBRARIES})
#or:
target_link_libraries(ROGL PUBLIC  ${LINK_LIBS} )
#target_link_libraries(ROGL PUBLIC ${GLEW_LIBRARIES})