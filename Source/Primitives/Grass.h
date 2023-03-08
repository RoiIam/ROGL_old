#include <glad/glad.h>
#include <glm/glm.hpp>
#include "stb_image.h"
#include "Utilities/PerfAnalyzer.h"

#include "shader.h"
#include "model.h"



class Grass :public Model {
//private:

public:
    Grass();
    ~Grass() override;
    Shader ourShader;

    static inline glm::vec3 min = glm::vec3(0, -0.5f, -0.01f);
    static inline glm::vec3 max = glm::vec3(1, 0.5f, 0.01f);

    unsigned int transparentTexture;
    unsigned int transparentVAO, transparentVBO;

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


    void Draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model);


    void Draw(Shader &shader, bool simple) override;
};