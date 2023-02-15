#include "ObjectInstance.h"
#include <imgui.h>
#include <iostream>
#include <vector>
#include <random>
#include "FileDialog/FileDialog.h"
#include "ObjectInstance.h"
//#include "Camera.h"

//#include "Light.h"

//primitives
//#include "Primitives/Cube.h"
//#include "Primitives/DefaultCube.h"
//#include "Primitives/Grass.h"

//#include "../Assets/Shaders/ShaderInstance/ShaderClass.h"



class SceneInstance{

public:
    SceneInstance()= default;
    virtual ~SceneInstance(){
    };

    // TODO add camera
    struct Camera{};
    Camera * camera;
    glm::mat4 projection;
    glm::mat4 view;
    std::string sceneDescription="test Scene";

    std::vector<ObjectInstance*> selectableObjInstances; //objects including lights that can be selected
    std::vector<ObjectInstance*> opaqueObjInstances; //objects to be rendered
    std::vector<ObjectInstance*> lightObjInstances; //lights that hold light info and cube light representation to be rendered

    // renderQuad for framebuffers FBOs
    unsigned int quadVAO = 0;
    unsigned int quadVBO;
    //DefaultCube * cube = new DefaultCube(); //for

    //every scene has dir light for default with default pos of...
    //Light* dirLight;
    glm::vec3 dirLightDirection = glm::vec3 (0.5f, -1.0f,0.5f);
    Shader* light_shader;
    Model* lightCube;
    ObjectInstance * dirLight_ObjInstance;
    //vector<Light*> lights;
    Shader basicShader= Shader("..\\Assets\\Shaders\\Basic\\basic.vert", "..\\Assets\\Shaders\\Basic\\basic.frag");
    Shader* ourShader = new Shader("..\\Assets\\Shaders\\01_SimpleTexture\\1.model_loading.vs", "..\\Assets\\Shaders\\01_SimpleTexture\\1.model_loading.fs");
    int selectedHierarchyObj = -1;
    bool disableShadows = true;

    ObjectInstance * selectedInstance; // either ray RMB  or ImGui window
    //Cube cubePrimitive = Cube(nullptr);

    //StencilShaderInstance * stencilShader = new StencilShaderInstance();

    virtual void Setup(Camera *cam);

    //render selectable objects with supplied shader
    void RenderObjectsS(Shader* s);

    //render whole scene, can be overriden
    virtual void RenderSceneInstance(Shader* s); // later renderer class?

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
};