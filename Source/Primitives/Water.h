#ifndef ROGL_WATER_H
#define ROGL_WATER_H

#include "Quad.h"

class Water : public Quad {



public:

    Water(Shader *shader);

    unsigned int reflectionTexture;
    unsigned int refractionTexture;
    void Draw(Shader &shader, bool simple) override;
    //void SetTextures();
};


#endif //ROGL_WATER_H
