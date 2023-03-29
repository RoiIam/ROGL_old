#include "ForwardScene1.h"


ForwardScene1::ForwardScene1() = default;

ForwardScene1::~ForwardScene1() = default;


void ForwardScene1::Setup(Camera *cam) // override
{
    camera = cam;
    sceneDescription = "This is a test scene for forward rendering with support for shadows";
//shaders load, setup
// uhh "//" both  // \\ and // /work, but in textures it fs up while using
    ourShader = new Shader("..\\Assets\\Shaders\\Forward\\01_SimpleTexture\\1.model_loading.vs",
                           "..\\Assets\\Shaders\\Forward\\01_SimpleTexture\\1.model_loading.fs");
    pinkDebug = new Shader("..\\Assets\\Shaders\\Debug\\emptyPink.vert",
                           "..\\Assets\\Shaders\\Debug\\emptyPink.frag");
    grassShader = new Shader("..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.vert",
                             "..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.frag");

    mesh_shader = new Shader("..\\Assets\\Shaders\\Forward\\MultipleLights\\mesh.vert",
                             "..\\Assets\\Shaders\\Forward\\MultipleLights\\mesh.frag");  // light compatible

    basicShader = Shader("..\\Assets\\Shaders\\Forward\\basic.vert",
                         "..\\Assets\\Shaders\\Forward\\basic.frag");

//models load, setup
    ourModel = new Model("../Assets/Models/OwnCube/Cube.obj");
    xModel = new
            Model("../Assets/Models/OwnCube/Cube.gltf", pinkDebug);

    shrekModel = new Model(
            "../Assets/Models/Shrek/Shrek_mod.gltf"); // src https://sketchfab.com/3d-models/shrek-ee9fbba7e7a841dbb817cc6cec678355

    SetupGlobalLight();

    cube_ObjInstance = new ObjectInstance(*ourModel, *ourShader, "cube",
                                          nullptr); // to assign shader it has to be already created or it will be null!!
    xModel_ObjInstance = new ObjectInstance(*xModel);
    xModel_ObjInstance->Name = "xModelName";
    shrekModel_ObjInstance = new ObjectInstance(*shrekModel, *ourShader, "shrek", nullptr);

    grassplane = Grass();
    cube = DefaultCube(nullptr, xModel->boundMin, xModel->boundMax);

    selectableObjInstances.push_back(cube_ObjInstance);
    selectableObjInstances.push_back(shrekModel_ObjInstance);
    shrekModel_ObjInstance->SetPos(glm::vec3(2, 0, 2));
    selectableObjInstances.push_back(xModel_ObjInstance);

    cube_ObjInstance->SetPos(glm::vec3(5.0f, 0.0f, 0.0f));

//hold for rendering
    vegetation.push_back(glm::vec3(-1.5f, 5.0f, -0.48f));
    vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
    vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
    vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
    vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

// instances preparation
    for (unsigned int i = 0; i < vegetation.size(); i++) {
        ObjectInstance *tmp = new ObjectInstance(grassplane, *grassShader, "grass_" + std::to_string(i), nullptr);

        tmp->SetPos(vegetation[i]);
        grassInstances.push_back(tmp);
        selectableObjInstances.push_back(tmp);
    }
//now add lights
    selectableObjInstances.push_back(dirLight_ObjInstance);
}

void ForwardScene1::SetupShaderMaterial() {
    ourShader->use();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(00.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));  // it's a bit too big for our scene, so scale it down
    model = glm::translate(model, cube_ObjInstance->GetPos());//just test of setpos
    ourShader->setMat4("model", model);
    cube_ObjInstance->SetRot(glm::vec3(0.0f, 0.0f, 1.0f));
    cube_ObjInstance->SetDeg(00.0f);
}

