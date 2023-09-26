cmake_minimum_required(VERSION 3.20)
project(ROGL)
#set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/modules/")

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS -fuse-ld=lld) #loong times for standard gxx projectdidnt work
set(CMAKE_FIND_DEBUG_MODE TRUE) #to write searched paths in console
#seems like using find_package helps us include it as static
find_package(ASSIMP 5.2 PATHS "C:/Program Files (x86)/Assimp/")

#GLAD
set(GLAD_INCLUDE_DIR C:/Libs/GLAD_c_46/include/)
add_library(GLAD_S_LIB C:/Libs/GLAD_c_46/src/glad.c)
include_directories(${GLAD_INCLUDE_DIR})

#GLFW
set(GLFW3_INCLUDE_DIR "C:/Libs/GLFW338/include/")
set(GLFW3_LIBRARY "C:/Libs/GLFW338/lib-mingw-w64/libglfw3.a")
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
include_directories("includes")
#set all libs that we want to be linked
set(LINK_LIBS ${GLFW3_LIBRARY} GLAD_S_LIB IMGUI_LIB)#tu bola chyba IMGUI_LIB ma byt bez ${} a ${IMGUI_INCLUDE_DIR} not needed


add_executable(ROGL main.cpp)
target_compile_options(ROGL PUBLIC -ggdb)

include_directories(Source)
add_subdirectory("Source")

target_link_libraries(ROGL PUBLIC assimp::assimp ${LINK_LIBS})

target_link_options(ROGL PRIVATE -static-libgcc -static-libstdc++ -Wl,-Bdynamic -lwinpthread)