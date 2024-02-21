#include "Plane.h"


Plane::Plane() {

    //TODO this doesnt exist
    ourShader = Shader("..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.vert",
                       "..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.frag");


    this->boundMin = min;
    this->boundMax = max;
    // transparent VAO

    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glBindVertexArray(0);
    //fix strings
    //still not sure if use / or \\ in strings same with water
    transparentTexture = Model::TextureFromFile("grass.png", "../Assets/Textures", false);

}

Plane::Plane(std::string map) {

    //TODO  copy of defualt constructor... for now
    ourShader = Shader("..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.vert",
                       "..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.frag");


    this->boundMin = min;
    this->boundMax = max;
    // transparent VAO

    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    glBindVertexArray(0);

    //still not sure if use / or \\ in strings same with water
    transparentTexture = Model::TextureFromFile(map.c_str(), "../Assets/Textures", false);

}

Plane::~Plane() = default;


void Plane::Draw(Shader &shader, bool simple) {
    //shader.use();
    shader.setInt("type", cellType);
    //blending must be enabled prior
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH);
    glBindVertexArray(transparentVAO);
    glBindTexture(GL_TEXTURE_2D, transparentTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    PerfAnalyzer::drawcallCount++;
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_DEPTH);
}