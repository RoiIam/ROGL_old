# testing adding a library/ source files, outside of main cmakelists, so we can add source cpp files here
# we tested redefinitions of Help(), in test.cpp and Log.cpp, working great
# it works as expected, adding both sources creates redefinition error, but its fine with just one of them
# plus we can have both log and test headers included in main.cpp

#old  way targeting it
#target_sources(ROGL
#        PRIVATE Log.cpp
#        PUBLIC Log.h test.h)
#target_sources(ROGL ROGL_Source)


#nah just create library
# if we dont specify object but ROGL_Source STATIC it wont show us error of redefinitions, weird

#find_package(ASSIMP 5.2 PATHS "C:/Program Files (x86)/Assimp/")
set(ASSIMP_INCLUDE_DIR  "C:/Program Files (x86)/Assimp/include/")

link_directories(includes/FileDialog/)

include_directories(includes/FileDialog)
include_directories(${ASSIMP_INCLUDE_DIR})

add_library(ROGL_Source OBJECT
         Log.cpp shader.cpp model.cpp mesh.cpp
        Utilities/Managers.cpp Utilities/PerfAnalyzer.cpp
        Instances/ObjectInstance.cpp Instances/SceneInstance.cpp Lights/Light.cpp Lights/DirectionalLight.cpp Lights/PointLight.cpp Camera/Camera.cpp)




target_link_libraries(ROGL PUBLIC ROGL_Source )