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

    std::vector<std::vector<float>> verts = {
            {0.0157563, 0.81994},
            {0.0136555, 0.691964},
            {0.111345, 0.723214},
            {0.136555, 0.825893},
            {0.186975, 0.736607},
            {0.215336, 0.85119},
            {0.241597, 0.965774},
            {0.132353, 0.971726},
            {0.105042, 0.592262},
            {0.176471, 0.595238},
            {0.173319, 0.311012},
            {0.246849, 0.346726},
            {0.227941, 0.123512},
            {0.310924, 0.178571},
            {0.330882, 0.85119},
            {0.310924, 0.74256},
            {0.434874, 0.799107},
            {0.409664, 0.690476},
            {0.590336, 0.68006},
            {0.564076, 0.568452},
            {0.698529, 0.620536},
            {0.785714, 0.623512},
            {0.767857, 0.907738},
            {0.888655, 0.873512},
            {0.670168, 0.553571},
            {0.746849, 0.525298},
            {0.933824, 0.665179},
            {0.947479, 0.540179},
            {0.635504, 0.380952},
            {0.713235, 0.372024},
            {0.625, 0.0238095},
            {0.781513, 0.016369},
            {0.915966, 0.440476},
            {0.978992, 0.294643},
            {0.641807, 0.206845},
            {0.641807, 0.0952381},
            {0.40021, 0.203869},
            {0.390756, 0.0833333},
            {0.698529, 0.212798},
            {0.696429, 0.104167}
    };

    //clock-wise
    std::vector<std::vector<int>> polys = {
            {0,3,2,1}, /*0*/
            {3,5,4,2},   /*1*/
            {7,6,5,3},  /*2*/
            {2,4,9,8},  /*3*/
            {8,9,11,10},    /*4*/
            {10,11,13,12},      /*5*/
            {5,14,15,4},        /*6*/
            {14,16,17,15},      /*7*/
            {16,18,19,17},      /*8*/
            {18,21,25,19},      /*9*/
            {20,22,23,21},      /*10*/
            {21,26,27,25},      /*11*/
            {24,25,29,28},      /*12*/
            {28,29,31,30},      /*13*/
            {32,33,39,38},      /*14*/
            {34,35,37,36}       /*15*/

    };



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
    //poly map settings
    bool usePolyMap = true;
    bool testMouse = true;
    int polyId = 0;


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
    void CreatePolyMap();
    bool TestPoly(glm::vec3 testV);
};


#endif //ROGL_COINMAPGAME_H
