//
// Created by RoiIam on 16. 10. 2023.
//

#ifndef ROGL_PLANETGAME_H
#define ROGL_PLANETGAME_H


#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include "../Instances/SceneInstance.h"
#include "../Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "../Lights//Light.h"


class PlanetGame : public SceneInstance{

public:
    PlanetGame();

    ~PlanetGame() override;

private:
    ObjectInstance *ball01_OI; //projectile
    ObjectInstance *ball02_OI; //planets
    ObjectInstance *ball03_OI; //planets
    ObjectInstance *target_OI; //target to hit


public:

    ObjectInstance *playerChick_OI;  //player
    int playerSteps = 7;
    bool allowBorderCross = false; //if we allow player to go out of window to the other side
    float playerOffset = 1.3;
    bool shoot = false;
    float shotStrengthPercent=0;
    float finalShotStrength=4;
    float shotAngle = 0;
    float defaultTimer = 15.0f; //projectileTimeToLive
    float timer = 15.0f;
    glm::vec3 force = glm::vec3(0);

    int score = 0;
    int defaultLives = 10; //should be protected from writing - const
    int lives = 10;
    int misses = 0;
    bool gameOver = false;

    float drag = 2.5f; // aj 0.25f je cool
    bool enableDrag=false;
    bool containInScreenSpace=false;//delete
    //bool containInScreenSpace=false;

    glm::vec3 v = glm::vec3(0,0,0);
    glm::vec3 launchVelocity = glm::vec3(0,0,0);
    glm::vec3 a = glm::vec3(5.2,-9.8,0);
    std::vector<ObjectInstance *> planets_OIs; //objects that apply forces
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
    void ManagePlanets();
    bool TestCollision(int i, int j,float &dx, float  &dy, float& d);
    //void SolveCollision(std::pair<int, int> p);
    void SolveCollision();
    void CreateNewBall( glm::vec3 coords);

};


#endif //ROGL_PLANETGAME_H
