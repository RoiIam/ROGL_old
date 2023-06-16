#include "DeferredScene1.h"


DeferredScene1::DeferredScene1() = default;

DeferredScene1::~DeferredScene1() {
    delete cube_ObjInstance; //TODO this is wrong?
}

void DeferredScene1::ImGuiHierarchy() {
    SceneInstance::ImGuiHierarchy();

    ImGui::Begin("Tool window"); //now append to base class
    ImGui::Checkbox("Show Debug images:", &showDeferredDebug);
    if (showDeferredDebug) {
        float w = 200;
        float t = w * windowSettings->CUR_HEIGHT / windowSettings->CUR_WIDTH;

/*
 shaderFBODebug.use(); //test pos
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, gNormal);

 */
/*
 shaderFBODebug.use();
 glActiveTexture(GL_TEXTURE0);
 glBindTexture(GL_TEXTURE_2D, gPosition);
 */

//renderQuad();
        ImGui::Checkbox("showDebug WorldPos", &isDebugPos);
        ImGui::Checkbox("showDebug Normals", &isDebugNorm);
        ImGui::Checkbox("showDebug Albedo+Spec", &isDebugAlbedo);
/*
ImGui::Image((void *) gPosition,ImVec2( w,t),  ImVec2(0, 1),ImVec2(1, 0), ImVec4(1,1,1,1));
ImGui::Image((void *) gNormal,ImVec2( w,t),  ImVec2(0, 1),ImVec2(1, 0), ImVec4(1,1,1,1));

ImGui::Image((void *) gAlbedoSpec,ImVec2( w,t),  ImVec2(0, 1),ImVec2(1, 0), ImVec4(1,1,1,1));
*/
    }

    ImGui::End();

}

void DeferredScene1::Setup(Camera *cam, GraphicsOptions *graphicsOptions) {

    SceneInstance::Setup(cam, graphicsOptions);
    sceneDescription = "This is a test scene for deferred rendering with many point lights";

//deferred
    shaderGeometryPass = Shader("..\\Assets\\Shaders\\Deferred\\g_buffer.vert",
                                "..\\Assets\\Shaders\\Deferred\\g_buffer.frag");
    shaderLightingPass = Shader("..\\Assets\\Shaders\\Deferred\\deferred_shading.vert",
                                "..\\Assets\\Shaders\\Deferred\\deferred_shading.frag");
    shaderLightBox = Shader("..\\Assets\\Shaders\\Deferred\\deferred_light_box.vert",
                            "..\\Assets\\Shaders\\Deferred\\deferred_light_box.frag");
    shaderFBODebug = Shader("..\\Assets\\Shaders\\Deferred\\debug_fbo.vert",
                            "..\\Assets\\Shaders\\Deferred\\debug_fbo.frag");
    light_shaderCube = Shader("..\\Assets\\Shaders\\Forward\\MultipleLights\\s_light.vert",
                              "..\\Assets\\Shaders\\Forward\\MultipleLights\\s_light.frag");

    ourModel = new Model("../Assets/Models/OwnCube/Cube.obj");
    depth_shader = Shader("..\\Assets\\Shaders\\Debug\\emptyPink.vert", "..\\Assets\\Shaders\\Debug\\emptyPink.frag");

    cube_ObjInstance = new ObjectInstance(*ourModel, depth_shader, " cubeInst", nullptr); // this is correct
    selectableObjInstances.push_back(cube_ObjInstance);


    cube_ObjInstance->SetPos(glm::vec3(1));
//light_shader->setVec3("light.position", l->GetPos());
    depth_shader.setVec3("viewPos", camera->Position);
//also draw the lamp object
    view = camera->GetViewMatrix();


//defered part
// configure g-buffer framebuffer
// ------------------------------
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

// position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
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

// lighting info
// -------------

    int seed = 22;
    srand(seed);
    for (unsigned int i = 0; i < NR_LIGHTS; i++) {
// calculate slightly random offsets
        float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6 - 3.0);
        float yPos = static_cast<float>(((rand() % 100) / 100.0) * 4.0 - 2.4);
        float zPos = static_cast<float>(((rand() % 100) / 100.0) * 4.0 - 3.0);
        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
// also calculate random color
        float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
        seed++;
    }

