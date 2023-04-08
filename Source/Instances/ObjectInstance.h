#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>
//#include <glad.h>





#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "shader.h"
#include "model.h"
#include "Lights/Light.h"
//#include "Light.h"
//#include "Material.h"

#include <sstream>
#include <string>
#include <vector>

//#include  "Utilities/Managers.h"

class ObjectInstance {

private:
    //
    Model *model;
    Shader *shader = new Shader("..\\Assets\\Shaders\\Debug\\emptyPink.vert",
                                "..\\Assets\\Shaders\\Debug\\emptyPink.frag");
    //Material * material;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(1.0f);  // if we try to ratate by 0 around ... 0 0 0 its bad...
    glm::vec3 scale = glm::vec3(1.0f);
    float deg = 0.0f;

public:
    // model data
    // Model**  model2;
    std::string Name = "defaultName";
    //OGLR::Managers::Transform transform{};
    Light *light = nullptr;
    bool enableRender = true;
    // creates model instance

    explicit ObjectInstance(Model &tmp);

    ObjectInstance(Model &tmp, Shader &shdr, const std::string &name, Light *l);

    ~ObjectInstance();


private:
    void SetTransformMat();

public:
    // render using this.... no need to set model mat
    void Render();

    void Render(Shader *s, bool simple);

    void SetPos(glm::vec3 p);

    glm::vec3 GetPos();

    void SetRot(glm::vec3 p);

    glm::vec3 GetRot();

    void SetScale(glm::vec3 p);

    glm::vec3 GetScale();

    void SetDeg(float d);

    float GetDeg();

    glm::mat4 GetTransformMat();

    void ForceSetTransformMat(glm::vec3 posVec, float degrees,
                              glm::vec3 rotAxisVec, glm::vec3 scaleVec);

    void UpdateTransformMat(Shader *sh)  // update model matrix with own values
    ;

    //preco su tieto 2 pointery?
    Model *GetModel();

    Shader *GetShader();

    void SetShader(Shader &s);

};