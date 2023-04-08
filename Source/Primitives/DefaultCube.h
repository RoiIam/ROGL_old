#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.h"
#include "Utilities/PerfAnalyzer.h"

class DefaultCube {
private:
    //Declaring buffer parameters
    GLuint vao, vbo, ebo;


public:
    DefaultCube();

    Shader ourShader = Shader("..\\Assets\\Shaders\\Forward\\basic.vert", "..\\Assets\\Shaders\\Forward\\basic.frag");

    static inline glm::vec3 minTest = glm::vec3(-1, -1.5, -1);
    static inline glm::vec3 maxTest = glm::vec3(1, 1.5, 1);
    // Declares the Vertex Array, where the coordinates of all the 8 cube vertices are stored
    float vertices[108];


    // Declares the Elements Array, where the indexs to be drawn are stored
    inline static GLuint elements[] = {
            3, 2, 6, 7, 4, 2, 0,
            3, 1, 6, 5, 4, 1, 0
    };


    DefaultCube(Shader *shader, glm::vec3 minB, glm::vec3 maxB);

    void CreateVert(glm::vec3 min, glm::vec3 max);

    void render(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 Col);
};