// shader configuration
// --------------------
    shaderLightingPass.use();

    shaderLightingPass.setInt("gPosition", 0);
    shaderLightingPass.setInt("gNormal", 1);
    shaderLightingPass.setInt("gAlbedoSpec", 2);


    objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
    objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
    objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
    objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
    objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));

}

void DeferredScene1::RenderSceneInstance(Shader *shader) {
//Scene::RenderScene(shader);



// render
// ------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// 1. geometry pass: render scene's geometry/color data into gbuffer
// -----------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 model = glm::mat4(1.0f);
    shaderGeometryPass.use();

    shaderGeometryPass.setMat4("projection", projection);
    shaderGeometryPass.setMat4("view", view);
    int seed = 22;
    for (unsigned int i = 0; i < objectPositions.size(); i++) {
        srand(seed++);
        float xPos = static_cast<float>((rand() % 5) - 2.7);
        float yPos = static_cast<float>((rand() % 5) - 2.7);
        float zPos = static_cast<float>((rand() % 5) - 2.7);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xPos, yPos, zPos));
//model = glm::scale(model, glm::vec3(0.5f));
        shaderGeometryPass.setMat4("model", model);
//cube_ObjInstance->SetPos(objectPositions[i]);
        cube_ObjInstance->SetPos(glm::vec3(xPos, yPos, zPos));
        cube_ObjInstance->SetScale(glm::vec3(0.5f));
        cube_ObjInstance->Render(&shaderGeometryPass, false);
//backpack.Draw(shaderGeometryPass);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

/*shaderFBODebug.use();
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, gPosition);

renderQuad();
//*/
// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
// -----------------------------------------------------------------------------------------------------------------------
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderLightingPass.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
// send light relevant uniforms
    for (unsigned int i = 0; i < lightPositions.size(); i++) {
        shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
        shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
// update attenuation parameters and calculate radius
        const float linear = 0.7f;
        const float quadratic = 1.8f;
        shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
        shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
    }
    shaderLightingPass.setVec3("viewPos", camera->Position);

// finally render quad
    renderQuad();

// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
// ----------------------------------------------------------------------------------
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, 0, windowSettings->CUR_WIDTH,
                      windowSettings->CUR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

// 3. render lights on top of scene
// --------------------------------
    for (unsigned int i = 0; i < lightPositions.size(); i++) {
        light_shaderCube.use();

        light_shaderCube.setVec3("light.position", lightPositions[i]);
        light_shaderCube.setVec3("viewPos", camera->Position);
        light_shaderCube.setMat4("projection", uniforms.projection);
        light_shaderCube.setMat4("view", uniforms.view);
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, lightPositions[i]);
        modelMat = glm::scale(modelMat, glm::vec3(0.25f));
        light_shaderCube.setMat4("model", modelMat);
//dirLight_ObjInstance->Render();
        lightCube.Draw(light_shaderCube, true);
    }

//render either buffers
    if (isDebugPos) {
        shaderFBODebug.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);

        renderQuad();

    }
    if (isDebugNorm) {
        shaderFBODebug.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gNormal);

        renderQuad();

    }
    if (isDebugAlbedo) {
        shaderFBODebug.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        renderQuad();

    }
}


void DeferredScene1::ResizeScene() {

    //TODO CLEAR PREVIOUS
    //del * = gBuffer,gPos
    DeleteSceneBuffers();


    //defered part
// configure g-buffer framebuffer
// ------------------------------
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

// position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA,
                 GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
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
}


void DeferredScene1::DeleteSceneBuffers() {
    glDeleteTextures(1, &gPosition);
    glDeleteTextures(1, &gNormal);
    glDeleteTextures(1, &gAlbedoSpec);
    glDeleteFramebuffers(1, &gBuffer);
    glDeleteRenderbuffers(1, &rboDepth);
}