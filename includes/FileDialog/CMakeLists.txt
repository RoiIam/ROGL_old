cmake_minimum_required(VERSION 3.20)

project(FileDialog)

set(SOURCE_FILES "FileDialog.cpp")
set(HEADER_FILES "FileDialog.h")

# We declare the project as a static library and add all the source code files to it.
add_library(FileDialog STATIC ${HEADER_FILES} ${SOURCE_FILES})

target_link_libraries(ROGL PUBLIC FileDialog)


#target_include_directories(ASSIMPViewer PUBLIC include/src/FileDialog/)
#target_sources(ASSIMPViewer PRIVATE FileDialog.h FileDialog.cpp)
