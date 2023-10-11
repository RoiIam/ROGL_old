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
    Shader *shader;
    //Material * material;



    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    float degX = 0.0f;
    float degY = 0.0f;
    float degZ = 0.0f;

public:
    // model data
    // Model**  model2;
    std::string Name = "defaultName";
    //OGLR::Managers::Transform transform{};
    Light *light = nullptr;
    bool enableVisualRender = true;
    bool forceRenderOwnShader = false;
    bool disableRender = false;
    // creates model instance

    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 acceleration = glm::vec3(0);
    float mass = 5.0f;

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

    void SetScale(glm::vec3 p);

    glm::vec3 GetScale();

    void SetDeg(float d, std::string id);

    float GetDeg(std::string id);

    glm::mat4 GetTransformMat();

    void ForceSetTransformMat(glm::vec3 posVec, float degrees[3],glm::vec3 scaleVec);

    void UpdateTransformMat(Shader *sh)  // update model matrix with own values
    ;

    //preco su tieto 2 pointery?
    Model *GetModel();

    Shader *GetShader();

    void SetShader(Shader &s);

};