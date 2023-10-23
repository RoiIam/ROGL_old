//
// Created by RoiIam on 17. 10. 2023.
//

#ifndef ROGL_COINMAPGAME_H
#define ROGL_COINMAPGAME_H


#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include "Instances/SceneInstance.h"
#include "Instances/ObjectInstance.h"
#include "Primitives/DefaultCube.h"
#include "Primitives/Grass.h"
#include "Lights/Light.h"


class CoinMapGame : public SceneInstance{

public:
    CoinMapGame();

    ~CoinMapGame() override;

private:
    ObjectInstance *playerChick_OI;  //player
    ObjectInstance *chick_OI; //
    ObjectInstance *eagle_OI;  //
    ObjectInstance *mapPlane_OI;


public:

    std::vector<glm::vec3>  vertex; //list of traversable places
    std::vector<std::pair<int,int>>  edges; //list of vertex indices, non directional!
    int curLocation = 0;
    Grass * mapPlane;
    //Grass grassplane;
    Shader * grassShader;

    //values for simple "interpolation"
    bool isMoving = false;
    const float moveStepDefault = 0.1f;
    float moveStep = 0;
    glm::vec3 startPos;
    glm::vec3 endPos;
    glm::vec3 direction;
    float moveSpeed = 10.0f;


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
    void ClickMouse(float x, float y);
    //we take ortho normalized scree  coordinates and calculate them to the ortho space
    glm::vec3 TranslateToWorld(glm::vec3 ndcVec);
    void InitMap();

};


#endif //ROGL_COINMAPGAME_H
