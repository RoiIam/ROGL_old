#include "DeferredScene2.h"
#include <utility>

DeferredScene2::DeferredScene2() = default;

DeferredScene2::~DeferredScene2() = default;

void DeferredScene2::Setup(Camera *cam, GraphicsOptions *graphicsOptions) {

    SceneInstance::Setup(cam, graphicsOptions);
    //camera = cam;
    sceneDescription = "This is a final scene showcasing forward and deferred rendering step by step, check the 'Interactive' UI window";

    sponzaModel = new Model("..\\Assets\\Models\\Sponza\\sponza.obj");
    meshLightShader = new Shader("..\\Assets\\Shaders\\Forward\\MultipleLights\\mesh.vert",
                                 "..\\Assets\\Shaders\\Forward\\MultipleLights\\mesh.frag");
    basicShader = new Shader("..\\Assets\\Shaders\\Forward\\01_SimpleTexture\\1.model_loading.vs",
                             "..\\Assets\\Shaders\\Forward\\01_SimpleTexture\\1.model_loading.fs");
    sponzaObjInstance = new ObjectInstance(*sponzaModel, *basicShader, "sponza", nullptr);
    sponzaObjInstance->SetScale(glm::vec3(0.020f));
    selectableObjInstances.push_back(sponzaObjInstance);

    selectableObjInstances.push_back(dirLight_ObjInstance);


    SetupForwardLights();

    SetupWater();
}

void DeferredScene2::SetupForwardLights() {
    simpleLights->use();
    model = glm::mat4(1.0f);  // same , below
    // m += 0.008f; dont move
    //model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.5f));
    //model = glm::scale(
    //    model, glm::vec3(1.0f, 1.0f, 1.0f));  // lets try setting it here
    //unsigned int transformLoc = glGetUniformLocation(mesh_shader->ID, "model");
    // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(model));
    //simpleLights->setMat4("model", model);
    //simpleLights->setVec3("light.position", lightPos);
    simpleLights->setVec3("viewPos", camera->Position);
    // light properties
    simpleLights->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);
    simpleLights->setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.7f);
    simpleLights->setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
    simpleLights->setVec3("dirLight.direction", 0.7f, 0.7f, -0.7f);
    // material properties
    //simpleLights->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    simpleLights->setFloat("material.shininess", 32.0f);
    // directional light
    //mesh_shader->setVec3("dirLight.direction",glm::make_vec3(dirLightDirImGui));
    //simpleLights->setVec3("dirLight.direction",static_cast<DirectionalLight>(dirLight_ObjInstance->light).direction);  //uhh static casts
    /*glm::vec3 a =static_cast<DirectionalLight>(dirLight_ObjInstance->light).direction;*/
}

void DeferredScene2::SetupWater() {

    Water *a = new Water(&waterShader,camera);

    waterObjInstance = new ObjectInstance(*(a), waterShader, "waterQuad", nullptr);

    waterObjInstance->SetScale(glm::vec3(5));
    waterObjInstance->SetDeg(-90.0f, "X");//make sure its correctly rotated-facing up along X
    waterObjInstance->SetDeg(-90.0f, "Z");//make sure its correctly rotated-facing up along Z

    //waterObjInstance->SetRot(glm::vec3(0, 1, 0));//rotates based on prev set Degrees

    selectableObjInstances.push_back(waterObjInstance);
    waterObjInstance->disableRender = true;



    //water setup
    //waterObjInstance->SetPos(glm::make_vec3("0.0"));//should work?

    //create reflection frame buffer
    //init frame buffer
    glGenFramebuffers(1, &reflectionFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFrameBuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    //init texture attachment
    glGenTextures(1, &reflectionTexture);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTexture, 0);
    //init depth buffer attachment
    glGenRenderbuffers(1, &reflectionDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionDepthBuffer);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer reflectionDepthBuffer not complete!" << std::endl;

    //unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    //create reflection frame buffer
    //init refraction frame buffer
    glGenFramebuffers(1, &refractionFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFrameBuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    //init texture attachment
    glGenTextures(1, &refractionTexture);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionTexture, 0);
    //init depth buffer attachment
    glGenRenderbuffers(1, &refractionDepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, refractionDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, refractionDepthBuffer);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer refractionDepthBuffer not complete!" << std::endl;
    //unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //TODO this is bad code... assign after binding...
    a->reflectionTexture = reflectionTexture;
    a->refractionTexture = refractionTexture;
}


