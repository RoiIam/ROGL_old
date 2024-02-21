#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "stb_image.h"
#include "Utilities/PerfAnalyzer.h"

#include "shader.h"
#include "model.h"

//zatial len copypasta grass
class Plane : public Model {
//private:

enum PlaneType{Default,Fire,Forest,Water};

public:
    Plane();
    Plane(std::string textureName);
    ~Plane() override;

    Shader ourShader;

    static inline glm::vec3 min = glm::vec3(0, -0.5f, -0.01f);
    static inline glm::vec3 max = glm::vec3(1, 0.5f, 0.01f);

    unsigned int transparentTexture;
    unsigned int transparentVAO, transparentVBO;
    unsigned int cellType;

    // same as  https://learnopengl.com/code_viewer_gh.php?code=src/4.advanced_opengl/3.1.blending_discard/blending_discard.cpp
    static inline float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)  // swap not neededd
            0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.0f, -0.5f, 0.0f, 0.0f, 0.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f,

            0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
            1.0f, -0.5f, 0.0f, 1.0f, 0.0f,
            1.0f, 0.5f, 0.0f, 1.0f, 1.0f
    };

    //unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma);//mozme pouzit base

    void Draw(Shader &shader, bool simple) override;
};