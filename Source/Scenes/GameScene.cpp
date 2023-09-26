//
// Created by RoiIam on 19. 9. 2023.
//
#include "GameScene.h"

GameScene::GameScene() {
    //enableMainUI = false; //comment for easier debug
}

void GameScene::Setup(Camera *cam, GraphicsOptions *graphicsOptions)  {
    SceneInstance::Setup(cam,graphicsOptions);
    InitializeGame();
}

void GameScene::InitializeGame() {

    gameover = false;
    //set camera to a defined spot
    camera->SetPosDir(glm::vec3(0,0,0),glm::vec3(0.0f, 1.0f, 0.0f),-90,0);
    //lock camera movement
    camera->cameraControlsUnlocked = false;
    //load chicken and egg assets
    //we should use basicShader
    playerChick_OI = new ObjectInstance(
            * new Model("../Assets/Models/Shrek/Shrek_mod.gltf"),
            basicTexturedShader, "player_chick", nullptr);
    chick_OI = new ObjectInstance(
            * new Model("../Assets/Models/sphere/sphere.obj"),
            basicTexturedShader, "chickGround", nullptr);
    eagle_OI = new ObjectInstance(
            * new Model("../Assets/Models/OwnCube/Cube.obj"),
            basicTexturedShader, "eagle", nullptr);

    selectableObjInstances.push_back(playerChick_OI);
    selectableObjInstances.push_back(chick_OI);
    selectableObjInstances.push_back(eagle_OI);


    //set chicken..etc to position
    playerChick_OI->SetPos(glm::vec3(0.0f, -3.55f, -10.0f));
    playerChick_OI->SetScale(glm::vec3(0.05, 0.05, 0.05));

    chick_OI->SetPos(glm::vec3(0.0f, -3.75f, -10.0f));
    chick_OI->SetScale(glm::vec3(0.25, 0.2, 0.25));

    eagle_OI->SetPos(glm::vec3(0.0f, 3.3f, -10.0f));
    eagle_OI->SetScale(glm::vec3(0.20,0.11,0.3));

    //range X shrekEagle left to right -5.7 az po 5.7
    // so shrek should move like 6 times to left to get to the edge
    //5.7*2 = 11,4 / 6 =1,9

    //takze pos bude int i, ktory ak stlacim <- tak sa zmensi a -> tak sa pripocita modulo 7

    //finalna x pozicia bude -5,7+ (i mod7)*1,9

    //range Y eagle + 3.7 - -3.25 , spolu je to 6,95 6,95/8 = 0,86875
    //kazdy krok sa posunie dole o 0,86875
    // initne y poziciu 0,86875 a x random od -5.4 po 5.4

    //so eagle should move 8 times to get to the chicken
    //set eagles spawn rate


    eagleStep = 6.95/8;


}

void GameScene::GameUI() {

    ImGui::Begin("Game stats");
    auto s=std::string("game score: ")+ std::to_string(score);
    ImGui::Text("%s", s.c_str());
    auto s2 = (std::string("lives: ")+ std::to_string(lives));
    ImGui::Text("%s", s2.c_str());
    ImGui::SliderInt("max lives ",&defaultLives, 2 ,10 );


    if(ImGui::Button("Reset/Restart Game"))
    {
        //restart
        ResetGame();
    }
    if(lives<=0)
        ImGui::TextWrapped("Game Over! enable cursor and pres button to go again");

    ImGui::End();

    //imgui
    // add reset button

    //add score counter


}


void GameScene::GameTimeStep() {
    //run this code each time

    float eagleY = eagle_OI->GetPos().y;

    stepTime-=stepSize;
    if(stepTime<0) {
        eagleY = eagleY -eagleStep;
        stepTime = defaultStepTime;
    }


    eagle_OI->SetPos(glm::vec3(eagle_OI->GetPos().x,eagleY,eagle_OI->GetPos().z));

    if( abs(glm::length(eagle_OI->GetPos()-playerChick_OI->GetPos()))  <0.8f)
    {
        //we caught the eagle
        score++;
        ReloadGame();
        return;
    }

    if (eagle_OI->GetPos().y< eagleCutoff)
    {
        if(lives==0) {
            std::cout << "Game Over";
            gameover = true;
        }
        else
        {
            std::cout<< "failed \n";
            //decrement lives
            lives--;
            stepTime = -1; // force this to prevent losing lives for the duration of next step
            //reset eagle to top
            ReloadGame();
            //set timer to default timer
        }
    }
}

void GameScene::Movement() {

    int iPlayer = camera->iPlayer;
    if(camera->lMove)
        iPlayer--;
    if(camera->rMove)
        iPlayer++;
    //iPlayer =  std::fmod(iPlayer,7); //not woring correctly if iPlayer negative

    if(allowBorderCross) {
        if (iPlayer < 0)
            iPlayer = playerSteps-1;
        else if (iPlayer > playerSteps-1)
            iPlayer = 0;
    }
    else {
        if (iPlayer < 0)
            iPlayer = 0;
        else if (iPlayer > playerSteps-1)
            iPlayer = playerSteps-1;
    }
    camera->iPlayer = iPlayer;
    float temp = 5.7f*2 /(float) (playerSteps-1);
    playerChick_OI->SetPos(glm::vec3(-5.7+ (iPlayer %playerSteps)*temp,playerChick_OI->GetPos().y,playerChick_OI->GetPos().z));
    //std::cout<< iPlayer << std::endl;
}

void GameScene::EndGame() {

}

void GameScene::ReloadGame() {

    // we should randonize X
    //v3 = rand() % 30 + 1985;   // v3 in the range 1985-2014
    //5,4*2
    //float a =  static_cast <float>(rand()) % 5.4f -5.4f;
    //float a = -5.4 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(5.4-(-5.4))));
    //https://stackoverflow.com/questions/686353/random-float-number-generation
    float a = rand() % 6 +0;
    float b =  -5.4f + a * (5.4f*2/6);
    eagle_OI->SetPos(glm::vec3(b,defaultEagleY,eagle_OI->GetPos().z));


}

void GameScene::ResetGame() {

    gameover = false;
    lives = defaultLives;
    score =0;
    ReloadGame();
}

void GameScene::DestroyGame() {

}
void GameScene::RenderSceneInstance(Shader *s, bool renderSelected)
{
    if(!gameover) {
        Movement();
        GameTimeStep();
    }
    GameUI();

    SceneInstance::RenderSceneInstance( s,  renderSelected);


}
GameScene::~GameScene() {

}

