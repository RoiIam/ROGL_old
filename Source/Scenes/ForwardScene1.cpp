#define TINYEXR_IMPLEMENTATION
#include "ForwardScene1.h"
//for glints_ch
#define TINYEXR_USE_MINIZ 0
#include "zlib.h"
#include "tinyexr.h"

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

    glintChShader = new Shader("..\\Assets\\Shaders\\Experimental\\glint_ch.vert",
                               "..\\Assets\\Shaders\\Experimental\\glint_ch.frag");

//models load, setup
    ourModel = new Model("../Assets/Models/OwnCube/Cube.obj");
    xModel = new
            Model("../Assets/Models/OwnCube/Cube.gltf", pinkDebug);

    shrekModel = new Model(
            "../Assets/Models/Shrek/Shrek_mod.gltf"); // src https://sketchfab.com/3d-models/shrek-ee9fbba7e7a841dbb817cc6cec678355

    sphereModel = new Model(
            "../Assets/Models/sphere/sphere.obj");
    SetupGlobalLight();

    cube_ObjInstance = new ObjectInstance(*ourModel, *ourShader, "cube",
                                          nullptr); // to assign shader it has to be already created or it will be null!!
    xModel_ObjInstance = new ObjectInstance(*xModel);
    xModel_ObjInstance->Name = "xModelName";
    //TODO glints
    shrekModel_ObjInstance = new ObjectInstance(*shrekModel, *glintChShader, "shrek", nullptr);
    sphereModel_ObjInstance = new ObjectInstance(*sphereModel, *glintChShader, "sphere", nullptr);

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


    //TODO glints
    /*
    loadMultiscaleMarginalDistributions("glint_dict.raw").then(t => {
            dicoTex = Texture2dArray();
            dicoTex.bind();
            gl.texImage3D(gl.TEXTURE_2D_ARRAY, 0, gl.RGB16F, t.w, 1, t.nbl * t.nbd, 0, gl.RGB, gl.FLOAT, t.data)
            gl.texParameteri(gl.TEXTURE_2D_ARRAY, gl.TEXTURE_MIN_FILTER,gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D_ARRAY, gl.TEXTURE_MAG_FILTER,gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D_ARRAY, gl.TEXTURE_WRAP_S,gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D_ARRAY, gl.TEXTURE_WRAP_T,gl.REPEAT);
            unbind_texture2dArray();
    */

    //example how they did it on the github real_time_glint/sceneglint.cpp L40
    //GLuint dicoTex = Texture::loadMultiscaleMarginalDistributions(MEDIA_PATH+std::string("dictionary/dict_16_192_64_0p5_0p02"), numberOfLevels, numberOfDistributionsPerChannel);
    dicoTex=loadTex("..//Assets//dict//dict_16_192_64_0p5_0p02", 16, 64);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_1D_ARRAY, dicoTex);
}


