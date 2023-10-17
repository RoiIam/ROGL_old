//
// Created by RoiIam on 10. 10. 2023.
//

#ifndef ROGL_POOLGAME_H
#define ROGL_POOLGAME_H

#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include "../Instances/SceneInstance.h"
#include "../Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "../Lights//Light.h"

struct collision{
    int i;
    int j;
    float dx;
    float dy;
    float d;
};

class PoolGame : public SceneInstance{

public:
    PoolGame();

    ~PoolGame() override;

private:
    ObjectInstance *ball01_OI;  //player
    ObjectInstance *ball02_OI; //eagles want this, small chicken/eggs


public:

    float drag = 2.5f; // aj 0.25f je cool
    bool enableDrag=true;
    bool containInScreenSpace=false;
    float systemKineticE = 0;

    //bool containInScreenSpace=false;

    glm::vec3 v = glm::vec3(0,0,0);
    glm::vec3 launchVelocity = glm::vec3(0,0,0);
    glm::vec3 a = glm::vec3(5.2,-9.8,0);
    std::vector<ObjectInstance *> colliders_OIs; //objects that can collide
    Shader * basicShaderR= nullptr;

    void Setup(Camera *cam, GraphicsOptions *graphicsOptions) override;
    void RenderSceneInstance(Shader *s, bool renderSelected) override;
    void InitializeGame();
    void GameTimeStep();
    void Movement();
    void EndGame();
    void DestroyGame();


    void ResetGame(); //start with full lives, 0 score
    void ReloadGame(); //only reload after eagle gets to the ground and we lose life
    void GameUI();
    void ManageCollisions();
    bool TestCollision(int i, int j,float &dx, float  &dy, float& d);
    //void SolveCollision(std::pair<int, int> p);
    void SolveCollision( collision c);
    void CreateNewBall( glm::vec3 coords);

};


#endif //ROGL_POOLGAME_H
