#define TINYEXR_IMPLEMENTATION

#include "ForwardScene1.h"
//for glints_ch
#define TINYEXR_USE_MINIZ 0

#include "zlib.h"
#include "tinyexr.h"

ForwardScene1::ForwardScene1() = default;

ForwardScene1::~ForwardScene1()
{
    DeleteSceneBuffers();
};


void ForwardScene1::Setup(Camera *cam, GraphicsOptions *graphicsOptions) // override
{
    graphicsOptions->rendererType = GraphicsOptions::RendererType::forward;
    graphicsOptions->enableWater = false;
    graphicsOptions->enableShadows = false;
    SceneInstance::Setup(cam, graphicsOptions);

    sceneDescription = "This is a test scene for forward rendering with support for shadows."
                       "Press ESC to show/hide cursor.";
//shaders load, setup
// uhh "//" both  // \\ and // /work, but in textures it fs up while using
    ourShader = new Shader("..\\Assets\\Shaders\\Forward\\01_SimpleTexture\\1.model_loading.vs",
                           "..\\Assets\\Shaders\\Forward\\01_SimpleTexture\\1.model_loading.fs");
    pinkDebug = new Shader("..\\Assets\\Shaders\\Debug\\emptyPink.vert", "..\\Assets\\Shaders\\Debug\\emptyPink.frag");
    grassShader = new Shader("..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.vert",
                             "..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.frag");

    mesh_shader = new Shader("..\\Assets\\Shaders\\Forward\\MultipleLights\\mesh.vert",
                             "..\\Assets\\Shaders\\Forward\\MultipleLights\\mesh.frag");  // light compatible

    basicShader = Shader("..\\Assets\\Shaders\\Forward\\basic.vert", "..\\Assets\\Shaders\\Forward\\basic.frag");

    glintChShader = new Shader("..\\Assets\\Shaders\\Experimental\\glint_ch.vert",
                               "..\\Assets\\Shaders\\Experimental\\glint_ch.frag");
    glintZKShader = new Shader("..\\Assets\\Shaders\\Experimental\\glint_ZK.vert",
                                   "..\\Assets\\Shaders\\Experimental\\glint_ZK.frag");

//models load, setup
    ourModel = new Model("../Assets/Models/OwnCube/Cube.obj");
    xModel = new Model("../Assets/Models/OwnCube/Cube.gltf", pinkDebug);

    shrekModel = new Model(
            "../Assets/Models/Shrek/Shrek_mod.gltf"); // src https://sketchfab.com/3d-models/shrek-ee9fbba7e7a841dbb817cc6cec678355

    sphereModel = new Model("../Assets/Models/sphere/sphere.obj");


    cube_ObjInstance = new ObjectInstance(*ourModel, *ourShader, "cube",
                                          nullptr); // to assign shader it has to be already created or it will be null!!
    xModel_ObjInstance = new ObjectInstance(*xModel);
    xModel_ObjInstance->Name = "xModelName";

    shrekModel_ObjInstance = new ObjectInstance(*shrekModel, *glintChShader, "shrek", nullptr);
    sphereModel_ObjInstance = new ObjectInstance(*sphereModel, *glintChShader, "sphere", nullptr);

    grassplane = Grass();
    cube = DefaultCube(nullptr, xModel->boundMin, xModel->boundMax);

    //selectableObjInstances.push_back(cube_ObjInstance);
    selectableObjInstances.push_back(shrekModel_ObjInstance);
    selectableObjInstances.push_back(sphereModel_ObjInstance);
    shrekModel_ObjInstance->SetPos(glm::vec3(1.2f, 0.9f, 0.35f));
    shrekModel_ObjInstance->SetScale(glm::vec3(0.1));

    //add aditionalShaders to an object
    shrekModel_ObjInstance->availableShaders.emplace_back(glintZKShader);
    sphereModel_ObjInstance->availableShaders.emplace_back(glintZKShader);


    //selectableObjInstances.push_back(xModel_ObjInstance);

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
        //selectableObjInstances.push_back(tmp);
    }
//now add lights
    selectableObjInstances.push_back(dirLight_ObjInstance);


    //GLuint dicoTex = Texture::loadMultiscaleMarginalDistributions(MEDIA_PATH+std::string("dictionary/dict_16_192_64_0p5_0p02"), numberOfLevels, numberOfDistributionsPerChannel);
    dicoTex = loadTex("..//Assets//dict//dict_16_192_64_0p5_0p02", 16, 64);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_1D_ARRAY, dicoTex);

    SetupShaderMaterial();

}