GLuint ForwardScene1::loadTex(const std::string& baseName, const unsigned int nlevels, const GLsizei ndists)
{
    const char* err = nullptr;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_1D_ARRAY, texID);

    GLint width;
    GLint height;
    GLsizei layerCount = ndists * nlevels;
    GLsizei mipLevelCount = 1;

    // Load the first one to get width/height
    std::string texName = baseName + "_0000" + "_" + "0000" + ".exr";
    float* data;
    bool ret = LoadEXR(&data, &width, &height, texName.c_str(), &err);//new tinyexr
    //use this for older tinyexr, used in paper
    //bool ret = exrio::LoadEXRRGBA(&data, &width, &height, texName.c_str(), err);
    int nrComponents;
    //stbi_load(texName.c_str(), &width, &height, &nrComponents, 0);
    //if (!ret) {
    if (!data) {
        exit(-1);
    }

    // Allocate the storage
    glTexStorage2D(GL_TEXTURE_1D_ARRAY, mipLevelCount, GL_RGB16F, width, layerCount);

    // Upload pixel data
    // The first 0 refers to the mipmap level (level 0)
    // The following zero refers to the x offset in case you only want to specify a subrectangle
    // The final 0 refers to the layer index offset (we start from index 0)
    glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, 0, width, 1, GL_RGBA, GL_FLOAT, data);

    free(data);

    // Load the other 1D distributions
    for (int l = 0; l < nlevels; ++l) {
        std::string index_level;
        if (l < 10)
            index_level = "000" + std::to_string(l);
        else if (l < 100)
            index_level = "00" + std::to_string(l);
        else if (l < 1000)
            index_level = "0" + std::to_string(l);
        else
            index_level = std::to_string(l);

        for (int i = 0; i < ndists; i++) {

            if (l == 0 && i == 0) continue;

            std::string index_dist;
            if (i < 10)
                index_dist = "000" + std::to_string(i);
            else if (i < 100)
                index_dist = "00" + std::to_string(i);
            else if (i < 1000)
                index_dist = "0" + std::to_string(i);
            else
                index_dist = std::to_string(i);

            texName = baseName + "_" + index_dist + "_" + index_level + ".exr";
            //bool ret = exrio::LoadEXRRGBA(&data, &width, &height, texName.c_str(), err);

            /*bool ret = stbi_load(texName.c_str(), &width, &height, &nrComponents, 0);
            std::cout << "texname "<< texName.c_str() << std::endl;
            std::cout.flush();
             we cant use stbi for exr format, we need tinyexr https://github.com/syoyo/tinyexr
             */

            int ret = LoadEXR(&data, &width, &height, texName.c_str(), &err); //new tinyexr
            //use this for older tinyexr, used in paper
            //bool ret = exrio::LoadEXRRGBA(&data, &width, &height, texName.c_str(), err);

            if (ret != TINYEXR_SUCCESS) {
                if (err) {
                    /*if(stbi_failure_reason()) {
                        std::cout << stbi_failure_reason();
                    }*/
                    std::cout << "huh " << texName.c_str() << std::endl;
                    std::cout.flush();
                    fprintf(stderr, "ERR : %s\n", err);
                    exit(-1);
                }
            }
            glTexSubImage2D(GL_TEXTURE_1D_ARRAY, 0, 0, l * ndists + i, width, 1, GL_RGBA, GL_FLOAT, data);
            free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    return texID;
}

void ForwardScene1::SetupShaderMaterial() {
    ourShader->use();
    ourShader->setMat4("projection", projection);
    ourShader->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(00.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::translate(model, cube_ObjInstance->GetPos());//just test of setpos
    ourShader->setMat4("model", model);
    cube_ObjInstance->SetRot(glm::vec3(0.0f, 0.0f, 1.0f));
    cube_ObjInstance->SetDeg(00.0f);

}

void ForwardScene1::drawShrek()
{
    //TODO part for glints
    //TODO only if shader is glint_ch

    ImGui::Begin("Parameters");

    ImGui::SliderFloat("Roughness X", &alpha_x, 0.01f, 1.0f);
    ImGui::SliderFloat("Roughness Y", &alpha_y, 0.01f, 1.0f);
    ImGui::SliderFloat("Log microfacet density", &logMicrofacetDensity, 15.f, 40.f);
    ImGui::SliderFloat("Microfacet relative area", &microfacetRelativeArea, 0.01f, 1.f);
    float v[]={lightPos.x,lightPos.y,lightPos.z};
    ImGui::DragFloat3("Light Pos", v,0.15f, -15.0f, 15.f);
    lightPos = glm::make_vec3(v);
    ImGui::DragFloat("Light Inten", &lightInten,0.25f,0.0,100);


    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();


    glintChShader->use();
    model = glm::translate(model,glm::vec3(0));
    model = glm::scale(model,glm::vec3(0.1));

    glintChShader->setMat4("model", model);
    glintChShader->setMat4("view", uniforms.view);
    glintChShader->setMat4("projection", uniforms.projection);

    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_1D_ARRAY, dicoTex);

    glintChShader->setVec4("Light.Position", glm::vec4(lightPos,1.0));
    glintChShader->setVec3("Light.L", glm::vec3(lightInten));

    glintChShader->setFloat("Material.Alpha_x", alpha_x);
    glintChShader->setFloat("Material.Alpha_y", alpha_y);
    glintChShader->setFloat("Material.LogMicrofacetDensity", logMicrofacetDensity);

    glintChShader->setFloat("Dictionary.Alpha", 0.5);
    glintChShader->setInt("Dictionary.N", 192); //64*3
    glintChShader->setInt("Dictionary.NLevels", 16);
    glintChShader->setInt("Dictionary.Pyramid0Size",  1 << (15));//16 - 1

    glintChShader->setVec3("CameraPosition", camera->Position);
    glintChShader->setFloat("MicrofacetRelativeArea", microfacetRelativeArea);
    glintChShader->setFloat("MaxAnisotropy", 8);

    //uniform mediump sampler2DArray DictionaryTex;
    //glintChShader->setFloat("DictionaryTex", XXX);
    glintChShader->setInt("DictionaryTex", 0);//has to be zero? ok
    dirLight_ObjInstance->SetPos(lightPos);
    dirLight_ObjInstance->SetScale(glm::vec3(0.3f));

    /*
     from webgl implementation,default values
     var mesh_rend = null;
    var prg = null;
    var dicoTex = null;
    var lightPos = Vec4(5, 5, 5, 1);
    var maxAnisotropy = 8;
    var microfacetRelativeArea = 1;
    var alpha_x = 0.5
    var alpha_y = 0.5;
    var logMicrofacetDensity = 27;
    var lastTime=0;
    var beta;
    var objRot;

     Uniforms.Dictionary.Alpha = 0.5;
    const numberOfLevels = 16;
    const numberOfDistributionsPerChannel = 64;
    Uniforms.Dictionary.N = numberOfDistributionsPerChannel * 3;
    Uniforms.Dictionary.NLevels = numberOfLevels;
     Uniforms.Dictionary.Pyramid0Size = 1 << (numberOfLevels - 1);
     * */
    shrekModel->Draw(*glintChShader, false);
    //sphereModel->Draw(*glintChShader, false);

}

void ForwardScene1::RenderObjects(Shader *shader, bool simple)
{
    view = uniforms.view;
    projection = uniforms.projection;
    // render
    glClearColor(backgroundClearCol[0], backgroundClearCol[1],
                 backgroundClearCol[2], backgroundClearCol[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    PerfAnalyzer::drawcallCount = 0;  // clear counter

    /*
    // draw stencil not correct, draws grass as stencil
    std::cout << "Draw Stencil\n";
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
    */
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
    //std::cout<< "uniV FS\n" << glm::to_string(uniforms.view);
    // render the loaded obj model , we also write to stencil
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));  // it's a bit too big for our scene, so scale it down
    //model = glm::translate(model,glm::vec3(0.0f,0.0f,0.0f));  dont use this as we set position inside obj instance // translate it down so it's at the center of the scene
    model = glm::translate(model, shrekModel_ObjInstance->GetPos());//just test of setpos
    //ourShader->setMat4("model", model);

    drawShrek();

    //shrekModel_ObjInstance->Render();

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
    //std::cout << "grass Test\n";//its ddoesnt even use this

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