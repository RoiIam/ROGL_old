#pragma once
//#include <glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../shader.h"
#include "imgui.h"
#include  "Utilities/Managers.h" //tu mozme skusit ci to funguje spravne
#include  "Utilities/PerfAnalyzer.h"

class ShaderInstance
{
protected:
    Shader * instancedShader = nullptr;

    void SetShader(Shader * s);

    virtual void SetupMaterial();
    // maybe add virtual Shader * GetShader();

};


class PhongShaderInstance : public ShaderInstance
{
public:

    void SetupMaterial() override;

};

class BasicShaderInstance : public ShaderInstance
{
public:
    BasicShaderInstance();

    void SetupMaterial() override;

private:
    glm::vec4 defaultCol = glm::vec4(1, 0.08, 0.575, 1.0);
};

class StencilShaderInstance : public ShaderInstance
{
public:
    StencilShaderInstance();

    void SetupMaterial() override;

    Shader * GetShader();


};