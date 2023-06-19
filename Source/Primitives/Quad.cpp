#include "Quad.h"


Quad::Quad() = default;

Quad::~Quad() {
    glDeleteTextures(1, &texture);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &vbo);
}

Quad::Quad(Shader *shader) {

    if (shader != nullptr)
        ourShader = *shader;
    else
        ourShader = Shader("..\\Assets\\Shaders\\Forward\\skyboxXX.vert",
                           "..\\Assets\\Shaders\\Forward\\skyboxXX.frag");
    // skybox VAO
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


//void Quad::render(glm::mat4 *projection, glm::mat4 *view) {
void Quad::Draw(Shader &shader, bool simple) {
    //ourShader.use();
    //glDepthMask(GL_FALSE);
    glBindVertexArray(vao);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    //ourShader.setMat4("projection", *projection);
    //ourShader.setMat4("view", *view);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    PerfAnalyzer::drawcallCount++;
    glBindVertexArray(0);
    //glDepthMask(GL_TRUE);
}