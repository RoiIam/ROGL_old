#include "Light.h" //redefinition lebo uz skor sme dakde inkludli

class DirectionalLight : public Light {
public:
    glm::vec3 direction;
    using Light::Light;//using
    DirectionalLight();

    ~DirectionalLight() override;

private:

};