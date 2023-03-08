#include "SceneInstance.h"


//already included in shaderinstance.h, so how to prevent this type of error? pragma once?
//#include  "Utilities/Managers.h"
//#include  "Utilities/PerfAnalyzer.h"



float a = 0;

SceneInstance::SceneInstance() {
    windowSettings = new WindowSettings();
    stencilShader = new StencilShaderInstance();
};

SceneInstance::~SceneInstance() = default;

void SceneInstance::Setup(Camera *cam) {
    camera = cam;
    selectedHierarchyObj = -1;
    SetupGlobalLight();
}

//render selectable objects with supplied shader
void SceneInstance::RenderObjectsS(Shader *s) {
    for (auto oi: selectableObjInstances) {
        if (oi->light != nullptr)
            continue;
        oi->Render(s, false);
    }
}

//render whole SceneInstance, can be overriden
void SceneInstance::RenderSceneInstance(Shader *s) // later renderer class?
{
    // render
    glClearColor(0.5, 0.5, 0.5, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    PerfAnalyzer::drawcallCount = 0;  // clear counter


    //set in main, we just access them
    view = uniforms.view;
    projection = uniforms.projection;


    DrawSky();

    //first render selected object into stencil
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST); // disable for stencil
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);  // enable writing to the stencil buffer
    // draw the below instead

    if (selectedInstance != nullptr) {
        selectedInstance->GetShader()->use();
        selectedInstance->Render();
    }
    glStencilMask(0x00);  // disable writing to the stencil buffer
    glEnable(GL_DEPTH_TEST); //reenable depth

    // now draw rest of the objects
    RenderObjectsS(s);

    // draw stencil tested ovelay
    if (selectedInstance != nullptr) {

        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);  // disable writing to the stencil buffer
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        // glm::mat4 modelMat = selectedInstance->GetTransformMat();
        glm::vec3 oldScale = selectedInstance->GetScale();
        selectedInstance->SetScale(oldScale * 1.015f);
        stencilShader->SetupMaterial();
        selectedInstance->Render(stencilShader->GetShader(), true);
        selectedInstance->SetScale(oldScale);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
    }

    /*disabled might have a mem leak yep it was this allocation
   //draw also BBox
   glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glm::vec4 BboxDbgCol = glm::vec4 (0.11,0.11,0.11,1);
   basicShader.use();
   basicShader.setMat4("projection", projection);
   basicShader.setMat4("view", view);
   basicShader.setVec4("col", BboxDbgCol);
   glm::mat4 model = glm::mat4 (1);

   if(selectedInstance!=nullptr) {
     model =  selectedInstance->GetTransformMat();//glm::scale(model, glm::vec3(1.002f, 1.002f, 1.002f));	// it's a bit too big for our SceneInstance, so scale it down
     basicShader.setMat4("model", model);
     dont just crate new cube every time, just make it scale next time
   cube =  new DefaultCube(nullptr,selectedInstance->GetModel()->boundMin, selectedInstance->GetModel()->boundMax);
   cube->render(projection, view, model, BboxDbgCol);
   glBindVertexArray(0);
   }
   //return back
   glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
   */
}

//render lights in lightinstances
void SceneInstance::RenderLights() {
    for (ObjectInstance *l: lightObjInstances) {
        light_shader->use();
        light_shader->setVec3("light.position", l->GetPos());
        light_shader->setVec3("viewPos", camera->Position);
        // also draw the lamp object
        //view = camera->GetViewMatrix();
        light_shader->setMat4("projection", uniforms.projection);
        light_shader->setMat4("view", uniforms.view);
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, l->GetPos());
        modelMat = glm::scale(modelMat, glm::vec3(1.02f));
        light_shader->setMat4("model", modelMat);
        dirLight_ObjInstance->Render();
    }
};

void SceneInstance::DrawSky() {
    //printf(":)");

    //glStencilMask(0x00);
    // cubemaps skybox

    // values are equal to depth buffer's content
    camera->SetPosDir(camera->Position, camera->Up, a += 0.01, 0);
    view = glm::mat4(glm::mat3(camera->GetViewMatrix()));  // remove translation from the view matrix

    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when";

    //we need custom view bcs its not dependant on
    cubePrimitive.render(&uniforms.projection, &view);
    glDepthFunc(GL_LESS);  // set depth function back to default was GL_LESS.... // now get it back

    view = camera->GetViewMatrix(); //should be using OGLR::Managers::Uniform now...
}


//create and assign sun directional light
void SceneInstance::SetupGlobalLight() {

    dirLight = new DirectionalLight(LightType::Directional);
    lightCube = new Model("../Assets/Models/LightCube/LightCube.obj");
    light_shader = new Shader("..\\Assets\\Shaders\\Forward\\MultipleLights\\s_light.vert",
                              "..\\Assets\\Shaders\\Forward\\MultipleLights\\s_light.frag");

    dirLight_ObjInstance = new ObjectInstance(*lightCube, *light_shader, "globalDirlight", dirLight);
    dirLight_ObjInstance->SetPos(glm::vec3(2.0f, 5.0f, 0.0f)); // dir Light pos does matter
    auto a = dynamic_cast<DirectionalLight *>(dirLight_ObjInstance->light);
    if (a == nullptr) {
        std::cout << "ERROR cant cast light to dirlight, is it nullptr?";
        return;
    }
    //pozor ak light je null toto segfaultne lebo tiez bude null
    dynamic_cast<DirectionalLight *>(dirLight_ObjInstance->light)->direction
            = glm::vec3(dirLightDirection); //dynamic is better than static but works only with virtual destructor? wut
    lightObjInstances.push_back(dirLight_ObjInstance);
    //its safer to not do it here
    //selectableObjInstances.push_back(dirLight_ObjInstance);add as instance do it later , now its first in  queue //maybe make this as generic fucntion, we need to always do this for lights, opaque and selectable

}

//render gui for selecting, changing parameters
void SceneInstance::ImGuiHierarchy() {

    //create hierarchy and add selection functionality
    ImGui::Begin("Hierarchy");
    int n = 0;

    for (n = 0; n < selectableObjInstances.size(); n++) {
        // bacause of ids being created from the hash of the string, it will only select first occurences, need to assign own ids added ##
        //use std::to_string()
        if (ImGui::Selectable((selectableObjInstances[n]->Name + "##" + std::to_string(n)).c_str(),
                              selectedHierarchyObj == n)) {
            //if already selected, deselect
            if (selectedHierarchyObj == n) {
                selectedHierarchyObj = -1;
                selectedInstance = nullptr;
            } else {
                selectedHierarchyObj = n;
                selectedInstance = selectableObjInstances[n];
                //std::cout << selectableObjInstances[n]->Name << "\n";
            }
        }
    }
    if (selectedHierarchyObj != -1 && selectableObjInstances[selectedHierarchyObj] != nullptr) {
        ImGui::Checkbox("disable render ", &selectableObjInstances[selectedHierarchyObj]->enableRender);
    }
    //debug ImGui::Text("%s%d count %d", "selected \n", selected, n);
    //later draw some selected variables/uniforms
    ImGui::End();

}

//didnt implement
void SceneInstance::LoadSceneInstance(std::string path) {
    //load SceneInstance from file
}


void SceneInstance::renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
