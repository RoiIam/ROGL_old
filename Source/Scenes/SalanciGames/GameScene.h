//
// Created by RoiIam on 19. 9. 2023.
//
#ifndef ROGL_GAMESCENE_H
#define ROGL_GAMESCENE_H
#endif //ROGL_GAMESCENE_H

#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include "Instances/SceneInstance.h"
#include "Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "Lights/Light.h"


class GameScene : public SceneInstance { //make it public so we can access camera in parent, Scene

public:
    GameScene();

    ~GameScene() override;

private:
    ObjectInstance *playerChick_OI;  //player
    ObjectInstance *chick_OI; //eagles want this, small chicken/eggs
    ObjectInstance *eagle_OI;  // enemies


public:

    //int spawnTime = 5; //time in seconds to spawn another eagle now it reuses the same eagle
    int playerSteps = 7;
    int score = 0;
    int defaultLives = 3; //should be protected from writing - const
    int lives = 3;
    float stepTime = 1.0;
    float stepSize = 0.165;
    const int defaultStepTime = 2;
    float eagleStep=1;
    float eagleCutoff = -3.21;
    float defaultEagleY = 3.7;
    //int iPlayer =0; moved to cam
    bool allowBorderCross = false; //if we allow player to go out of window to the other side
    bool gameover = false;
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