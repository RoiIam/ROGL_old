#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include "../Instances/SceneInstance.h"
#include "../Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "../Lights//Light.h"


class ForwardScene1 : public SceneInstance { //make it public so we can access camera in parent, Scene

public:
    ForwardScene1();

    ~ForwardScene1();

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

    //Glints
    GLuint dicoTex;
    GLuint texID;
    //Glints vars
    float logMicrofacetDensity = 27.0;
    float microfacetRelativeArea = 0.1;//1 for best
    float alpha_x = 0.5;
    float alpha_y = 0.5;
    float lightInten = 8.0;
    // Add lights
public:

    Shader *mesh_shader;

private:
    int enableSpotlight = 1;

    float m = 0.5f;

public:


    //TODO glints
    GLuint loadTex(const std::string &baseName, const unsigned int nlevels, const GLsizei ndists);

    void Setup(Camera *cam) override; // only once before the render loop


    //jsut our testscene needs this
    void SetupShaderMaterial();

    void RenderLights() override;

    void ImGuiHierarchy() override;

    void UIGlintParams();
};
