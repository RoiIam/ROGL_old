#include <glm/vec3.hpp>//ci ine?

#pragma once

enum LightType {
    Directional, Point, Spot, Other
};

class Light {

public:
    //make enum
    LightType lightType;
    glm::vec3 color = glm::vec3(1);

    Light();

    explicit Light(void *);

    explicit Light(LightType l);

    virtual ~Light();
    //note position is inherently set by Instance so dont do it here
private:
};