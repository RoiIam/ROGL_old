//
// Created by RoiIam on 16. 10. 2023.
//

#include "PlanetGame.h"
PlanetGame::PlanetGame() {
    //enableMainUI = false; //comment for easier debug
}

void PlanetGame::Setup(Camera *cam, GraphicsOptions *graphicsOptions) {
    SceneInstance::Setup(cam, graphicsOptions);
    InitializeGame();
}

void PlanetGame::InitializeGame() {

    gameOver = false;
    //set camera to a defined spot
    camera->SetPosDir(glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f), -90, 0);
    //lock camera movement
    camera->cameraControlsUnlocked = false;

    playerChick_OI = new ObjectInstance(
            * new Model("../Assets/Models/Shrek/Shrek_mod.gltf"),
            basicTexturedShader, "player", nullptr);

    //load chicken and egg assets
    //we should use basicShader
    Shader *basicShaderB = new Shader("..\\Assets\\Shaders\\Forward\\basic.vert",
                                      "..\\Assets\\Shaders\\Forward\\basic.frag");
    basicShaderR = new Shader("..\\Assets\\Shaders\\Forward\\basic.vert", "..\\Assets\\Shaders\\Forward\\basic.frag");

    basicShaderB->use();
    basicShaderB->setVec4("col", glm::vec4(0.0, 0.0, 1.0, 1.0));
    basicShaderR->use();
    basicShaderR->setVec4("col", glm::vec4(1.0, 0.0, 0.0, 1.0));
    ball01_OI = new ObjectInstance(
            *new Model("../Assets/Models/sphere/sphere.obj"),
            *basicShaderB, "projectile", nullptr);
    ball02_OI = new ObjectInstance(
            *new Model("../Assets/Models/sphere/sphere.obj"),
            *basicShaderR, "planet1", nullptr);

    ball03_OI = new ObjectInstance(
            *new Model("../Assets/Models/sphere/sphere.obj"),
            *basicShaderR, "planet2", nullptr);

    target_OI = new ObjectInstance(
            *new Model("../Assets/Models/OwnCube/Cube.obj"),
            basicTexturedShader, "target_OI", nullptr);

    selectableObjInstances.push_back(playerChick_OI);
    selectableObjInstances.push_back(ball01_OI);
    selectableObjInstances.push_back(ball02_OI);
    selectableObjInstances.push_back(ball03_OI);
    selectableObjInstances.push_back(target_OI);

    //add to special list
    //planets_OIs.push_back(ball01_OI);// no this is our projectile...
    planets_OIs.push_back(ball02_OI);
    planets_OIs.push_back(ball03_OI);


    //set chicken..etc to position
    playerChick_OI->SetPos(glm::vec3(0.0f, -3.55f, -10.0f));
    playerChick_OI->SetScale(glm::vec3(0.05, 0.05, 0.05));
    //set chicken..etc to position
    ball01_OI->SetPos(glm::vec3(-5.5f, 0.0f, -10.0f));
    ball01_OI->SetScale(glm::vec3(0.25, 0.25, 0.25));
    //ball01_OI->SetDeg(-20.0, "Z");

    ball02_OI->SetPos(glm::vec3(-2.25f, 0.0f, -10.0f));
    ball02_OI->SetScale(glm::vec3(0.25, 0.25, 0.25));

    ball03_OI->SetPos(glm::vec3(2.0f, 1.2f, -10.0f));
    ball03_OI->SetScale(glm::vec3(0.25, 0.25, 0.25));

    target_OI->SetPos(glm::vec3(5.45f, 3.5f, -10.0f));
    target_OI->SetScale(glm::vec3(0.20,0.11,0.3));

    //set initial velocity
    //ball01_OI->velocity.x = 1.5f;

}