void ForwardScene1::RenderObjects(Shader *shader, bool simple) //override
{
    view = uniforms.view;
    projection = uniforms.projection;
    // render
    glClearColor(backgroundClearCol[0], backgroundClearCol[1],
                 backgroundClearCol[2], backgroundClearCol[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    PerfAnalyzer::drawcallCount = 0;  // clear counter

    // draw stencil
    glDisable(GL_DEPTH_TEST); // disable for stencil
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);  // enable writing to the stencil buffer

    if (selectedInstance != nullptr) {
        selectedInstance->GetShader()->use();
        selectedInstance->Render();
    }
    glStencilMask(0x00);  // disable writing to the stencil buffer
    glEnable(GL_DEPTH_TEST); //reenable depth

    SetupShaderMaterial(); // repalce code above
    cube_ObjInstance->Render(ourShader, false);


    // draw light thingy
    mesh_shader->use();
    model = glm::mat4(1.0f);  // same , below
    // m += 0.008f; dont move
    model = glm::translate(model, glm::vec3(-5.0f, 0.0f, 0.5f));
    model = glm::scale(
            model, glm::vec3(1.0f, 1.0f, 1.0f));  // lets try setting it here
    unsigned int transformLoc = glGetUniformLocation(mesh_shader->ID, "model");
    mesh_shader->setMat4("model", model);
    mesh_shader->setVec3("light.position", lightPos);
    mesh_shader->setVec3("viewPos", camera->Position);
    // light properties
    mesh_shader->setVec3("light.ambient", 0.05f, 0.05f, 0.1f);
    mesh_shader->setVec3("light.diffuse", 0.5f, 0.5f, 0.7f);
    mesh_shader->setVec3("light.specular", 0.7f, 0.7f, 0.7f);
    // material properties
    mesh_shader->setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    mesh_shader->setFloat("material.shininess", 32.0f);

    mesh_shader->setVec3("dirLight.direction",
                         static_cast<DirectionalLight>(dirLight_ObjInstance->light).direction);  //uhh static casts


    //  dirlightCol use instead of hardcoded vars
    mesh_shader->setVec3("dirLight.ambient",
                         dirlightCol * 0.5f);  // 0.6f, 0.2f, 0.3f //tone it down
    mesh_shader->setVec3("dirLight.diffuse", dirlightCol);   // 0.6f, 0.2f, 0.3f
    mesh_shader->setVec3("dirLight.specular", dirlightCol);  // 0.7f, 0.7f, 0.7f

    // spotLight
    // enable
    mesh_shader->setInt("spotLight.Enabled", false);  // camera.enableSpotlight
    mesh_shader->setVec3("spotLight.position", camera->Position);
    mesh_shader->setVec3("spotLight.direction", camera->Front);
    mesh_shader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    mesh_shader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    mesh_shader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    mesh_shader->setFloat("spotLight.constant", 1.0f);
    mesh_shader->setFloat("spotLight.linear", 0.09);
    mesh_shader->setFloat("spotLight.quadratic", 0.032);
    mesh_shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    mesh_shader->setFloat("spotLight.outerCutOff",
                          glm::cos(glm::radians(15.0f)));
    mesh_shader->setVec3("testPointLight.position", lightPos);
    mesh_shader->setFloat("testPointLight.constant", 1.0f);
    mesh_shader->setFloat("testPointLight.linear", 0.09f);
    mesh_shader->setFloat("testPointLight.quadratic", 0.032f);
    mesh_shader->setVec3("testPointLight.ambient", 0.2f, 0.2f, 0.3f);
    mesh_shader->setVec3("testPointLight.diffuse", 0.2f, 0.2f, 0.7f);
    mesh_shader->setVec3("testPointLight.specular", 0.7f, 0.7f, 0.7f);

    mesh_shader->setMat4("projection", projection);
    mesh_shader->setMat4("view", view);

    // maybe you cant do this after you calculated all previous light calcs....
    xModel_ObjInstance->SetPos(glm::vec3(-5.0f, 0.0f, 0.5)); //mesh_shader.setMat4("model", model);
    xModel->Draw(*mesh_shader, false);

    ourShader->use();
    ourShader->setMat4("projection", uniforms.projection);
    ourShader->setMat4("view", uniforms.view);
    // render the loaded obj model , we also write to stencil
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));  // it's a bit too big for our scene, so scale it down
    //model = glm::translate(model,glm::vec3(0.0f,0.0f,0.0f));  dont use this as we set position inside obj instance // translate it down so it's at the center of the scene
    model = glm::translate(model, shrekModel_ObjInstance->GetPos());//just test of setpos
    ourShader->setMat4("model", model);
    shrekModel_ObjInstance->Render();

    // be sure to activate shader when setting uniforms/drawing objects
    light_shader->use();
    light_shader->setVec3("light.position", lightPos);
    light_shader->setVec3("viewPos", camera->Position);
    // also draw the lamp object
    light_shader->setMat4("projection", projection);
    light_shader->setMat4("view", view);
    glm::mat4 modelMat = glm::mat4(1.0f);
    modelMat = glm::translate(modelMat, lightPos);
    // modelMat = glm::scale(modelMat, glm::vec3(1.2f));
    light_shader->setMat4("model", modelMat);


    // Draw cubePrimitive // cubemaps skybox
    //  don't forget to enable shader before setting uniforms
    ourShader->use();
    view = camera->GetViewMatrix();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);


    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when
    // values are equal to depth buffer's content
    view = glm::mat4(glm::mat3(camera->GetViewMatrix()));  // remove translation from the view matrix
    cubePrimitive.render(&projection, &view);
    glDepthFunc(GL_LESS);  // set depth function back to default was GL_LESS.... // now get it back

    view = camera->GetViewMatrix(); //should be using OGLR::Managers::Uniform now...


    // draw transparent grass
    //aways disable backface culling, ignore global setting
    glDisable(GL_CULL_FACE);
    grassShader->use();
    grassShader->setInt("texture1", 0);
    grassShader->setMat4("projection", projection);
    grassShader->setMat4("view", view);
    for (unsigned int i = 0; i < grassInstances.size(); i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, vegetation[i]);
        // update.. and draw does it in instance >render()
        grassInstances[i]->SetPos(vegetation[i]);
        grassInstances[i]->Render();
    }
    glEnable(GL_CULL_FACE);

    glm::mat4 model2 = glm::mat4(1.0f);
    model2 =
            glm::rotate(model2, glm::radians(00.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model2 = glm::scale(
            model2,
            glm::vec3(1.0f, 1.0f,
                      1.0f));  // it's a bit too big for our scene, so scale it down
    model2 = glm::translate(model2, vegetation[0]);


}

void ForwardScene1::RenderLights() {//override
    SceneInstance::RenderLights();
}