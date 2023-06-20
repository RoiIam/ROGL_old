#ifndef ROGL_WATER_H
#define ROGL_WATER_H

#include "Quad.h"

class Water : public Quad {



public:

    Water(Shader *shader);

    unsigned int reflectionTexture;
    unsigned int refractionTexture;
    unsigned int dudvTexture;

    glm::vec2 moveFactor = glm::vec2(0);
    glm::vec2 waveSpeed = glm::vec2(0.003f,0);
    void Draw(Shader &shader, bool simple) override;
    //void SetTextures();
};


#endif //ROGL_WATER_H
