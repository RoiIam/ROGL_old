#ifndef ROGL_WATER_H
#define ROGL_WATER_H

#include "Quad.h"
#include "Camera/Camera.h"
class Water : public Quad {



public:

    Water(Shader *shader, Camera * cam);

    unsigned int reflectionTexture;
    unsigned int refractionTexture;
    unsigned int dudvTexture;
    unsigned int normalMapTexture;





    glm::vec2 moveFactor = glm::vec2(0);
    glm::vec2 waveSpeed = glm::vec2(0.0016f,0.0009f);

    Camera * cam;

    void Draw(Shader &shader, bool simple) override;
    //void SetTextures();
};


#endif //ROGL_WATER_H
