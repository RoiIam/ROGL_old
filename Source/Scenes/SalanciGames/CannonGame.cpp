//
// Created by RoiIam on 4. 10. 2023.
//

#include "CannonGame.h"


CannonGame::CannonGame() {
    //enableMainUI = false; //comment for easier debug
}

void CannonGame::Setup(Camera *cam, GraphicsOptions *graphicsOptions)  {
    SceneInstance::Setup(cam,graphicsOptions);
    InitializeGame();
}

void CannonGame::InitializeGame() {

    gameOver = false;
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
    playerChick_OI->SetPos(glm::vec3(-5.5f, -3.55f, -10.0f));
    playerChick_OI->SetScale(glm::vec3(0.05, 0.05, 0.05));
        playerChick_OI->SetDeg(-20.0, "Z");

    //chick_OI->SetPos(glm::vec3(0.0f, -3.75f, -10.0f));
    chick_OI->SetScale(glm::vec3(0.25, 0.2, 0.25));

    eagle_OI->SetPos(glm::vec3(5.45f, -3.3f, -10.0f));
    eagle_OI->SetScale(glm::vec3(0.20,0.11,0.3));

}

void CannonGame::GameUI() {

    ImGui::Begin("Game stats");
    auto s=std::string("hits: ")+ std::to_string(score);
    ImGui::Text("%s", s.c_str());
    auto s2 = (std::string("misses: ")+ std::to_string(misses));
    ImGui::Text("%s", s2.c_str());


    float accel[] = {a.x,a.y,a.z};
    ImGui::SliderFloat3("acccel",accel, 0.05f,  100, "%.3f");
    a = glm::make_vec3(accel);

    float vel[] = {v.x,v.y,v.z};
    ImGui::SliderFloat3("vel ",vel, 0.05f,  100, "%.3f");
    v= glm::make_vec3(vel);

    //this part of ui will not be updated when shot is flying, we should fix it
    ImGui::SliderFloat("shot strength multiplier ", &finalShotStrength, 5 , 50 );
    ImGui::Text("%s", ("shot strength " + std::to_string((int)(shotStrengthPercent * 100)) + " %").c_str());
    ImGui::Text("%s", ("shot angle "+ std::to_string(shotAngle)).c_str());

    ImGui::SliderFloat("player offset ",&playerOffset, 0.05f,  5);


    if(ImGui::Button("Reset/Restart Game"))
    {
        //restart
        ResetGame();
    }

    ImGui::End();

}


