#include "ObjectInstance.h"


using namespace std;

// allow more instances with the same model and or shader
//

ObjectInstance::ObjectInstance(Model &tmp) { model = &tmp; }
  // creates model instance, pass empty name to assign generated one, pass null light for renderable objects only// yeah bad design for now
  ObjectInstance::ObjectInstance(Model &tmp, Shader &shdr, const string& name, void * l) {
    //TODO TEMP BEFORE WE ADD LIGHTS
      model = &tmp;
      shader = &shdr;
      if(name.empty())
          Name = "NewObj";
      else
          Name = name;
      //if(l != nullptr)
      //  light=l; //else nullptr?

  }
  ObjectInstance::ObjectInstance(Model &tmp, Shader &shdr, const string& name, Light* l ) {
    model = &tmp;
    shader = &shdr;
    if(name.empty())
      Name = "NewObj";
    else
      Name = name;
    //if(l != nullptr)
    //  light=l; //else nullptr?

  }
ObjectInstance::~ObjectInstance()=default;


  // render using this.... no need to set model mat
  void ObjectInstance::Render() {
    if(!enableRender)
      return;
    shader->use();
    UpdateTransformMat(shader);
    model->Draw(*shader,false);
  }
  void ObjectInstance::Render(Shader *s, bool simple) {
    if(s == nullptr)
    {
      Render();
      return;
    }
    if(!enableRender)
      return;
    s->use();
    UpdateTransformMat(s);
    model->Draw(*s,simple);
  }
  void ObjectInstance::SetPos(glm::vec3 p) {
    position = p;
    SetTransformMat();
  }
  glm::vec3 ObjectInstance::GetPos() { return position; }

  void ObjectInstance::SetRot(glm::vec3 p) { rotation = p;
    SetTransformMat();
  }
  glm::vec3 ObjectInstance::GetRot() { return rotation; }
  void ObjectInstance::SetScale(glm::vec3 p) {
       scale = p;
    SetTransformMat();
  }
  glm::vec3 ObjectInstance::GetScale() { return scale; }

  void ObjectInstance::SetDeg(float d) { deg = d; }
  float ObjectInstance::GetDeg() { return deg; }
  glm::mat4 ObjectInstance::GetTransformMat() { return modelMatrix; }
  void ObjectInstance::ForceSetTransformMat(glm::vec3 posVec, float degrees,
                            glm::vec3 rotAxisVec, glm::vec3 scaleVec) {
    SetScale(scaleVec);
    SetRot(rotAxisVec);
    SetPos(posVec);
    deg = degrees;
    SetTransformMat();
  }
  void ObjectInstance::UpdateTransformMat(Shader *sh)  // update model matrix with own values
  {
    SetTransformMat();
    sh->setMat4("model", modelMatrix);
    //sh->setMat4("view", uniforms.view);
    //sh->setMat4("projection", uniforms.projection);
  }

  Model *ObjectInstance::GetModel() { return model; }//preco je toto pointer?
  Shader *ObjectInstance::GetShader() { return shader; }//preco je toto pointer?
  void ObjectInstance::SetShader(Shader &s) {
    //if(s != nullptr)
      shader = &s;
  }

  void ObjectInstance::SetTransformMat() {
    modelMatrix =
        glm::mat4(1.0f);  // do not forget this otherwise we just fly off...
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(deg), rotation);
    modelMatrix = glm::scale(modelMatrix, scale);

  }