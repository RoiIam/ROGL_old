//
// Created by RoiIam on 4. 10. 2023.
//

#ifndef ROGL_CANNONGAME_H
#define ROGL_CANNONGAME_H

#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include "Instances/SceneInstance.h"
#include "Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "Lights/Light.h"


class CannonGame : public SceneInstance{

public:
    CannonGame();

    ~CannonGame() override;

private:
    ObjectInstance *playerChick_OI;  //player
    ObjectInstance *chick_OI; //eagles want this, small chicken/eggs
    ObjectInstance *eagle_OI;  // enemies


public:


    int score = 0;
    int defaultLives = 3; //should be protected from writing - const
    int misses = 3;
    bool gameOver = false;
    bool shoot = false;
    float shotStrengthPercent=0;
    float finalShotStrength=15;
    float shotAngle = 0;
    float playerOffset = 2.0;
    glm::vec3 v = glm::vec3(0,0,0);
    glm::vec3 a = glm::vec3(5.2,-9.8,0);

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

};


#endif //ROGL_CANNONGAME_H
