#include "Light.h"

    //make enum
    LightType lightType;
    glm::vec3 color = glm::vec3(1);
    Light::Light(void*){

    };
    Light::Light(LightType l){
        lightType = l;
    };
    Light::~Light() = default;
    //note position is inherently set by Instance so dont do it here




