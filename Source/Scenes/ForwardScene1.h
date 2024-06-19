#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include <dds_loader.h>
#include "../Instances/SceneInstance.h"
#include "../Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "../Lights//Light.h"


class ForwardScene1 : public SceneInstance { //make it public so we can access camera in parent, Scene

public:
    ForwardScene1();

    ~ForwardScene1() override;

private:

    bool OpenSecond = false;
    std::string path; // beware making this const and public while having path as constructor :D xd
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 dirlightCol = {0.6f, 0.2f, 0.3f};  // glm::vec3(1.0f);
    float backgroundClearCol[4] = {0.1f, 0.1f, 0.1f, 0.1f};
    bool isOutline = false;

    //models decl
    Model *ourModel;
    Cube cubePrimitive = Cube(nullptr);
    Model *xModel;
    DefaultCube cube; //has no default constructor, either cahnge to pointer or add default empty constr
    Model *shrekModel;
    Model *sphereModel;
    //instances , probably cant do this too?
    ObjectInstance *cube_ObjInstance;// = new ObjectInstance(*ourModel); ehh no please not working, says null model
    ObjectInstance *xModel_ObjInstance;
    ObjectInstance *shrekModel_ObjInstance;
    ObjectInstance *sphereModel_ObjInstance;
    Grass grassplane;

    std::vector<ObjectInstance *> grassInstances;
    std::vector<glm::vec3> vegetation;

    //shaders decl
    // well we cant do this before initializing OpenGL like glfwMakeContextCurrent... we dont have check for that in sandbox rn...
    Shader *ourShader;
    Shader basicShader;
    Shader *pinkDebug;
    Shader *grassShader;
    Shader *glintChShader;
    Shader *glintZKShader;
    Shader *glintDeShader;
    Shader *glintHWShader;

    //Glints
    GLuint dicoTex;
    GLuint texID;
    //Glints vars
    float logMicrofacetDensity = 27.0;
    float microfacetRelativeArea = 0.1;//1 for best
    float alpha_x = 0.5;
    float alpha_y = 0.5;
    float lightInten = 8.0;

    //glints ZK
    float zk_roughness[2] ={0.397f,0.397f} ; //0.6 * 0.024;
    float zk_microRoughness[2]={0.018F,0.018F} ;

    float zk_searchConeAngle = 0.002f;
    float zk_dynamicRange = 1000.0f;
    float zk_variation = 78000.0f;
    float zk_density = 5.e8;
    //glints de
    GLuint glintTexture;
    float de_maxNDF = 0.021f;
    float de_targetNDF = 0.014f;

    // glints HW
    float glitterStrength = 1.0f;
    float wh_color[3]={0.665f,0.596f,0.158f};

    bool    with_anisotropy =  true;
    float i_sparkle_size = 0.012f;
    float i_sparkle_density = 1.37f;
    float i_noise_density = 0.5f;
    float i_noise_amount = 0.43f;
    float i_view_amount = 5.85f;
    float i_time_factor = 1.0f;
    float time_0_X = 1.0f;





    // Add lights
public:

    Shader *mesh_shader;

private:
    int enableSpotlight = 1;

    float m = 0.5f;

public:


    //TODO glints
    GLuint loadTex(const std::string &baseName, const unsigned int nlevels, const GLsizei ndists);

    void Setup(Camera *cam, GraphicsOptions * graphicsOptions) override; // only once before the render loop


    //jsut our testscene needs this
    void SetupShaderMaterial();

    void RenderLights() override;

    void ImGuiHierarchy() override;

    void UIGlintParams();

    void DeleteSceneBuffers();
};