void DeferredScene2::RenderWater() {
    //std::cout << "RenderWater!" << std::endl;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0); // make sure not bound
    //glBindFramebuffer(GL_FRAMEBUFFER,reflectionFrameBuffer);



    if (renderReflection)
        glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    else if (renderRefraction)
        glBindTexture(GL_TEXTURE_2D, refractionTexture);
    if (renderReflection or renderRefraction) {
        //to test, render renderQuad

        //draws white if selected?
        shaderFBODebug.use();
        renderQuad();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float DeferredScene2::lerp(float a, float b, float f) {
    return a + f * (b - a);
}

void DeferredScene2::SetupSSAO() {

    //make sure to run it onlly once
    if (notFirstTime)
        return;
    notFirstTime = true;
    // configure g-buffer framebuffer
    // ------------------------------
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // also create framebuffer to hold SSAO processing stage
    // -----------------------------------------------------
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RED, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // generate sample kernel
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator(22);
    for (unsigned int i = 0; i < 64; ++i) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;

        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0,
                        0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // lighting info
    // -------------
    glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);

    // shader configuration
    // --------------------
    shaderLightingPass->use();
    shaderLightingPass->setInt("gPosition", 0);
    shaderLightingPass->setInt("gNormal", 1);
    shaderLightingPass->setInt("gAlbedo", 2);
    shaderLightingPass->setInt("ssao", 3);
    shaderLightingPassOFF->use();
    shaderLightingPassOFF->setInt("gPosition", 0);
    shaderLightingPassOFF->setInt("gNormal", 1);
    shaderLightingPassOFF->setInt("gAlbedo", 2);
    shaderLightingPassOFF->setInt("ssao", 3);


    shaderSSAO->use();
    shaderSSAO->setInt("gPosition", 0);
    shaderSSAO->setInt("gNormal", 1);
    shaderSSAO->setInt("texNoise", 2);
    shaderSSAO->setInt("power", powerSSAO);
    shaderSSAOBlur->use();
    shaderSSAOBlur->setInt("ssaoInput", 0);

}

void DeferredScene2::RenderSceneInstance(Shader *shader) {
    if (graphicsOptions->rendererType == GraphicsOptions::RendererType::forward) {
        SceneInstance::RenderSceneInstance(shader);
    } else {

//NOTE its pointless to raypick, use menu instead
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);



// 1. geometry pass: render scene's geometry/color data into gbuffer
// -----------------------------------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        RenderObjectsS(shaderGeometryPass);
//renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

// 2. generate SSAO texture
// ------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO->use();
// Send kernel + rotation
        for (unsigned int i = 0; i < 64; ++i)
            shaderSSAO->setVec3("samples[" + std::to_string(i) + "]",
                                ssaoKernel[i]); // if this breaks, well we ddont run setupssao eh
        shaderSSAO->setMat4("projection", projection);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


// 3. blur SSAO texture to remove noise
// ------------------------------------
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAOBlur->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        renderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


// 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
// -----------------------------------------------------------------------------------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Shader *shaderToUse;
        if (enableSSAO)
            shaderToUse = shaderLightingPass;
        else
            shaderToUse = shaderLightingPassOFF;
        shaderToUse->use();
// send light relevant uniforms

        glm::vec3 lightPosView = glm::vec3(uniforms.view * glm::vec4(dirLight_ObjInstance->GetPos(), 0.0));
        shaderToUse->setVec3("light.Position", lightPosView);
        glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
        shaderToUse->setVec3("light.Color", dirLight_ObjInstance->light->color);

// Update attenuation parameters
        const float linear = 0.09f;
        const float quadratic = 0.032f;
        shaderToUse->setFloat("light.Linear", linear);
        shaderToUse->setFloat("light.Quadratic", quadratic);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        renderQuad();


        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        glBlitFramebuffer(0, 0, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, 0, windowSettings->CUR_WIDTH,
                          windowSettings->CUR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        DrawSky();


        if (isDebugSSAO) {
            shaderFBODebug.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
            renderQuad();
        }

    }
}

void DeferredScene2::LoadModel(std::string path) {
    if (path.empty())
        return;
    Model *m = new Model(path, false);

    Shader *s;
    switch (graphicsOptions->rendererType) {
        case GraphicsOptions::RendererType::forward: //0
            s = simpleLights;
            break;
        case GraphicsOptions::RendererType::deferred: //1
            s = shaderLightingPass;
            break;
        default:
            s = NULL;
            break;

    }
    ObjectInstance *a = new ObjectInstance(*m, *s, "openModel", nullptr);
    selectableObjInstances.push_back(a);
    selectedInstance = a;
}

void DeferredScene2::ImGuiHierarchy() {
    SceneInstance::ImGuiHierarchy();


    ImGui::Begin("Interactive");
    ImGui::TextWrapped(
            "Welcome to this Sponza demo with Blinn-Phong lighting. By pressing buttons bellow, you will add more graphical features that will make it look even better!");


    if (ImGui::CollapsingHeader("Various Options")) {
        ImGui::Checkbox("Switch shadows on (default off)", &graphicsOptions->enableShadows);
    }
//imgui buttons that handle switching of demo stages and their setup
#pragma region buttonsAndSetup


    ImGui::TextWrapped("Use 'Sun offset' in the 'Tool Window' UI Window to change sun direction");
    if (ImGui::Button("Change to deferred rendering") &&
        graphicsOptions->rendererType == GraphicsOptions::RendererType::forward) {
        SetupSSAO();

        graphicsOptions->rendererType = GraphicsOptions::RendererType::deferred;
    }
    if (ImGui::Button("Return back to forward rendering") &&
        graphicsOptions->rendererType == GraphicsOptions::RendererType::deferred) {
        graphicsOptions->rendererType = GraphicsOptions::RendererType::forward;
    }
    ImGui::TextWrapped("Loads model from hard drive and selects it: ");

    if (ImGui::Button("Load Model into scene")) {
//OpenSecond = true;
        static char *path = FileDialog::Open();
        LoadModel(path);
    }

#pragma endregion buttons
//not working for now DirectionalLight*  a = dynamic_cast<DirectionalLight*>(dirLight);
    switch (graphicsOptions->rendererType) {

        case GraphicsOptions::RendererType::forward://dir light with shadows //0
//simpleLights->setVec3("dirLight.diffuse", dirlightCol);
            break;
        case GraphicsOptions::RendererType::deferred://deferred pipeline //1

            ImGui::Checkbox("enable SSAO", &enableSSAO);
            ImGui::InputInt("AO power", &powerSSAO, 1);


//shaderLightingPass->setVec3("Light.direction", );
//dynamic_cast<DirectionalLight*>(dirLight_ObjInstance->light)->direction = glm::vec3(dirLightDirection);
            ImGui::Checkbox("showDebug SSAO", &isDebugSSAO);
            break;
            //case 4: //deffered+ shadows? or ssao
            //break;
        default:
            break;

    }


#pragma region water

    waveSpeedUI[0] = ((Water*)waterObjInstance->GetModel())->waveSpeed.x;
    waveSpeedUI[1] = ((Water*)waterObjInstance->GetModel())->waveSpeed.y;
    ImGui::SliderFloat2("water speed, direction", waveSpeedUI, -0.0025, 0.0025 ,"%.04f");
    ((Water*)waterObjInstance->GetModel())->waveSpeed = glm::make_vec2(waveSpeedUI);


    ImGui::SliderFloat("water fresnelStrength", &fresnelStrengthUI, 0.1, 10, "%.3f");
    waterShader.use();
    waterShader.setFloat("fresnelStrength", fresnelStrengthUI);
    ImGui::Checkbox("renderQuad debug water, renderReflection", &renderReflection);
    ImGui::Checkbox("renderQuad debug water, renderRefraction", &renderRefraction);

#pragma endregion water
    ImGui::TextWrapped("%s", std::string("curr stage ").append(
            std::to_string(graphicsOptions->rendererType)).c_str()); //cant use std::to_chars even with std::to_chars
    //now it can print only number, enum names are not complied etc
    ImGui::End();
}