void PlanetGame::GameUI() {

    ImGui::Begin("Game stats");
    auto s=std::string("hits: ")+ std::to_string(score);
    ImGui::Text("%s", s.c_str());
    auto s2 = (std::string("misses: ")+ std::to_string(misses));
    ImGui::Text("%s", s2.c_str());

    auto s3 = (std::string("lives: ")+ std::to_string(lives));
    ImGui::Text("%s", s3.c_str());
    ImGui::SliderInt("max lives ",&defaultLives, 2 ,15 );

    ImGui::SliderFloat("projectileTimeToLive ",&defaultTimer, 2 ,15 );
    auto s4 = std::to_string(timer);
    ImGui::Text("remaining projectile TTL: %s ",  s4.c_str());


    ImGui::Checkbox("Switch pool vs border teleporting", &containInScreenSpace);
    float accel[] = {a.x, a.y, a.z};
    ImGui::SliderFloat3("acccel", accel, 0.05f, 100, "%.3f");
    a = glm::make_vec3(accel);

    float vel[] = {v.x, v.y, v.z};
    ImGui::SliderFloat3("vel ", vel, 0.05f, 100, "%.3f");
    v = glm::make_vec3(vel);

    float launchVel[] = {launchVelocity.x, launchVelocity.y, launchVelocity.z};
    ImGui::SliderFloat3("launch vel ", launchVel, -30, 30, "%.3f");
    launchVelocity = glm::make_vec3(launchVel);

    if (ImGui::Button("Launch Blue ball with launch vel"))
    {
        shoot = true;
        timer = defaultTimer;
        ball01_OI->velocity.x =launchVelocity.x;
        ball01_OI->velocity.y =launchVelocity.y;
    }

    ImGui::SliderFloat("shot multiplier ", &finalShotStrength, 0.01f, 15);

    ImGui::SliderFloat("drag ammount ", &drag, 0.00f, 5);
    if (ImGui::Checkbox("Switch drag of the balls", &enableDrag)) {}
    if (enableDrag)
        ImGui::SliderFloat("global drag ammount ", &drag, 0.00f, 5);


    if (ImGui::Button("Reset/Restart Game")) {
        //restart
        ResetGame();
    }
    ImGui::End();
}


void PlanetGame::GameTimeStep() {
    //run this code each frame
    //nastavme projektil hned nad hlavu, pokial sa nestriela
    glm::vec3 newPos = glm::vec3();

    glm::mat4 lm = glm::mat4(1.0f);
    lm = glm::translate(lm, playerChick_OI->GetPos());
    lm = glm::rotate(lm, glm::radians(playerChick_OI->GetDeg("Z")), glm::vec3(0, 0, 1)); //z

    if(lives==0) {
        std::cout << "Game Over";
        gameOver = true;
    }

    //ak nestrielame ale mierime
    if(!shoot) {
        newPos = playerChick_OI->GetPos() + playerOffset * glm::vec3(lm[1][0], lm[1][1], lm[1][2]);
        ball01_OI->SetPos(newPos);
    }

    else
    {



        //ak sme zasiahli ciel, teda, iba ich pozicia je veelmi blizko seba, ziadne kolizie
        if( abs(glm::length(target_OI->GetPos()-ball01_OI->GetPos()))  <0.4f)
        {
            score++;
            std::cout << "hit!, try some more.. " << std::endl;
            //reset target pos also
            float a = rand() % 6 +0;
            float b =  -5.2f + a * (5.4f*2/6);
            target_OI->SetPos(glm::vec3(b,3.5f,target_OI->GetPos().z));
            ReloadGame();
            return;
        }

        timer -=Managers::deltaTime;
        if(timer<=0)
        {
            std::cout<< "Projectile exploded without hitting the target. Reloading... \n";
            lives--;
            //reset projectile
            shoot = false;
            misses +=1;
        }
    }

    //nastavme pociatocne rychlosti podla uhla a sily
    if(camera->shootSpace && !shoot)
    {
        shoot = true;
        timer = defaultTimer;
        ball01_OI->velocity.x = shotStrengthPercent * finalShotStrength * cos(glm::radians(shotAngle));
        ball01_OI->velocity.y = shotStrengthPercent * finalShotStrength * sin(glm::radians(shotAngle));
    }
    //get strength, ziskavanie uhla a sily na vystrel
    shotStrengthPercent= playerChick_OI->GetScale().y;
    shotAngle = playerChick_OI->GetDeg("Z") + 90.0; //should we remap? yea
    shotStrengthPercent = (shotStrengthPercent - 0.008f) * 23.81f;
}

void PlanetGame::SolveCollision() {


}

bool PlanetGame::TestCollision(int i, int j, float &dx, float &dy, float &d) {
    float radius = 0.25f;
    dx = planets_OIs[j]->GetPos().x - planets_OIs[i]->GetPos().x;
    dy = planets_OIs[j]->GetPos().y - planets_OIs[i]->GetPos().y;
    d = sqrt(dx * dx + dy * dy);

    if (d < radius * 2) {
        return true;
    }
    return false;

}

