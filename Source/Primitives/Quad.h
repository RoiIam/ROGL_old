#pragma once

#include "glad/glad.h"
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include <iostream>
#include "stb_image.h" //inc here or  in its .h?
#include "Utilities/PerfAnalyzer.h" //already in main?
#include "model.h"

class Quad : public Model {
private:
    //Declaring buffer parameters
    GLuint vao, vbo;

    unsigned int texture;

public:

    explicit Quad(Shader *shader);

    ~Quad() override;

    //Shader ourShader = Shader("..\\Assets\\Shaders\\Basic\\skybox.vert", "..\\Assets\\Shaders\\Basic\\skybox.frag"); ahaaaa
    Shader ourShader;
    static inline glm::vec3 min = glm::vec3(-1, -1, -0.01);
    static inline glm::vec3 max = glm::vec3(1, 1, 0.01);
    // Declares the Vertex Array, where the coordinates of all the 8 cube vertices are stored
    inline static float vertices[] = {
            // positions        // texture Coords
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.0f, -0.5f, 0.0f, 0.0f, 0.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f,

            0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f,
            1.0f, 0.5f, 0.0f, 1.0f, 1.0f};


    //void render(glm::mat4 *projection, glm::mat4 *view);
    void Draw(Shader &shader, bool simple) override;
};