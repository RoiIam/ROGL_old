#pragma once

#include <imgui.h>
#include <iostream>
#include <memory>
#include <vector>
#include <sstream>
#include <string> //to use  std::to_string
#include <random>
#include "FileDialog/FileDialog.h"
#include "ObjectInstance.h"
#include "Camera/Camera.h"
#include "Instances/ShaderInstance.h"
#include "Primitives/Cube.h"
#include "Primitives/Quad.h"
#include <glm/gtx/string_cast.hpp>

#include "Lights/DirectionalLight.h"
#include "Utilities/GraphicsOptions.h"

//#include "Lights/Light.h" //redefinition , already in objectinstance

//primitives
//#include "Primitives/Cube.h"
//#include "Primitives/DefaultCube.h"
//#include "Primitives/Grass.h"

//#include "../Assets/Shaders/ShaderInstance/ShaderClass.h"

//trochu pomimo?
struct WindowSettings {
    GLFWmonitor *monitor;
    GLFWwindow *window;
    unsigned int CUR_WIDTH = 800;
    unsigned int CUR_HEIGHT = 600;
}; //predtym } ws;



class SceneInstance {
public:
    SceneInstance();

    virtual ~SceneInstance();

    WindowSettings *windowSettings;//preco ma scena kazdy svoj?

    Camera *camera;
    GraphicsOptions * graphicsOptions = NULL;


    glm::mat4 projection;
    glm::mat4 view;
    std::string sceneDescription = "test Scene";

    std::vector<ObjectInstance *> selectableObjInstances; //objects including lights that can be selected
    std::vector<ObjectInstance *> opaqueObjInstances; //objects to be rendered
    std::vector<ObjectInstance *> lightObjInstances; //lights that hold light info and cube light representation to be rendered

    // renderQuad for framebuffers FBOs
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    //DefaultCube * cube = new DefaultCube(); //for

    //every scene has dir light for default with default pos of...
    Light *dirLight;
    glm::vec3 dirLightDirection = glm::vec3(0.5f, -1.0f, 0.5f);
    Shader *light_shader;
    Model *lightCube;
    ObjectInstance *dirLight_ObjInstance = NULL;
    //vector<Light*> lights;
    Shader basicShader = Shader("..\\Assets\\Shaders\\Forward\\basic.vert", "..\\Assets\\Shaders\\Forward\\basic.frag");

    int selectedHierarchyObj = -1;

    ObjectInstance *selectedInstance = nullptr; // either ray RMB  or ImGui window //if not set to nullptr, then crash
    Cube cubePrimitive = Cube(nullptr);

    StencilShaderInstance *stencilShader;

    virtual void Setup(Camera *cam, GraphicsOptions * graphicsOptions);

    //render selectable objects with supplied shader
    void RenderObjectsS(Shader *s);

    //render whole scene, can be overriden
    virtual void RenderSceneInstance(Shader *s); // later renderer class?

    //render lights in lightinstances
    virtual void RenderLights();

    void DrawSky();


    //create and assign sun directional light
    void SetupGlobalLight();

    //render gui for selecting, changing parameters
    virtual void ImGuiHierarchy();

    //didnt implement
    void LoadSceneInstance(std::string path);

    void renderQuad();

    virtual void ResizeScene();

    virtual void DeleteSceneBuffers();
};