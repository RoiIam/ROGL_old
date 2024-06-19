#pragma once
//#include <glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "../ASSIMPViewer/shader.cpp"
#include "imgui.h"

class ShaderClass
{
 protected:
  Shader * instancedShader = nullptr;

  void SetShader(Shader * s)
  {
      instancedShader = s;
  }
  virtual void SetupMaterial()
  {

  }



};


class PhongShaderInstance : public ShaderClass
{
 public:

  void SetupMaterial() override
  {

  }




};

class BasicShaderInstance : public ShaderClass
{
 public:
  BasicShaderInstance(){
      instancedShader = new Shader("..\\Assets\\Shaders\\Basic\\basic.vert", "..\\Assets\\Shaders\\Basic\\basic.frag");
  };

  void SetupMaterial() override
  {
    instancedShader->use();
    instancedShader->setMat4("projection", uniforms.projection);
    instancedShader->setMat4("view", uniforms.view);
    instancedShader->setVec4("col", defaultCol);
  }

 private:
  glm::vec4 defaultCol = glm::vec4(1, 0.08, 0.575, 1.0);




};

class StencilShaderInstance : public ShaderClass
{
 public:
  StencilShaderInstance(){
    instancedShader = new  Shader("..\\Assets\\Shaders\\Basic\\stencil.vert",
                                                 "..\\Assets\\Shaders\\Basic\\stencil.frag");
  };

  void SetupMaterial() override
  {
    instancedShader->use();
    instancedShader->setMat4("projection", uniforms.projection);
    instancedShader->setMat4("view", uniforms.view);
    //instancedShader->setVec4("col", defaultCol);
  }

  Shader * GetShader(){
    return instancedShader;
  }


};