#include "Water.h"


void Water::Draw(Shader &shader, bool simple) {

    shader.use();
    glBindVertexArray(vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    shader.setInt("reflectionTexture", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    shader.setInt("refractionTexture", 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dudvTexture);
    shader.setInt("dudvTexture", 2);
    //normal
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normalMapTexture);
    shader.setInt("normalMapTexture", 3);


    moveFactor.x += waveSpeed.x; //not frame independent for now...
    moveFactor.x = std::fmod(moveFactor.x, 1.0f);
    moveFactor.y += waveSpeed.y; //not frame independent for now...
    moveFactor.y = std::fmod(moveFactor.y, 1.0f);
    shader.setVec2("moveFactor", moveFactor);


    shader.setVec3("cameraPos", cam->Position);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    PerfAnalyzer::drawcallCount++;
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);


}


Water::Water(Shader *shader, Camera * cam) {


    this->cam = cam;
    if (shader != nullptr)
        ourShader = *shader;
    else
        ourShader = Shader("..\\Assets\\Shaders\\Forward\\Water\\water.vert",
                           "..\\Assets\\Shaders\\Forward\\Water\\water.frag");


    dudvTexture= Model::TextureFromFile("waterDUDV.png", "..\\Assets\\Textures\\", false);
    normalMapTexture= Model::TextureFromFile("matchingNormalMap.png", "..\\Assets\\Textures\\", false);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
    //texture = loadCubemap(faces);

}