void CannonGame::GameTimeStep() {
    //run this code each frame
    //nastavme projektil hned nad hlavu, pokial sa nestriela
    glm::vec3 newPos = playerChick_OI->GetPos();

    //this works but is dependent on scale too
    //glm::mat4 lm = playerChick_OI->GetTransformMat();
    //newPos = playerChick_OI->GetPos()+ 205.0f * glm::vec3(lm[1][0],lm[1][1],lm[1][2]);
    //src https://community.khronos.org/t/get-direction-from-transformation-matrix-or-quat/65502
    glm::mat4 lm = glm::mat4(1.0f);
    lm = glm::translate(lm, playerChick_OI->GetPos());
    lm = glm::rotate(lm, glm::radians(playerChick_OI->GetDeg("Z")), glm::vec3(0, 0, 1)); //z

    //ak nestrielame ale mierime
    if(!shoot) {
        newPos = playerChick_OI->GetPos() + playerOffset * glm::vec3(lm[1][0], lm[1][1], lm[1][2]);
        chick_OI->SetPos(newPos);
    }
    //ak sme vystrelili pocitajme rychlost, nove suradnice objektov
    else
    {
        //    vx = vx + ax * dt
        //    vy = vy + ay * dt
        //    x = x + vx * dt
        //    y = y + vy * dt
        // troska to zmenne
        v = glm::vec3( v.x,
                       v.y+a.y*Managers::deltaTime,
                      v.z+a.z*Managers::deltaTime);

        newPos =glm::vec3(chick_OI->GetPos().x + v.x*Managers::deltaTime,
                          chick_OI->GetPos().y + v.y*Managers::deltaTime - 1.0/2*a.y *(Managers::deltaTime*Managers::deltaTime),
                          chick_OI->GetPos().z + v.z*Managers::deltaTime
        );
        chick_OI->SetPos(newPos);

        //ak sa nam prepadol objekt hlboko pod zem/uroven ciela
        if(chick_OI->GetPos().y<-12.0f) {
            std::cout << "missed!, try again.. " << std::endl;
            misses +=1;
            //std::cout << "posX " << chick_OI->GetPos().x << std::endl;
            ReloadGame();
        }
        //ak sme zasiahli ciel, teda, iba ich pozicia je veelmi blizko seba, ziadne kolizie
        if( abs(glm::length(eagle_OI->GetPos()-chick_OI->GetPos()))  <0.4f)
        {
            score++;
            std::cout << "hit!, try some more.. " << std::endl;
            //reset target pos also
            float a = rand() % 6 +0;
            float b =  -5.2f + a * (5.4f*2/6);
            eagle_OI->SetPos(glm::vec3(b,-3.3f,eagle_OI->GetPos().z));
            ReloadGame();
            return;
        }

        return; //we dont want to overwrite values set below
    }

    //nastavme pociatocne rychlosti podla uhla a sily
    if(camera->shootSpace)
    {
        shoot = true;
        v.x = shotStrengthPercent * finalShotStrength * cos(glm::radians(shotAngle));
        v.y = shotStrengthPercent * finalShotStrength * sin(glm::radians(shotAngle));
    }

    //get strength, ziskavanie uhla a sily na vystrel
    shotStrengthPercent= playerChick_OI->GetScale().y;
    shotAngle = playerChick_OI->GetDeg("Z") + 90.0; //should we remap? yea
    //remap 0-100
    //0.008 je min + 0,042 = 0,05 max
    // teda kedze chceme od nuly, musime vstup opdocitat minimum, teda mame 0,042
    //1/0,042 je cca 23,81
    //teraz size- minSize * 23,81 vypocitana konstanta
    shotStrengthPercent = (shotStrengthPercent - 0.008f) * 23.81f;
    //std::cout << "shootAngle: " << shootAngle << std::endl;
    //std::cout << "strength: " << strength << std::endl;
}

void CannonGame::Movement() {

    //ziskajme uhol
    //same here  we dont use camera->rightArrow and *camera->leftArrow bcs its one press
    // but use camera->rMove and camera->lMove
    // note now its using A and D to rotate
    //actually we added leftArrowHold and rightArrowHold
    float temp = -80.0f*camera->rightArrowHold + 80.0f*camera->leftArrowHold;
    temp *= Managers::deltaTime;
    float curAngle = playerChick_OI->GetDeg("Z");
    //std::cout << "curAngle: " <<curAngle << std::endl;
    if (curAngle+temp <-1 && curAngle+temp >-76 )//add an offset to avoid lockups
        playerChick_OI->SetDeg(curAngle + temp, "Z");

    //now add scale along y as strength of the shot
    glm::vec3 curSize = playerChick_OI->GetScale();
    float newSize = -0.15f*camera->downArrow + 0.15f*camera->upArrow; //TODO deltatime
    newSize *= Managers::deltaTime;

    //std::cout << "curSize: " << curSize << std::endl;
    if (curSize.y+newSize >0.008 && curSize.y+newSize <0.052 )//add an offset to avoid lockups
        playerChick_OI->SetScale(glm::vec3(curSize.x,curSize.y+newSize,curSize.z));

}

void CannonGame::EndGame() {

}

void CannonGame::ReloadGame() {

    //only set shooting as false and reset the velocity
    shoot = false;
    v = glm::vec3(0,0,0);

}

void CannonGame::ResetGame() {

    gameOver = false;
    score =0;
    misses = 0;
    ReloadGame();
}

void CannonGame::DestroyGame() {

}
void CannonGame::RenderSceneInstance(Shader *s, bool renderSelected)
{
    if(!gameOver) {
        Movement();
        GameTimeStep();
    }
    GameUI();

    SceneInstance::RenderSceneInstance( s,  renderSelected);


}
CannonGame::~CannonGame() {

}
