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

class Cube {
private:
    //Declaring buffer parameters
    GLuint vao, vbo, ebo; //treba glad

    unsigned int cubemapTexture; //= loadCubemap(faces);

    std::vector<std::string> faces
            {
                    //enjoy https://www.google.com/maps/place/43%C2%B050'23.8%22N+79%C2%B023'12.2%22W/@43.8398201,-79.3890158,78a,35y,87.33h,69.1t/data=!3m1!1e3!4m5!3m4!1s0x0:0x0!8m2!3d43.8399294!4d-79.3867094
                    "../Assets/models/skybox/right.jpg",
                    "../Assets/models/skybox/left.jpg",
                    "../Assets/models/skybox/bottom.jpg",
                    "../Assets/models/skybox/top.jpg",
                    "../Assets/models/skybox/front.jpg",
                    "../Assets/models/skybox/back.jpg"
            };

    unsigned int loadCubemap(std::vector<std::string> faces);

public:

    //Shader ourShader = Shader("..\\Assets\\Shaders\\Basic\\skybox.vert", "..\\Assets\\Shaders\\Basic\\skybox.frag"); ahaaaa
    Shader ourShader;
    static inline glm::vec3 min = glm::vec3(-1, -1, -1);
    static inline glm::vec3 max = glm::vec3(1, 1, 1);
    // Declares the Vertex Array, where the coordinates of all the 8 cube vertices are stored
    static inline float vertices[] = {
            // positions
            min.x, max.y, min.z, //top left corner
            min.x, min.y, min.z,
            max.x, min.y, min.z,
            max.x, min.y, min.z,
            max.x, max.y, min.z,
            min.x, max.y, min.z,

            min.x, min.y, max.z,
            min.x, min.y, min.z,
            min.x, max.y, min.z,
            min.x, max.y, min.z,
            min.x, max.y, max.z,
            min.x, min.y, max.z,

            max.x, min.y, min.z,
            max.x, min.y, max.z,
            max.x, max.y, max.z,
            max.x, max.y, max.z,
            max.x, max.y, min.z,
            max.x, min.y, min.z,

            min.x, min.y, max.z,
            min.x, max.y, max.z,
            max.x, max.y, max.z,
            max.x, max.y, max.z,
            max.x, min.y, max.z,
            min.x, min.y, max.z,

            min.x, max.y, min.z,
            max.x, max.y, min.z,
            max.x, max.y, max.z,
            max.x, max.y, max.z,
            min.x, max.y, max.z,
            min.x, max.y, min.z,

            min.x, min.y, min.z,
            min.x, min.y, max.z,
            max.x, min.y, min.z,
            max.x, min.y, min.z,
            min.x, min.y, max.z,
            max.x, min.y, max.z
    };

    inline static GLfloat vertices2[] = {
            1.0, 1.0, 1.0,
            0.0f, 1.0, 1.0,
            1.0, 1.0, 0.0f,
            0.0f, 1.0, 0.0f,
            1.0, 0.0f, 1.0,
            0.0f, 0.0f, 1.0,
            0.0f, 0.0f, 0.0f,
            1.0, 0.0f, 0.0f
    };
    // Declares the Elements Array, where the indexs to be drawn are stored
    inline static GLuint elements[] = {
            3, 2, 6, 7, 4, 2, 0,
            3, 1, 6, 5, 4, 1, 0
    };


    Cube(Shader *shader);


    void render(glm::mat4 *projection, glm::mat4 *view);
};