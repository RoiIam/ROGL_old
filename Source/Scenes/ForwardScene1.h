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
    //instances , probably cant do this too?
    ObjectInstance *cube_ObjInstance;// = new ObjectInstance(*ourModel); ehh no please not working, says null model
    ObjectInstance *xModel_ObjInstance;
    ObjectInstance *shrekModel_ObjInstance;
    Grass grassplane;

    std::vector<ObjectInstance *> grassInstances;
    std::vector<glm::vec3> vegetation;

    //shaders decl
    // well we cant do this before initializing OpenGL like glfwMakeContextCurrent... we dont have check for that in sandbox rn...
    Shader *ourShader;
    Shader basicShader;
    Shader *pinkDebug;
    Shader *grassShader;

    // Add lights
public:

    Shader *mesh_shader;

private:
    glm::vec3 lightPos = glm::vec3(0.0f, 2.0f, 3.0f);
    glm::vec3 dirLightPos = glm::vec3(0.7f, -0.4f, -0.1f);
    int enableSpotlight = 1;

    float m = 0.5f;

public:
    void Setup(Camera *cam) override; // only once before the render loop


    //jsut our testscene needs this
    void SetupShaderMaterial();

    void RenderObjects(Shader *shader, bool simple);//maybe override?
    void RenderLights() override;
};
