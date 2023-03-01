
#include <glm/glm.hpp>//je to spravne?

class Managers {
    public:
        struct Uniforms {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 projection;
        } ;

        struct Transform {
            glm::vec3 pos = { 0.0f, 0.0f, 0.0f };
            glm::vec3 eulerRot = { 0.0f, 0.0f, 0.0f };
            glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
        } ;

    };


//TODO tu bude asi static problem, interal linkage vid https://stackoverflow.com/questions/3698043/static-variables-in-c
static Managers::Uniforms uniforms; //zeby stale tu?

