#include "ObjectInstance.h"

namespace OGLR {

using namespace std;

// allow more instances with the same model and or shader
//
class ObjectInstance {
 public:
  // model data
  // Model**  model2;
  string Name="defaultName";
  //OGLR::Managers::Transform transform{};
  //Light* light = nullptr;
  bool enableRender = true;
  // creates model instance
  explicit ObjectInstance(Model &tmp) { model = &tmp; }
  // creates model instance, pass empty name to assign generated one, pass null light for renderable objects only// yeah bad design for now
  ObjectInstance(Model &tmp, Shader &shdr, const string& name, void * l) {
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
//  ObjectInstance(Model &tmp, Shader &shdr, const string& name, Light* l ) {
//    model = &tmp;
//    shader = &shdr;
//    if(name.empty())
//      Name = "NewObj";
//    else
//      Name = name;
//    //if(l != nullptr)
//    //  light=l; //else nullptr?
//
//  }
  ~ObjectInstance()
  {

  }

  // render using this.... no need to set model mat
  void Render() {
    if(!enableRender)
      return;
    shader->use();
    UpdateTransformMat(shader);
    model->Draw(*shader,false);
  }
  void Render(Shader *s, bool simple) {
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
  void SetPos(glm::vec3 p) {
    position = p;
    SetTransformMat();
  }
  glm::vec3 GetPos() { return position; }

  void SetRot(glm::vec3 p) { rotation = p;
    SetTransformMat();
  }
  glm::vec3 GetRot() { return rotation; }
  void SetScale(glm::vec3 p) {
       scale = p;
    SetTransformMat();
  }
  glm::vec3 GetScale() { return scale; }

  void SetDeg(float d) { deg = d; }
  float GetDeg() { return deg; }
  glm::mat4 GetTransformMat() { return modelMatrix; }
  void ForceSetTransformMat(glm::vec3 posVec, float degrees,
                            glm::vec3 rotAxisVec, glm::vec3 scaleVec) {
    SetScale(scaleVec);
    SetRot(rotAxisVec);
    SetPos(posVec);
    deg = degrees;
    SetTransformMat();
  }
  void UpdateTransformMat(Shader *sh)  // update model matrix with own values
  {
    SetTransformMat();
    sh->setMat4("model", modelMatrix);
    //sh->setMat4("view", uniforms.view);
    //sh->setMat4("projection", uniforms.projection);
  }

  Model *GetModel() { return model; }
  Shader *GetShader() { return shader; }
  void SetShader(Shader &s) {
    //if(s != nullptr)
      shader = &s;
  }

 private:
  //
  Model *model;
  Shader *shader = new Shader("..\\Assets\\Shaders\\Debug\\emptyPink.vert",
                              "..\\Assets\\Shaders\\Debug\\emptyPink.frag");
  //Material * material;

  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glm::vec3 position = glm::vec3(0.0f);
  glm::vec3 rotation =
      glm::vec3(1.0f);  // if we try to ratate by 0 around ... 0 0 0 its bad...
  glm::vec3 scale = glm::vec3(1.0f);
  float deg = 0.0f;

  void SetTransformMat() {
    modelMatrix =
        glm::mat4(1.0f);  // do not forget this otherwise we just fly off...
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(deg), rotation);
    modelMatrix = glm::scale(modelMatrix, scale);

  }
};
}