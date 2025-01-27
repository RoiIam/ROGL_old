#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Shader {


private:
    void checkCompileErrors(GLuint shader, std::string type); //TODO add shader location/name so debug is easier


    std::string vertexPath;
    std::string fragmentPath;
    std::string geometryPath;

public:
    Shader();

    ~Shader();

    Shader(  std::string vertexPath,   std::string fragmentPath,  std::string geometryPath="");

    unsigned int ID;


    void use();

    void Recompile();

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w);

    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;

    void setMat3(const std::string &name, const glm::mat3 &mat) const;

};
