#include "DefaultCube.h"


DefaultCube::DefaultCube() = default;

DefaultCube::DefaultCube(Shader *shader, glm::vec3 minB, glm::vec3 maxB) {
    if (shader != nullptr)
        ourShader = *shader;

    CreateVert(minB, maxB);

    // skybox VAO
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
}


void DefaultCube::CreateVert(glm::vec3 min, glm::vec3 max) {
    float vertices2[108] = {
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
    std::copy(vertices2, vertices2 + sizeof(vertices2) / sizeof(vertices2[0]), vertices);
}

void DefaultCube::render(glm::mat4 projection, glm::mat4 view, glm::mat4 model, glm::vec4 Col) {
    ourShader.use();


    ourShader.setMat4("projection", projection);
    ourShader.setMat4("view", view);
    ourShader.setMat4("model", model);
    ourShader.setVec4("col", Col);
    // render the loaded obj model
    glBindVertexArray(vao);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    PerfAnalyzer::drawcallCount++;
    glBindVertexArray(0);

}