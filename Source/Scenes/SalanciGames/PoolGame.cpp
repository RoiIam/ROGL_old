//
// Created by RoiIam on 10. 10. 2023.
//

#include "PoolGame.h"


PoolGame::PoolGame() {
    //enableMainUI = false; //comment for easier debug
}

void PoolGame::Setup(Camera *cam, GraphicsOptions *graphicsOptions) {
    SceneInstance::Setup(cam, graphicsOptions);
    InitializeGame();
}

void PoolGame::InitializeGame() {

    //set camera to a defined spot
    camera->SetPosDir(glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f), -90, 0);
    //lock camera movement
    camera->cameraControlsUnlocked = false;
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
            *basicShaderB, "player_chick", nullptr);
    ball02_OI = new ObjectInstance(
            *new Model("../Assets/Models/sphere/sphere.obj"),
            *basicShaderR, "chickGround", nullptr);


    selectableObjInstances.push_back(ball01_OI);
    selectableObjInstances.push_back(ball02_OI);

    //add to special list
    colliders_OIs.push_back(ball01_OI);
    colliders_OIs.push_back(ball02_OI);



    //set chicken..etc to position
    ball01_OI->SetPos(glm::vec3(-5.5f, 0.0f, -10.0f));
    ball01_OI->SetScale(glm::vec3(0.25, 0.25, 0.25));
    //ball01_OI->SetDeg(-20.0, "Z");

    ball02_OI->SetPos(glm::vec3(0.0f, 0.0f, -10.0f));
    ball02_OI->SetScale(glm::vec3(0.25, 0.25, 0.25));

    //set initial velocity
    //ball01_OI->velocity.x = 1.5f;

}

void PoolGame::GameUI() {

    ImGui::Begin("Game stats");
    ImGui::Checkbox("Switch pool vs border teleporting", &containInScreenSpace);
    auto stringSKE = std::to_string(systemKineticE);
    ImGui::Text("total system kinetic energy: %s ", stringSKE.c_str());
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
        ball01_OI->velocity.x =launchVelocity.x;
        ball01_OI->velocity.y =launchVelocity.y;
    }

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


void PoolGame::GameTimeStep() {
    //run this code each frame
    //nastavme projektil hned nad hlavu, pokial sa nestriela
    glm::vec3 newPos = glm::vec3();

    float e = 2.7182818284f;
    float pee;
    for (int i = 0; i < colliders_OIs.size(); ++i) {
        pee = colliders_OIs[i]->mass / drag;
        newPos = glm::vec3(colliders_OIs[i]->GetPos().x + colliders_OIs[i]->velocity.x * Managers::deltaTime,
                           colliders_OIs[i]->GetPos().y + colliders_OIs[i]->velocity.y * Managers::deltaTime,
                           colliders_OIs[i]->GetPos().z
        );

        if (enableDrag) {
            colliders_OIs[i]->velocity.x *= pow(e, -Managers::deltaTime / pee);
            colliders_OIs[i]->velocity.y *= pow(e, -Managers::deltaTime / pee);
        }
        //ak nahodou pridemem mimo obrazovky, supnime ho na druhu stranu
        //este pre bonus otoc velocity

        if(containInScreenSpace) {
            if (newPos.x > 6.25f) {
                newPos.x = -6.25f;
            }
            if (newPos.x < -6.25f) {
                newPos.x = 6.25f;
            }
            if (newPos.y < -4.0f) {
                newPos.y = 4.0f;
            }
            if (newPos.y > 4.0f) {
                newPos.y = -4.0f;
            }
        }
        else
        {
            if (newPos.x > 6.25f || newPos.x < -6.25f) {
                //glm::mat4 lm=glm::mat4(1.0f);
                //float cosine= glm::dot(glm::normalize(colliders_OIs[i]->velocity),glm::vec3(0,1,0));
                //float final = acos(cosine);
                //final = glm::degrees(final);
                //std::cout<< "final " << final<< std::endl;
                //lm = glm::rotate(lm, glm::radians(45.0f), glm::vec3(0,0,1) );
                //colliders_OIs[i]->velocity = glm::vec3(lm[1][0], lm[1][1], lm[1][2]);
                colliders_OIs[i]->velocity.x =-colliders_OIs[i]->velocity.x;
            }
            if (newPos.y < -4.0f || newPos.y > 4.0f) {
                colliders_OIs[i]->velocity.y =-colliders_OIs[i]->velocity.y;
            }
        }
        colliders_OIs[i]->SetPos(newPos);

    }

    /*newPos =glm::vec3(ball01_OI->GetPos().x +ball01_OI->velocity.x*Managers::deltaTime,
                      ball01_OI->GetPos().y+ball01_OI->velocity.y*Managers::deltaTime,
                      ball01_OI->GetPos().z
                      );
    */
    systemKineticE = 0;
    for (int i = 0; i < colliders_OIs.size(); ++i) {

        systemKineticE += colliders_OIs[i]->velocity.x * colliders_OIs[i]->velocity.x +
                 colliders_OIs[i]->velocity.y * colliders_OIs[i]->velocity.y;
    }
    //std::cout << "kinetic e " << SystemKineticE << std::endl;

}

