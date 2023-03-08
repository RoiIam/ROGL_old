#include "Light.h"


Light::~Light() = default;

//make enum
LightType lightType;
glm::vec3 color = glm::vec3(1);

Light::Light() = default;

Light::Light(void *) {};

Light::Light(LightType l) {
    lightType = l;
}
//note position is inherently set by Instance so dont do it here