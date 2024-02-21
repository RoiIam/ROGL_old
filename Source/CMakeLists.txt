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
set(CMAKE_CXX_STANDARD 17)
set(ASSIMP_INCLUDE_DIR "C:/Program Files (x86)/Assimp/include/")

link_directories(../includes/FileDialog/)
include_directories("../includes/FileDialog")#needed to add ../
include_directories(${ASSIMP_INCLUDE_DIR})

add_library(ROGL_Source OBJECT
        Log.cpp ../includes/FileDialog/FileDialog.cpp ${IMGUI_LIB_NAME}/imgui_demo.cpp shader.cpp mesh.cpp model.cpp
        Utilities/Managers.cpp Utilities/PerfAnalyzer.cpp
        Instances/ObjectInstance.cpp Instances/SceneInstance.cpp Lights/Light.cpp Lights/DirectionalLight.cpp Lights/PointLight.cpp
        Camera/Camera.cpp Instances/ShaderInstance.cpp Primitives/Cube.cpp Primitives/DefaultCube.cpp Primitives/Grass.cpp Utilities/Controls.cpp
        Scenes/ForwardScene1.cpp Scenes/DeferredScene1.cpp Scenes/DeferredScene2.cpp
        Primitives/Quad.cpp Utilities/GraphicsOptions.cpp Primitives/Water.cpp Primitives/Path.cpp
        Scenes/SalanciGames/GameScene.cpp Scenes/SalanciGames/CannonGame.cpp Scenes/SalanciGames/PlanetGame.cpp
        Scenes/SalanciGames/PoolGame.cpp Scenes/SalanciGames/CoinMapGame.cpp
        Scenes/SalanciGames/Pathfinding.cpp Primitives/2D/Plane.cpp
        )


target_link_libraries(ROGL PUBLIC ROGL_Source)