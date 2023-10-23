#include <glm/glm.hpp>

class Managers {
public:
    struct Uniforms {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
    };

    struct Transform {
        glm::vec3 pos = {0.0f, 0.0f, 0.0f};
        glm::vec3 eulerRot = {0.0f, 0.0f, 0.0f};
        glm::vec3 scale = {1.0f, 1.0f, 1.0f};
    };

    static float deltaTime;
    static int currentSceneIndex;

};

//not static, interal linkage vid https://stackoverflow.com/questions/3698043/static-variables-in-c
//nah, this https://stackoverflow.com/questions/8074174/how-to-share-one-static-variable-with-multiple-translation-unit
extern Managers::Uniforms uniforms;

