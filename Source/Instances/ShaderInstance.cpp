//example of one file having definitions for multiple classes in one  .h full of declarations
#include "ShaderInstance.h"
//we need to addres how to report which shader got errors, bcs now its mainly cant find the shader, wrong location errors


ShaderInstance::ShaderInstance() = default;
ShaderInstance::~ShaderInstance() = default;

void ShaderInstance::SetShader(Shader *s) {
    instancedShader = s;
}
//nie void SetShader::ShaderInstance(Shader * s)


void ShaderInstance::SetupMaterial() //virtual, why wont it say that?
{

};

void PhongShaderInstance::SetupMaterial() //override
{

}


BasicShaderInstance::BasicShaderInstance() {
    instancedShader = new Shader("..\\Assets\\Shaders\\Forward\\basic.vert",
                                 "..\\Assets\\Shaders\\Forward\\basic.frag");
};

void BasicShaderInstance::SetupMaterial() //override
{
    instancedShader->use();
    instancedShader->setMat4("projection", uniforms.projection);
    instancedShader->setMat4("view", uniforms.view);
    instancedShader->setVec4("col", defaultCol);
}


StencilShaderInstance::StencilShaderInstance() {
    instancedShader = new Shader("..\\Assets\\Shaders\\Forward\\stencil.vert",
                                 "..\\Assets\\Shaders\\Forward\\stencil.frag");
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



