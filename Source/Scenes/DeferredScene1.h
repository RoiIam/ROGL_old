#ifndef ROGL_DEFERREDSCENE1_H
#define ROGL_DEFERREDSCENE1_H

#include "../Instances/SceneInstance.h"
#include "../Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "../Lights//Light.h"
#include "glm/gtc/type_ptr.hpp" //for make_vec3

class DeferredScene1 : public SceneInstance {

    //there are no constructors, should we use base::base?
    using SceneInstance::SceneInstance;

    Model *ourModel;
    ObjectInstance *cube_ObjInstance;

    Shader depth_shader;
    Shader mesh_shader;
    Shader light_shaderCube;
    //= Shader("..\\Assets\\Shaders\\Debug\\emptyPink.vert", "..\\Assets\\Shaders\\Debug\\emptyPink.frag");

    //model;
    //light;

    //deferred part
    unsigned int gBuffer;
    unsigned int gPosition, gNormal, gAlbedoSpec;
    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    unsigned int rboDepth;

    bool isDebugPos = false;
    bool isDebugNorm = false;
    bool isDebugAlbedo = false;


    const unsigned int NR_LIGHTS = 32;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
    Shader shaderGeometryPass;
    Shader shaderLightingPass;
    Shader shaderLightBox;
    Shader shaderFBODebug;
    std::vector<glm::vec3> objectPositions;
    bool showDeferredDebug = true;
    Model lightCube = Model("../Assets/Models/LightCube/LightCube.obj");

    void ImGuiHierarchy() override;


    void Setup(Camera *cam) override;

    void RenderSceneInstance(Shader *shader) override;

};

#endif //ROGL_DEFERREDSCENE1_H
