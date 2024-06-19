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

std::vector<float> LoadTextureData(const std::string& filePath, int width, int height)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return std::vector<float>();
    }

    std::vector<float> data(width * height * 4); // 4 floats per pixel
    file.read(reinterpret_cast<char*>(data.data()), data.size() * sizeof(float));
    /*long long int tes =0;
    int contr = 0;
    while(tes<width*height*4) {
        std::cout << data[tes] <<" "<< data[tes+1] <<" " <<data[tes+2] <<" "<< data[tes+3]
        <<" "<< contr++ <<"\n";
        tes+=4;
    }*/
    return data;
}


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
    glintDeShader = new Shader("..\\Assets\\Shaders\\Experimental\\glint_De.vert",
    "..\\Assets\\Shaders\\Experimental\\glint_De.frag");
    glintHWShader = new Shader("..\\Assets\\Shaders\\Experimental\\glint_WH.vert",
    "..\\Assets\\Shaders\\Experimental\\glint_WH.frag");

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

    //add aditionalShaders to an object Deliot
    shrekModel_ObjInstance->availableShaders.emplace_back(glintDeShader);
    sphereModel_ObjInstance->availableShaders.emplace_back(glintDeShader);
    //add aditionalShaders to an object Wang Howles
    shrekModel_ObjInstance->availableShaders.emplace_back(glintHWShader);
    sphereModel_ObjInstance->availableShaders.emplace_back(glintHWShader);
    shrekModel_ObjInstance->curSelectedShader =glintHWShader; // make it default

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

    //glints3 De part
    glintDeShader->use();
    // almost same as model.cpp implementation
    //glintTexture;
    std::string filename;// = std::string(path);
    //printf((directory + '\\' + filename).c_str());
    filename = "../Assets/Textures/g1.png";
    //printf(filename.c_str());

    //unsigned int textureID;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, glintTexture);
    glGenTextures(1, &glintTexture);
    /*
    std::ifstream input(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = input.tellg();
    input.seekg(0, std::ios::beg);
    char* buffer = new char[size];
    if (input.read(buffer, size)) {
        std::cout << "File read successfully." << std::endl;
    }
    */
    /*
    struct ColorFloat {
        float r, g, b, a;
    };
    std::ifstream input(filename, std::ios::binary);

    if (!input) {
        std::cerr << "Error opening file! "<< filename << std::endl;

    }

    // Determine the size of the file
    input.seekg(0, std::ios::end);
    std::streampos fileSize = input.tellg();
    input.seekg(0, std::ios::beg);

    // Calculate the number of ColorFloat elements
    size_t numPixels = fileSize / sizeof(ColorFloat);

    // Allocate memory to store the texture data
    ColorFloat* textureData = new ColorFloat[numPixels];

    // Read the data into the memory
    input.read(reinterpret_cast<char*>(textureData), fileSize);

    // Don't forget to close the file
    input.close();
   */

    // Load texture data
    std::vector<float> textureData = LoadTextureData("../Assets/Textures/g1.bin", 512  , 512);

    if (textureData.empty())
        std::cout << "File failed to load at path: ../Assets/Textures/g1.bin" << std::endl;



    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;

        std::cout << "no comp: " << nrComponents << " for " << filename <<std::endl;
        std::cout << "dimen " << width <<" "<< height <<std::endl;
        format = GL_RGBA32F; //TODO is this correct

        //glBindTexture(GL_TEXTURE_2D, textureID);

        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
            GL_RGBA,  GL_FLOAT, textureData.data()); //TODO buffer or data or textureData
        //glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    //return textureID;
    //glintTexture = textureID;
    std::cout << "glint de tex id  " << glintTexture;

    //delete[] buffer; //TODO


    // now load dds texure for WangHowles
    char* filePath = "../Assets/Textures/NoiseVolume.dds";

    DDS_TEXTURE* sample_texture = new DDS_TEXTURE();

    bool do_flip = false;
    load_dds_from_file(filePath, &sample_texture, do_flip);
    /*
    glActiveTexture(GL_TEXTURE6);
    GLuint NoiseTextureID;
    glGenTextures(1, &NoiseTextureID);
    glBindTexture(GL_TEXTURE_3D, NoiseTextureID);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Specify the image data
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, width, height,
        128, 0, GL_RED, GL_UNSIGNED_BYTE, sample_texture->pixels);

    glBindTexture(GL_TEXTURE_3D, 0); // Unbind the texture
    */
    delete (sample_texture);

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
    glintZKShader->setVec3("CameraPosition", camera->Position);

    //glints de part
    glintDeShader->use();
    glintDeShader->setInt("_Glint2023NoiseMap", 1);
    glintDeShader->setVec4("Light.Position", glm::vec4(lightPos, 1.0));
    glintDeShader->setVec3("Light.L", glm::vec3(lightInten));
    glintDeShader->setVec3("CameraPosition", camera->Position);

    glintDeShader->setFloat("targetNDF", de_maxNDF);
    glintDeShader->setFloat("maxNDF", de_targetNDF);


    //glintDeShader-setFloat()

    //glint WH Wang Howles
    glintHWShader->use();
    //glintChShader->setInt("Noise", 6); TODO not implemented
    glintHWShader->setFloat("glitterStrength", glitterStrength);

    if(selectedInstance != nullptr)
        glintHWShader->setVec4("lightDir", glm::vec4(lightPos-selectedInstance->GetPos(), 1.0));
    glintHWShader->setVec4("color", glm::vec4(glm::make_vec3(wh_color), 1.0f));

    glintHWShader->setVec3("CameraPosition", camera->Position);
    glintHWShader->setBool("with_anisotropy", with_anisotropy);
    glintHWShader->setFloat("i_sparkle_size", i_sparkle_size);
    glintHWShader->setFloat("i_sparkle_density", i_sparkle_density);
    glintHWShader->setFloat("i_noise_density", i_noise_density);

    glintHWShader->setFloat("i_noise_amount", i_noise_amount);
    glintHWShader->setFloat("i_view_amount", i_view_amount);
    glintHWShader->setFloat("i_time_factor", i_time_factor);
    glintHWShader->setFloat("time_0_X", time_0_X);
    glintHWShader->setInt("Noise", 6);



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

            else if(selectedInstance->curSelectedShader == glintDeShader)
            {
                ImGui::Text("%s",(methodName+ de).c_str());
                ImGui::SliderFloat("maxNDF", &de_maxNDF, 0.00001f, 0.1f);
                ImGui::SliderFloat("targetNDF", &de_targetNDF, 0.00001f, 0.1f);


            }
            else if(selectedInstance->curSelectedShader == glintHWShader)
            {
                ImGui::Text("%s",(methodName+ de).c_str());
                ImGui::SliderFloat("glitterStrength", &glitterStrength, 0.001f, 0.25f);
                ImGui::SliderFloat3("base col", wh_color, 0.1f, 1.0f);

                ImGui::Checkbox("with_anisotropy", &with_anisotropy);
                ImGui::SliderFloat("i_sparkle_size", &i_sparkle_size, 0.001f, 1.0f);
                ImGui::SliderFloat("i_sparkle_density", &i_sparkle_density, 0.001f, 10.0f);
                ImGui::SliderFloat("i_noise_density", &i_noise_density, 0.001f, 14.0f);
                ImGui::SliderFloat("i_noise_amount", &i_noise_amount, 0.001f, 1.0f);
                ImGui::SliderFloat("i_view_amount", &i_view_amount, 0.001f, 10.0f);
                ImGui::SliderFloat("i_time_factor", &i_time_factor, 0.001f, 1.0f);
                ImGui::SliderFloat("time_0_X", &time_0_X, 0.001f, 1.0f);


            }
            else {
                ImGui::Text("No glint shader to show properties");

            }


    }
    else {
        ImGui::Text("%s","Please select an object that has glinty shaders attached in the hierarchy first, it shows parameters only then");
    }

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);
    ImGui::End();
}

void ForwardScene1::DeleteSceneBuffers() {
    glDeleteTextures(1, &texID);

}