void PlanetGame::ManagePlanets() {


    if(!shoot)
        return;
    force = glm::vec3(0);
    for (int i = 0; i < planets_OIs.size(); ++i) {
        //pee = planets_OIs[i]->mass / drag;
        //glm::vec3 newPos = glm::vec3(planets_OIs[i]->GetPos().x + planets_OIs[i]->velocity.x * Managers::deltaTime,
        //                             planets_OIs[i]->GetPos().y + planets_OIs[i]->velocity.y * Managers::deltaTime,
        //                             planets_OIs[i]->GetPos().z
        //);

        if (enableDrag) {
            //planets_OIs[i]->velocity.x *= pow(e, -Managers::deltaTime / pee);
            //planets_OIs[i]->velocity.y *= pow(e, -Managers::deltaTime / pee);
        }
        glm::vec3 delta = glm::vec3(0);
        delta.x = planets_OIs[i]->GetPos().x - ball01_OI->GetPos().x;
        delta.y = planets_OIs[i]->GetPos().y - ball01_OI->GetPos().y;
        float r = sqrt(delta.x*delta.x+delta.y*delta.y);
        if(r<1.0) r=1.0f;
        //potrebujem malu konstantu
        float f = 10 *   ball01_OI->mass * planets_OIs[i]->mass /(r*r);
        //sily akumulujeme
        force += glm::vec3(f*delta.x/r,f*delta.y/r,0);

    }
    //ked pridame dalsi objekt musime to tu oddelit
    //std::cout << "delta " << glm::to_string(delta) << std::endl;
    //std::cout << "f " << f << std::endl;
    //std::cout << "force  " << glm::to_string(force) << std::endl;

    //zrychlenie //tu moze byt problem, lebo my nastavujeme rychlost pri vystrele
    ball01_OI->acceleration = force / ball01_OI->mass;
    //rychlost
    ball01_OI->velocity =  ball01_OI->velocity +  ball01_OI->acceleration* Managers::deltaTime;
    //pos
    ball01_OI->SetPos(ball01_OI->GetPos() +  ball01_OI->velocity* Managers::deltaTime);

}

void PlanetGame::Movement() {


    int iPlayer =0; //horizontal mov
    if(camera->leftArrowHold)
        iPlayer =-1;
    if(camera->rightArrowHold)
        iPlayer =1;

    glm::vec3 oldPos = playerChick_OI->GetPos();
    if(abs(oldPos.x + 0.5f*iPlayer)< 5.6f) {
        oldPos.x += 1.0f * iPlayer* Managers::deltaTime;
        playerChick_OI->SetPos(oldPos);
        //std::cout<< iPlayer << std::endl;
    }

    //ziskajme uhol
    float temp = -80.0f * camera->DKey + 80.0f * camera->AKey;
    temp *= Managers::deltaTime;
    float curAngle = ball01_OI->GetDeg("Z");
    //std::cout << "curAngle: " <<curAngle << std::endl;
    if (curAngle + temp < 76 && curAngle + temp > -76)//add an offset to avoid lockups
    {
        ball01_OI->SetDeg(curAngle + temp, "Z");
        playerChick_OI->SetDeg(curAngle + temp, "Z");
    }
    //now add scale along y as strength of the shot
    glm::vec3 curSize = playerChick_OI->GetScale();
    float newSize = -0.15f * camera->downArrow + 0.15f * camera->upArrow; //TODO deltatime
    newSize *= Managers::deltaTime;

    //std::cout << "curSize: " << glm::to_string(curSize) << std::endl;
    if (curSize.y + newSize > 0.008 && curSize.y + newSize < 0.052)//add an offset to avoid lockups
        playerChick_OI->SetScale(glm::vec3(curSize.x, curSize.y + newSize, curSize.z));

}

void PlanetGame::CreateNewBall(glm::vec3 coords) {
//dont

}


void PlanetGame::EndGame() {

}

void PlanetGame::ReloadGame() {
    //only set shooting as false and reset the velocity
    v = glm::vec3(0, 0, 0);
    shoot = false;

}

void PlanetGame::ResetGame() {
    gameOver = false;
    shoot = false;
    lives = defaultLives;
    score =0;
    misses = 0;
    ReloadGame();
}

void PlanetGame::DestroyGame() {

}

void PlanetGame::RenderSceneInstance(Shader *s, bool renderSelected) {
    if(!gameOver) {
        Movement();
        GameTimeStep();
        ManagePlanets();
    }
    GameUI();
    SceneInstance::RenderSceneInstance(s, renderSelected);


}

PlanetGame::~PlanetGame() {

}
