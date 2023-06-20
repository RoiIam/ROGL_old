#ifndef ROGL_DEFERREDSCENE2_H
#define ROGL_DEFERREDSCENE2_H

#include "../Instances/SceneInstance.h"
#include "../Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "../Lights//Light.h"
#include "Primitives/Water.h"
#include "glm/gtc/type_ptr.hpp" //for make_vec3

#include "FileDialog/FileDialog.h"


class DeferredScene2 : public SceneInstance {//should we use public?


    using SceneInstance::SceneInstance;//should we?

public:

    DeferredScene2();

    ~DeferredScene2() override;

// we start with only this....
    Model *sponzaModel;
    ObjectInstance *sponzaObjInstance;
    ObjectInstance *waterObjInstance;
    Shader *meshLightShader;
    Shader *basicShader;
    bool OpenSecond = false;

    void Setup(Camera *cam, GraphicsOptions *graphicsOptions) override;


    //stuff for simple lights stage 1
#pragma region stage_1 //TODO move this to .cpp?
    Shader *simpleLights = new Shader("..\\Assets\\Shaders\\Forward\\MultipleLights\\forwardMultiple.vert",
                                      "..\\Assets\\Shaders\\Forward\\MultipleLights\\forwardMultiple.frag");
    //glm::vec3 dirLightDirImGui = dirLight_ObjInstance->GetPos();
    float dirLightDirImGui[3] = {-0.222f, -0.666f, -0.444f};
    glm::vec3 dirlightCol = glm::vec3(0.945f, 0.894f, 0.753f);
    glm::mat4 model;

    void SetupForwardLights();

#pragma endregion stage_1
    //forward+shadows
#pragma region stage_2
    Shader *shadowLights = new Shader("..\\Assets\\Shaders\\Forward\\Shadows\\forwardMultipleShadows.vert",
                                      "..\\Assets\\Shaders\\Forward\\Shadows\\forwardMultipleShadows.frag");
#pragma endregion stage_2

    //SSAO
#pragma region stage_3
    Shader *shaderGeometryPass = new Shader("..\\Assets\\Shaders\\Deferred\\SSAO\\ssao_geometry.vert",
                                            "..\\Assets\\Shaders\\Deferred\\SSAO\\ssao_geometry.frag");
    Shader *shaderLightingPass = new Shader("..\\Assets\\Shaders\\Deferred\\SSAO\\ssao.vert",
                                            "..\\Assets\\Shaders\\Deferred\\SSAO\\ssao_lighting.frag");
    Shader *shaderLightingPassOFF = new Shader("..\\Assets\\Shaders\\Deferred\\SSAO\\ssao.vert",
                                               "..\\Assets\\Shaders\\Deferred\\SSAO\\ssao_lightingOFF.frag");
    Shader *shaderSSAO = new Shader("..\\Assets\\Shaders\\Deferred\\SSAO\\ssao.vert",
                                    "..\\Assets\\Shaders\\Deferred\\SSAO\\ssao.frag");
    Shader *shaderSSAOBlur = new Shader("..\\Assets\\Shaders\\Deferred\\SSAO\\ssao.vert",
                                        "..\\Assets\\Shaders\\Deferred\\SSAO\\ssao_blur.frag");
    Shader shaderFBODebug = Shader("..\\Assets\\Shaders\\Deferred\\debug_fbo.vert",
                                   "..\\Assets\\Shaders\\Deferred\\debug_fbo.frag");


    unsigned int gBuffer;
    unsigned int gPosition, gNormal, gAlbedo;
    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    unsigned int rboDepth;
    unsigned int ssaoFBO, ssaoBlurFBO;
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
    std::vector<glm::vec3> ssaoNoise;

    std::vector<glm::vec3> ssaoKernel;
    unsigned int noiseTexture;
    bool notFirstTime = false;
    bool isDebugSSAO = false;
    bool enableSSAO = false;
    int powerSSAO = 1;

    float lerp(float a, float b, float f);

    void SetupSSAO();

    void RenderSceneInstance(Shader *shader) override;

#pragma endregion stage_3

#pragma region water
    //create reflection and refraction buffers
    unsigned int reflectionFrameBuffer;
    unsigned int reflectionTexture;
    unsigned int reflectionDepthBuffer;

    unsigned int refractionFrameBuffer;
    unsigned int refractionTexture;
    unsigned int refractionDepthBuffer;
    unsigned int refractionDepthTexture;


    //ui
    float fresnelStrengthUI = 0.5;
    float waveSpeedUI[2];

    bool renderRefraction = false;
    bool renderReflection = false;

    Shader waterShader = Shader("..\\Assets\\Shaders\\Forward\\Water\\water.vert",
                                "..\\Assets\\Shaders\\Forward\\Water\\water.frag");

    void SetupWater();

    void RenderWater();

#pragma endregion water

    void LoadModel(std::string path);

    //main way to interact with the demo with buttons
    void ImGuiHierarchy() override;
};

#endif //ROGL_DEFERREDSCENE2_H