#include "ObjectInstance.h"
#include  "Utilities/Managers.h"



// allow more instances with the same model and or shader
//

ObjectInstance::ObjectInstance(Model &tmp) { model = &tmp; }
// creates model instance, pass empty name to assign generated one, pass null light for renderable objects only// yeah bad design for now

ObjectInstance::ObjectInstance(Model &tmp, Shader &shdr, const std::string &name, Light *l) {
    //TODO decouple lights from objects...
    model = &tmp;
    shader = &shdr;
    if (name.empty())
        Name = "NewObj";
    else
        Name = name;
    if (l != nullptr)
        light = l; //else nullptr?

}

//ObjectInstance::~ObjectInstance() = default;
ObjectInstance::~ObjectInstance() {
    delete model;
    //delete shader;
};


// render using this.... no need to set model mat
void ObjectInstance::Render() {
    if (!enableVisualRender)
        return;
    shader->use();
    UpdateTransformMat(shader);
    model->Draw(*shader, false);
}

void ObjectInstance::Render(Shader *s, bool simple) {
    if (s == nullptr) {
        Render();
        return;
    }
    if (!enableVisualRender)
        return;
    s->use();

    //TODO we need a way to assign relevant uniforms for the specific shaders
    UpdateTransformMat(s);
    model->Draw(*s, simple);
}

void ObjectInstance::SetPos(glm::vec3 p) {
    position = p;
    SetTransformMat();
}

glm::vec3 ObjectInstance::GetPos() { return position; }


void ObjectInstance::SetScale(glm::vec3 p) {
    scale = p;
    SetTransformMat();
}

glm::vec3 ObjectInstance::GetScale() { return scale; }

void ObjectInstance::SetDeg(float d, std::string id) {
    if (id == "X")
        degX = d;
    else if (id == "Y")
        degY = d;
    else if (id == "Z")
        degZ = d;
    else
        std::cout << "cannot set rotation for non existent " << id << " id val" << std::endl;
}

float ObjectInstance::GetDeg(std::string id) {
    if (id == "X")
        return degX;
    else if (id == "Y")
        return degY;
    else if (id == "Z")
        return degZ;
    else {
        std::cout << "cannot return rotation for non existent " << id << " id val" << std::endl;
        return -10101;
    }


}

glm::mat4 ObjectInstance::GetTransformMat() { return modelMatrix; }

void ObjectInstance::ForceSetTransformMat(glm::vec3 posVec, float degrees[3], glm::vec3 scaleVec) {
    SetScale(scaleVec);
    SetPos(posVec);
    degX = degrees[0];
    degY = degrees[1];
    degZ = degrees[2];
    SetTransformMat();
}

void ObjectInstance::UpdateTransformMat(Shader *sh)  // update model matrix with own values
{
    SetTransformMat();
    sh->setMat4("model", modelMatrix);
    sh->setMat4("view", uniforms.view);
    sh->setMat4("projection", uniforms.projection);
}

Model *ObjectInstance::GetModel() { return model; }//preco je toto pointer?
Shader *ObjectInstance::GetShader() { return shader; }//preco je toto pointer?
void ObjectInstance::SetShader(Shader &s) {
    //if(s != nullptr)
    shader = &s;
}

void ObjectInstance::SetTransformMat() {
    modelMatrix = glm::mat4(1.0f);  // do not forget this otherwise we just fly off...
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(degX), glm::vec3(1, 0, 0)); //x
    modelMatrix = glm::rotate(modelMatrix, glm::radians(degY), glm::vec3(0, 1, 0)); //y
    modelMatrix = glm::rotate(modelMatrix, glm::radians(degZ), glm::vec3(0, 0, 1)); //z

    modelMatrix = glm::scale(modelMatrix, scale);

}