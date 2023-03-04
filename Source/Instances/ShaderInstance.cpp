//example of one file having definitions for multiple classes in one  .h full of declarations
#include "ShaderInstance.h"


void ShaderInstance::SetShader(Shader *s) {
    instancedShader = s;
}
//nie void SetShader::ShaderInstance(Shader * s)


/*void ShaderInstance::SetupMaterial() //virtual, why wont it say that?
{
// lets try to uncomment it, if we even need it
};*/

void PhongShaderInstance::SetupMaterial() //override
{

}


BasicShaderInstance::BasicShaderInstance() {
    instancedShader = new Shader("..\\..\\Assets\\Shaders\\Basic\\basic.vert",
                                 "..\\..\\Assets\\Shaders\\Basic\\basic.frag");
};

void BasicShaderInstance::SetupMaterial() //override
{
    instancedShader->use();
    instancedShader->setMat4("projection", uniforms.projection);
    instancedShader->setMat4("view", uniforms.view);
    instancedShader->setVec4("col", defaultCol);
}


StencilShaderInstance::StencilShaderInstance() {
    instancedShader = new Shader("..\\Assets\\Shaders\\Basic\\stencil.vert",
                                 "..\\Assets\\Shaders\\Basic\\stencil.frag");
};

void StencilShaderInstance::SetupMaterial() //override
{
    instancedShader->use();
    instancedShader->setMat4("projection", uniforms.projection);
    instancedShader->setMat4("view", uniforms.view);
    //instancedShader->setVec4("col", defaultCol);
}

Shader *StencilShaderInstance::GetShader() {
    return instancedShader;
}



