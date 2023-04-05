#include "Grass.h"


Grass::Grass() {

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

    transparentTexture = Model::TextureFromFile("grass.png", "..\\Assets\\Textures\\", false);

}
Grass::~Grass() = default;


void Grass::Draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
    //TODO do we even use this
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);
    ourShader.setMat4("model", model);
    Grass::Draw(ourShader, false);


}

void Grass::Draw(Shader &shader, bool simple) {

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