void PoolGame::SolveCollision(collision c) {

    if (c.d <= 0)
        c.d = 0.00001f;
    float nx = c.dx / c.d;
    float ny = c.dy / c.d;
    glm::vec3 n = glm::vec3(nx, ny, 0);
    //std::cout << "err: " << c.d << std::endl;

    float s = 0.25f * 2 - c.d;

    glm::vec3 newPos = colliders_OIs[c.i]->GetPos();
    newPos.x = colliders_OIs[c.i]->GetPos().x - nx * s / 2.0;
    newPos.y = colliders_OIs[c.i]->GetPos().y - ny * s / 2.0;
    colliders_OIs[c.i]->SetPos(newPos);
    //now the second object
    newPos = colliders_OIs[c.j]->GetPos();
    newPos.x = colliders_OIs[c.j]->GetPos().x + nx * s / 2.0;
    newPos.y = colliders_OIs[c.j]->GetPos().y + ny * s / 2.0;
    colliders_OIs[c.j]->SetPos(newPos);

    /* taken from lecture
    k = –2 * ((o2.vx – o1.vx) * nx + (o2.vy – o1.vy) * ny) / (1 / o1.m + 1 / o2.m)
    o1.vx = o1.vx – k * nx / o1.m
    o1.vy = o1.vy – k * ny / o1.m
    o2.vx = o2.vx + k * nx / o2.m
    o2.vy = o2.vy + k * ny / o2.m
    */
    float k = -2.0f * ((colliders_OIs[c.j]->velocity.x - colliders_OIs[c.i]->velocity.x) * nx +
                       (colliders_OIs[c.j]->velocity.y - colliders_OIs[c.i]->velocity.y) * ny) /
              (1.0 / colliders_OIs[c.i]->mass + 1.0 / colliders_OIs[c.j]->mass);
    colliders_OIs[c.i]->velocity = colliders_OIs[c.i]->velocity - k * n / colliders_OIs[c.i]->mass;
    colliders_OIs[c.j]->velocity = colliders_OIs[c.j]->velocity + k * n / colliders_OIs[c.j]->mass;

}

bool PoolGame::TestCollision(int i, int j, float &dx, float &dy, float &d) {
    float radius = 0.25f;
    dx = colliders_OIs[j]->GetPos().x - colliders_OIs[i]->GetPos().x;
    dy = colliders_OIs[j]->GetPos().y - colliders_OIs[i]->GetPos().y;
    d = sqrt(dx * dx + dy * dy);

    if (d < radius * 2) {
        return true;
    }
    return false;

}

void PoolGame::ManageCollisions() {
    //zoberme zoznam vsetkych objektov

    std::pair<int, int> p = {1, 2}; // neviem ci je toto najvhodnejsie, ale pojde to
    //std::vector<ObjectInstance *> selectableObjInstances;
    std::vector<std::pair<int, int>> vp; //tu si ulozime indexy parov

    std::vector<collision> colS; //este lepsie



    for (int i = 0; i < colliders_OIs.size(); ++i) {
        for (int j = i + 1; j < colliders_OIs.size(); ++j) {

            //std::cout <<"hehe " << std::endl;
            //zisti koliziu
            float dx = 0;
            float dy = 0;
            float d = 0;
            bool collision = TestCollision(i, j, dx, dy, d);
            if (collision) {
                //vp.push_back(std::pair<int, int>(i,j));
                struct collision newCol;
                newCol.i = i;
                newCol.j = j;
                newCol.dx = dx;
                newCol.dy = dy;
                newCol.d = d;
                colS.push_back(newCol);
            }
            //ak kolizia pridaj do vp pairs
        }
    }
    //vyries kolizie pre kazdy par
    //for (int i = 0; i < vp.size(); ++i)
    //std::cout << "colision Pairs size " << colS.size() <<std::endl;
    for (int i = 0; i < colS.size(); ++i) {
        //SolveCollision(vp[i]);
        SolveCollision(colS[i]);
        //std::cout << "colided " << vp[i].first << " "<<vp[i].second <<std::endl;
    }

    vp.clear();
    colS.clear();


}

void PoolGame::Movement() {

    //ziskajme uhol
    float temp = -80.0f * camera->rightArrow + 80.0f * camera->leftArrow;
    temp *= Managers::deltaTime;
    float curAngle = ball01_OI->GetDeg("Z");
    //std::cout << "curAngle: " <<curAngle << std::endl;
    if (curAngle + temp < -1 && curAngle + temp > -76)//add an offset to avoid lockups
        ball01_OI->SetDeg(curAngle + temp, "Z");

    //now add scale along y as strength of the shot
    glm::vec3 curSize = ball01_OI->GetScale();
    float newSize = -0.15f * camera->downArrowHold + 0.15f * camera->upArrowHold; //TODO deltatime
    newSize *= Managers::deltaTime;

    //std::cout << "curSize: " << curSize << std::endl;
    if (curSize.y + newSize > 0.008 && curSize.y + newSize < 0.052)//add an offset to avoid lockups
        ball01_OI->SetScale(glm::vec3(curSize.x, curSize.y + newSize, curSize.z));

}

void PoolGame::CreateNewBall(glm::vec3 coords) {
    ObjectInstance *newBall_OI = new ObjectInstance(
            *new Model("../Assets/Models/sphere/sphere.obj"),
            *basicShaderR, "newBall", nullptr);

    selectableObjInstances.push_back(newBall_OI);
    colliders_OIs.push_back(newBall_OI);
    newBall_OI->SetScale(glm::vec3(0.25f));

    //newBall_OI->SetPos(glm::normalize(coords)*10.0f); //not correct
    newBall_OI->SetPos(coords * 10.0f);

}


void PoolGame::EndGame() {

}

void PoolGame::ReloadGame() {
    //only set shooting as false and reset the velocity
    v = glm::vec3(0, 0, 0);
}

void PoolGame::ResetGame() {
    ReloadGame();
}

void PoolGame::DestroyGame() {

}

void PoolGame::RenderSceneInstance(Shader *s, bool renderSelected) {
    Movement();
    GameTimeStep();
    ManageCollisions();
    GameUI();
    SceneInstance::RenderSceneInstance(s, renderSelected);


}

PoolGame::~PoolGame() {

}
