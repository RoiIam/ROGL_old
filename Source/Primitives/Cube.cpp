#include "Cube.h"
#include <iostream>
#include "stb_image.h" //inc here or  in its .h?
#include "Utilities/PerfAnalyzer.h"


unsigned int Cube::loadCubemap(std::vector<std::string> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }


    Cube::Cube(Shader *shader) {

        if(shader != nullptr)
            ourShader = *shader;
        else
            ourShader = Shader("..\\Assets\\Shaders\\Forward\\skybox.vert", "..\\Assets\\Shaders\\Forward\\skybox.frag");

        // skybox VAO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        cubemapTexture = loadCubemap(faces);

        /*
          //back to stripes...
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Creates and binds Vertex Buffer Object (24 is the elements array size)
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

        // Creates and bind Elements Buffer Object (14 is the elements array size)
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
        */
    }


    void Cube::render (glm::mat4 *projection,glm::mat4 *view) {
        ourShader.use();

        glDepthMask(GL_FALSE); // nove
        glBindVertexArray(vao);
        //glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        ourShader.setMat4("projection", *projection);
        ourShader.setMat4("view", *view);
        // render the loaded obj model
        //glBindVertexArray(vao);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        glDrawArrays(GL_TRIANGLES, 0, 36);
        PerfAnalyzer::drawcallCount++;
        glBindVertexArray(0);
        glDepthMask(GL_TRUE);


/*
      //next try... of stripes/nope
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
      glDrawElements(GL_TRIANGLE_STRIP, 14, GL_UNSIGNED_INT, 0);*/
    }