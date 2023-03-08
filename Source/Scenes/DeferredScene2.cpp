#include "DeferredScene2.h"

void DeferredScene2::Setup(Camera *cam) {
camera = cam;
sceneDescription = "This is a final scene showcasing forward and deferred rendering step by step, check the 'Interactive' UI window";
disableShadows = false;
sponzaModel = new Model(      "..\\Assets\\Models\\Sponza\\sponza.obj");
meshLightShader = new Shader("..\\Assets\\Shaders\\MultipleLights\\mesh.vert","..\\Assets\\Shaders\\MultipleLights\\mesh.frag");
basicShader = new Shader("..\\Assets\\Shaders\\01_SimpleTexture\\1.model_loading.vs", "..\\Assets\\Shaders\\01_SimpleTexture\\1.model_loading.fs");
sponzaObjInstance =  new ObjectInstance(*sponzaModel,*basicShader, "sponza", nullptr);
sponzaObjInstance->SetScale(glm::vec3(0.020f));
selectableObjInstances.push_back(sponzaObjInstance);
SetupGlobalLight();
selectableObjInstances.push_back(dirLight_ObjInstance);
}

void DeferredScene2::SetupForwardLights()
{
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

float DeferredScene2::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void DeferredScene2::SetupSSAO()
{
    disableShadows = true;
    //make sure to run it onlly once
    if(notFirstTime)
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
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
    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    // and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
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
    for (unsigned int i = 0; i < 64; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
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
    for (unsigned int i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                        randomFloats(generator) * 2.0 - 1.0,
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

void DeferredScene2::RenderSceneInstance(Shader* shader)
{
if(demoStage != 3)
{
SceneInstance::RenderSceneInstance(shader);
}
else
{

//NOTE its pointless to raypick, use menu instead
glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glEnable(GL_DEPTH_TEST);



// 1. geometry pass: render scene's geometry/color data into gbuffer
// -----------------------------------------------------------------
glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//projection = uniforms.projection;  //glm::perspective(glm::radians(camera->Zoom), (float)windowSettings->CUR_WIDTH / (float)indowSettings.CUR_HEIGHT, 0.1f, 50.0f);
//glm::mat4 view = uniforms.view;
//glm::mat4 model = glm::mat4(1.0f);
//shaderGeometryPass->use();
//shaderGeometryPass->setMat4("projection", projection);
//shaderGeometryPass->setMat4("view", view);
//glActiveTexture(GL_TEXTURE0);

//selectableObjInstances[0]->Render(shaderGeometryPass,false);
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
shaderSSAO->setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]); // if this breaks, well we ddont run setupssao eh
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


Shader * shaderToUse;
if(enableSSAO)
shaderToUse = shaderLightingPass;
else
shaderToUse = shaderLightingPassOFF;
shaderToUse->use();
// send light relevant uniforms

//from glm::vec3 lightPosView = glm::vec3(camera->GetViewMatrix() * glm::vec4(lightPos, 1.0));
glm::vec3 lightPosView = glm::vec3(uniforms.view * glm::vec4(dirLight_ObjInstance->GetPos(), 0.0));
//shaderLightingPass->setVec3("dirLight.direction", glm::make_vec3(dirLightDirImGui));

shaderToUse->setVec3("light.Position", lightPosView);

glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
shaderToUse->setVec3("light.Color",dirLight_ObjInstance->light->color);
//shaderToUse->setVec3("light.Color",lightColor);

///eee
// Update attenuation parameters
const float linear    = 0.09f;
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
glBlitFramebuffer(0, 0, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, 0, 0, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
glBindFramebuffer(GL_FRAMEBUFFER, 0);
DrawSky();


if(isDebugSSAO) {
shaderFBODebug.use();
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
renderQuad();
}

}
}

void  DeferredScene2::LoadModel(std::string path)
{
    if(path.empty())
        return;
    Model* m = new Model(path,false);

    Shader* s;
    switch (demoStage) {
        case 1:
            s = simpleLights;
            break;
        case 2:
            s = simpleLights;
            break;
        case 3:
            s = shaderLightingPass;
            break;
        default:
            s = basicShader;
            break;

    }
    ObjectInstance* a = new ObjectInstance(*m,*s,"openModel", nullptr);
    selectableObjInstances.push_back(a);
    selectedInstance = a;
}

void DeferredScene2::ImGuiHierarchy()  {
SceneInstance::ImGuiHierarchy();


ImGui::Begin("Interactive");
ImGui::TextWrapped("Welcome to this demo. You see famous Sponza demo in with Phong lighting. By pressing buttons bellow, you will add more graphical features that will make it look even better! "
"Please work through them one by one from the top, otherwise the next button wont be active");

//imgui buttons that handle switching of demo stages and their setup
#pragma region buttonsAndSetup
if(ImGui::Button("Add simple lights") &&demoStage==0)
{
//sponzaObjInstance->SetShader(*simpleLights);
//TODO should be for every object
for(ObjectInstance* oi : selectableObjInstances) {
if (oi->light != nullptr) continue;

oi->SetShader(*simpleLights);
}
//set light properties
//setup Cam
glm::vec3 startPos =glm::vec3(1.333f,1.111f,3.777f);
//camera->SetPosDir(startPos,glm::vec3(0.0, 1.0, 0.0),-157.0f,6.9f);
demoStage=1;
}
if(ImGui::Button("Add directional light with shadows")&&demoStage==1)
{


demoStage=2;
}
ImGui::TextWrapped("NOTE: for shadows to work you need to have checked the box 'Switch shadows on' in the 'Tool Window' UI Window!");
ImGui::TextWrapped("Use 'Sun offset' in the 'Tool Window' UI Window to change sun direction");
if(ImGui::Button("Change to deferred" )&&demoStage==2)
{
SetupSSAO();

demoStage=3;
}
if(ImGui::Button("return back to forward, directional light with shadows ")&&demoStage==3)
{
demoStage=2;
}
ImGui::TextWrapped("Loads model from hard drive and selects it: ");

if (ImGui::Button("Load Model into scene")) {
//OpenSecond = true;
static char* path = FileDialog::Open();
LoadModel(path);
}

#pragma endregion buttons
//not working for now DirectionalLight*  a = dynamic_cast<DirectionalLight*>(dirLight);
switch (demoStage) {
case 0:
break;
case 1:// simple forward lights
ImGui::TextWrapped("Feel free to manipulate  selected lights either by right clicking on them or from the list.");
SetupForwardLights();

ImGui::SliderFloat3("Sun direction", dirLightDirImGui , -1, 1, "%.3f");
//DirectionalLight*  a = dynamic_cast<DirectionalLight*>(dirLight);
//=   static_cast<DirectionalLight>(dirLight_ObjInstance->light);

//a->direction = glm::make_vec3(dirLightDirImGui);
//simpleLights->setVec3("dirLight.direction", a->direction);  //uhh static casts
simpleLights->setVec3("dirLight.direction", glm::make_vec3(dirLightDirImGui));

static ImVec4 color = ImVec4(dirlightCol.x, dirlightCol.y, dirlightCol.z, 1);
ImGui::ColorEdit3("Sun Color", (float*)&color);
if (ImGui::IsItemActive())  // continous edit or IsItemDeactivatedAfterEdit-
// only after i lift mouse
{
dirlightCol.x = color.x;
dirlightCol.y = color.y;
dirlightCol.z = color.z;

// std::cout << color.x << " " << color.y << " " << color.z << std::endl;
}
//daj prvotne nastavenie svetla dir a col + mozno aj kameru na nove miesto,
//    kresli debug pre deferred priamo cez...
simpleLights->setVec3("dirLight.diffuse", dirlightCol);

break;
case 2://dir light with shadows
//simpleLights->setVec3("dirLight.diffuse", dirlightCol);
break;
case 3://deferred pipeline

ImGui::Checkbox("enable SSAO", &enableSSAO);
ImGui::InputInt("AO power",&powerSSAO,1);


//shaderLightingPass->setVec3("Light.direction", );
//dynamic_cast<DirectionalLight*>(dirLight_ObjInstance->light)->direction = glm::vec3(dirLightDirection);
ImGui::Checkbox("showDebug SSAO",&isDebugSSAO);
break;
case 4://deffered+ shadows? or ssao

break;
default:
break;

}

ImGui::End();
}