GLuint ForwardScene1::loadTex(const std::string &baseName, const unsigned int nlevels, const GLsizei ndists) {
    const char *err = nullptr;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_1D_ARRAY, texID);

    GLint width;
    GLint height;
    GLsizei layerCount = ndists * nlevels;
    GLsizei mipLevelCount = 1;

    // Load the first one to get width/height
    std::string texName = baseName + "_0000" + "_" + "0000" + ".exr";
    float *data;
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
    ourShader->setMat4("projection", uniforms.projection);
    ourShader->setMat4("view", uniforms.view);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(00.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::translate(model, cube_ObjInstance->GetPos());//just test of setpos
    ourShader->setMat4("model", model);
    //printf("SetupShaderMaterial\n");

    //glints part
    glintChShader->use();
    glm::vec3 lightPos = dirLight_ObjInstance->GetPos();

    glintChShader->setVec4("Light.Position", glm::vec4(lightPos, 1.0));
    //glintChShader->setVec4("Light.Position", glm::vec4(dirLight_ObjInstance->GetPos(), 1.0));
    glintChShader->setVec3("Light.L", glm::vec3(lightInten));

    glintChShader->setFloat("Material.Alpha_x", alpha_x);
    glintChShader->setFloat("Material.Alpha_y", alpha_y);
    glintChShader->setFloat("Material.LogMicrofacetDensity", logMicrofacetDensity);

    glintChShader->setFloat("Dictionary.Alpha", 0.5);
    glintChShader->setInt("Dictionary.N", 192); //64*3
    glintChShader->setInt("Dictionary.NLevels", 16);
    glintChShader->setInt("Dictionary.Pyramid0Size", 1 << (15));//16 - 1

    glintChShader->setVec3("CameraPosition", camera->Position);
    glintChShader->setFloat("MicrofacetRelativeArea", microfacetRelativeArea);
    glintChShader->setFloat("MaxAnisotropy", 8);

    glintChShader->setInt("DictionaryTex", 8);//has to be set to GL_TEXTURE XXX when glBindTexture for array dicoTex
    //dirLight_ObjInstance->SetPos(lightPos);
    //dirLight_ObjInstance->SetScale(glm::vec3(0.3f));


    //glints2
    //glints part
    glintZKShader->use();
    lightPos = dirLight_ObjInstance->GetPos();

    glintZKShader->setVec4("Light.Position", glm::vec4(lightPos, 1.0));
    //glintChShader->setVec4("Light.Position", glm::vec4(dirLight_ObjInstance->GetPos(), 1.0));
    glintZKShader->setVec3("Light.L", glm::vec3(lightInten));

    glintZKShader->setVec2("Material.roughness", glm::make_vec2(zk_roughness));
    glintZKShader->setVec2("Material.microRoughness", glm::make_vec2(zk_microRoughness));
    glintZKShader->setFloat("Material.searchConeAngle", zk_searchConeAngle);
    glintZKShader->setFloat("Material.variation", zk_variation);
    glintZKShader->setFloat("Material.dynamicRange", zk_dynamicRange);
    glintZKShader->setFloat("Material.density", zk_density);
    /*
    glintZKShader->setVec2("Material.roughness", glm::vec2(0.6f,0.6f));
    glintZKShader->setVec2("Material.microRoughness", glm::vec2(0.6f*0.024f,0.6*0.024f));
    glintZKShader->setFloat("Material.searchConeAngle", 0.01f);
    glintZKShader->setFloat("Material.variation", 100);
    glintZKShader->setFloat("Material.dynamicRange", 50000);
    glintZKShader->setFloat("Material.density", 5.e8);
    */



    glintZKShader->setVec3("CameraPosition", camera->Position);



}

void ForwardScene1::RenderLights() {
    SceneInstance::RenderLights();
}

void ForwardScene1::ImGuiHierarchy() {
    SceneInstance::ImGuiHierarchy();

    UIGlintParams();
}

void ForwardScene1::UIGlintParams() {

    std::string ch= "cherm";
    std::string zk= "zirrKapl";
    std::string de= "Deliot";

    ImGui::Begin((std::string("Glint Parameters")+ ch).c_str());

    if(selectedInstance != nullptr) {

        if(ImGui::Button("recompile Shader")) {
            selectedInstance->curSelectedShader->Recompile();
        }
        std::string methodName = "selected Method: ";

        //glm::vec3 lightPos = dirLight_ObjInstance->GetPos();
        //float v[] = {lightPos.x, lightPos.y, lightPos.z};
        ImGui::DragFloat("Light Inten", &lightInten, 0.25f, 0.0, 100);


            if(selectedInstance->curSelectedShader == glintChShader) {
                ImGui::Text("%s", (methodName+ ch).c_str());

                ImGui::SliderFloat("Roughness X", &alpha_x, 0.01f, 1.0f);
                ImGui::SliderFloat("Roughness Y", &alpha_y, 0.01f, 1.0f);
                ImGui::SliderFloat("Log microfacet density", &logMicrofacetDensity, 15.f, 40.f);
                ImGui::SliderFloat("Microfacet relative area", &microfacetRelativeArea, 0.01f, 1.f);

            }
            else if(selectedInstance->curSelectedShader == glintZKShader) {
                ImGui::Text("%s",(methodName+ zk).c_str());

                ImGui::SliderFloat2("Roughness", zk_roughness, 0.001f, 1.0f);
                ImGui::SliderFloat2("Micro Roughness", zk_microRoughness, 0.001f, 0.1f);
                ImGui::SliderFloat("Search Cone Angle", &zk_searchConeAngle, 0.001f, 0.1f);
                ImGui::SliderFloat("Variation", &zk_variation, 20000.0f, 100000.0f);
                ImGui::SliderFloat("DynamicRange", &zk_dynamicRange, 10.0f, 1000.0f);
                ImGui::SliderFloat("Density", &zk_density, 1.e8, 7.e8);

            }
            /*
            else if(selectedInstance->curSelectedShader == glintDeShader)
            {
                ImGui::Text("%s",(methodName+ de).c_str());

            }*/
            else {
                ImGui::Text("No glint shader to show properties");

            }


        /*
        if(shrekModel_ObjInstance->curShaderIndex==0) {
            //chermain
        }
        else if(shrekModel_ObjInstance->curShaderIndex ==1) {
            //zirr
        }
        else if(shrekModel_ObjInstance->curShaderIndex ==2) {
            //deliot
        }
        */

    }
    else {
        ImGui::Text("%s","Please select shrek character, it only works there for now");
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);
    ImGui::End();
}

void ForwardScene1::DeleteSceneBuffers() {
    glDeleteTextures(1, &texID);

}