//
// Created by RoiIam on 17. 10. 2023.
//

#include "CoinMapGame.h"


CoinMapGame::CoinMapGame() {
    //enableMainUI = false; //comment for easier debug
}

void CoinMapGame::Setup(Camera *cam, GraphicsOptions *graphicsOptions) {
    SceneInstance::Setup(cam, graphicsOptions);
    InitializeGame();
}


glm::vec3 CoinMapGame::TranslateToWorld(glm::vec3 ndcVec) {
    //TODO fix the z value
    glm::vec3 result;
    //teraz to mame tak, ze obrazovka je taka velka ako obrazok,
    //mozme pouzivat relativne suradnice
    //cize vrcholy mozme namapovat v rozsahoch x,y 0-1
    // nasledne pre reprezentaciu v scene ich musime "namapovat"
    // spat do -orthoApect*orthoScale az +orthoApect*orthoScale
    //zoberieme vzorec z initgame
    //(+-x+10)/20=i pre x=-5 je to 0.25=i
    // napiseme ho aby sme vedeli ratat x , x= i*scale*scale-scale
    //skuska pre 0.25... 0.25*20-10 = -5
    //skuska pre 0 je +10 0*20-10 =-10 az nato ze scale*apsect*2

    // pre y by to malo byt pre 1 rovne 10 a pre 0 -10
    // cize y = 1*scale*2 -scale
    float aspect = (float) windowSettings->CUR_WIDTH / (float) windowSettings->CUR_HEIGHT;
    result = glm::vec3(ndcVec.x * camera->orthoScale * aspect * 2 - camera->orthoScale * aspect,
                       ndcVec.y * camera->orthoScale * 2 - camera->orthoScale,
                       -10);

    return result;
}

void CoinMapGame::InitMap() {
    //teraz nam idu suradnice l -15 - p+15 prex a pre y bottom -10 top +10
    //15 je to preto lebo top a bottom su scale+- a left right su  aspect*scale
    // cize L/R su width/height * 10, kedze W,H mam 1200/800 tak aspect je 1.5
    //1,5*10 je 15
    //vhodne by bolo dat si to do 0-1
    // 0,0 bude vlavo dole?
    // pre y by to bolo -scale je 0 a +scale je 1 cize -10 bude 0 a +10 bude 1
    //teda len dame offset (+-x+10)/20 skuska pre -5, malo by byt 0.25

    //initialize map
    /*
     *
    mouse pos50, 511  norm 0.052521, 0.760417
    mouse pos175, 619 norm 0.183824, 0.921131
    mouse pos151, 529 norm 0.158613, 0.787202

    mouse pos245, 106 norm 0.257353, 0.157738
    mouse pos761, 552 norm 0.79937, 0.821429
    mouse pos677, 382 norm 0.711134, 0.568452

    mouse pos892, 401 norm 0.936975, 0.596726
    mouse pos856, 216 norm 0.89916, 0.321429
    mouse pos627, 100 norm 0.658613, 0.14881

    mouse pos396, 86 norm  0.415966, 0.127976

     */
    float z = 0;
    vertex.push_back(glm::vec3(0.052521, 0.760417, z)); // i 0
    vertex.push_back(glm::vec3(0.183824, 0.921131, z)); // i 1
    vertex.push_back(glm::vec3(0.158613, 0.787202, z));// i 2

    vertex.push_back(glm::vec3(0.257353, 0.157738, z));// i 3
    vertex.push_back(glm::vec3(0.79937, 0.821429, z));// i 4
    vertex.push_back(glm::vec3(0.711134, 0.568452, z));// i 5

    vertex.push_back(glm::vec3(0.936975, 0.596726, z));// i 6
    vertex.push_back(glm::vec3(0.89916, 0.321429, z));// i 7
    vertex.push_back(glm::vec3(0.658613, 0.14881, z));// i 8

    vertex.push_back(glm::vec3(0.415966, 0.127976, z));// i 9

    //see mapIDs.png, non directional
    edges.push_back(std::pair<int, int>(0, 2));
    edges.push_back(std::pair<int, int>(2, 1));
    edges.push_back(std::pair<int, int>(2, 3));
    edges.push_back(std::pair<int, int>(2, 5));

    edges.push_back(std::pair<int, int>(5, 4));
    edges.push_back(std::pair<int, int>(5, 6));
    edges.push_back(std::pair<int, int>(5, 8));

    edges.push_back(std::pair<int, int>(8, 7));
    edges.push_back(std::pair<int, int>(8, 9));


}

void CoinMapGame::InitializeGame() {

    InitMap();
    CreatePolyMap();
    //image size is 595 *420 ratio 1,416
    //we want it like *1.6 so 952 x 672
    ResizeWindow(952, 672); //hardcoded for now
    //interesting problem, my texture mapa.png had 24bit depth
    //so when loading it, it was skewed to a side...
    //i now use 32bit represeted resaved image
    mapPlane = new Grass("mapa.png");
    //mapPlane = Grass();

    grassShader = new Shader("..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.vert",
                             "..\\Assets\\Shaders\\Forward\\Transparent\\transparentGrass.frag");

    mapPlane_OI = new ObjectInstance(
            *mapPlane,
            *grassShader, "playBoard", nullptr);

    gameOver = false;
    //set camera to a defined spot
    camera->SetPosDir(glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f), -90, 0);
    //lock camera movement
    camera->cameraControlsUnlocked = false;
    camera->orthoScale = 10.0f;
    //also set ortho cam
    camera->cameraPerspective = Camera_Perspective::ORTHO;
    camera->hideCursor = false;
    //load chicken and egg assets
    //we should use basicShader
    playerChick_OI = new ObjectInstance(
            *new Model("../Assets/Models/Shrek/Shrek_mod.gltf"),
            basicTexturedShader, "player_chick", nullptr);
    chick_OI = new ObjectInstance(
            *new Model("../Assets/Models/sphere/sphere.obj"),
            basicTexturedShader, "chickGround", nullptr);
    eagle_OI = new ObjectInstance(
            *new Model("../Assets/Models/OwnCube/Cube.obj"),
            basicTexturedShader, "eagle", nullptr);

    selectableObjInstances.push_back(playerChick_OI);
    selectableObjInstances.push_back(chick_OI);
    selectableObjInstances.push_back(eagle_OI);
    selectableObjInstances.push_back(mapPlane_OI);


    //set chicken..etc to position
    playerChick_OI->SetPos(TranslateToWorld(vertex[0]));
    playerChick_OI->SetScale(glm::vec3(0.05, 0.05, 0.05));
    //playerChick_OI->SetDeg(-20.0, "Z");

    //chick_OI->SetPos(glm::vec3(0.0f, -3.75f, -10.0f));
    chick_OI->SetScale(glm::vec3(0.25, 0.25, 0.25));

    eagle_OI->SetPos(glm::vec3(-13.0f, -3.3f, -10.0f));
    eagle_OI->SetScale(glm::vec3(0.20, 0.11, 0.3));

    float scale = camera->orthoScale;
    float aspect = (float) windowSettings->CUR_WIDTH / (float) windowSettings->CUR_HEIGHT;
    mapPlane_OI->SetPos(glm::vec3(-aspect * scale, 0.0f, -20.0f));
    mapPlane_OI->SetScale(glm::vec3(aspect * scale * 2, scale * 2, 1));

    //mapPlane_OI->SetDeg(180.0, "Y"); //not needed

    /*for (int i = 0; i < polys.size(); ++i) {
        for (int j = 0; j < polys[i].size(); ++j) {
            std::cout << polys[i][j] << " ";
        }
        std::cout<< std::endl;
    }*/
}

void CoinMapGame::GameUI() {

    ImGui::Begin("Game stats");
    auto s = std::string("hits: ") + std::to_string(score);
    ImGui::Text("%s", s.c_str());
    auto s2 = (std::string("misses: ") + std::to_string(misses));
    ImGui::Text("%s", s2.c_str());


    float accel[] = {a.x, a.y, a.z};
    ImGui::SliderFloat3("acccel", accel, 0.05f, 100, "%.3f");
    a = glm::make_vec3(accel);

    float vel[] = {v.x, v.y, v.z};
    ImGui::SliderFloat3("vel ", vel, 0.05f, 100, "%.3f");
    v = glm::make_vec3(vel);

    ImGui::Checkbox("Enable Polymap(T) or mouse clicking(F)", &usePolyMap);
    ImGui::Checkbox("Enable test mouse click", &testMouse);

    if (ImGui::Button("Reset/Restart Game")) {
        //restart
        ResetGame();
    }

    auto polyIdUI = std::string("palyer at poly id ") + std::to_string(polyId);
    ImGui::Text("%s", polyIdUI.c_str());

    ImGui::End();

}

bool CoinMapGame::TestPoly(glm::vec3 testV = glm::vec3(0., 0., 0.0)) {
    //test ci lezi v polygone A B C
    // bod x,y lezi v polygone polys[i][j-0,size()] ak
    //(AY – BY)*(x – AX) + (BX – AX) * (y – AY) ≥ 0 &&
    //(BY – CY) * (x – BX) + (CX – BX) * (y – BY) >=0 &&
    //(CY – AY) * (x – CX) + (AX – CX) * (y – CY) ≥ 0
    //pre tri a takto pre vseobecne
    // (vrch[i].y -vrch[i+1].y) * (x-vrch[i].x) + (vrch[i+1].x-vrch[i].x) *(y-vrch[i].y) >= 0 &&
    //zvysuje sa i a teda aj i+1 a checkuje dalsi vrchol polygonu
    //(vrch[i].y -vrch[i+1].y) * (x-vrch[i].x) + (vrch[i+1].x-vrch[i].x) *(y-vrch[i].y)
    //teda
    //(verts(polys[i][0],1) – verts(polys[i][1],1))*(x – verts(polys[i][0],0))
    // + (verts(polys[i][1],0) – verts(polys[i][0],0)) * (y – verts(polys[i][0],1)) ≥ 0 &&
    //to bol len prvy riadok
    //druhy bude
    //(BY – CY) * (x – BX) + (CX – BX) * (y – BY) &&

    float x, y;
    x = camera->xMousePos;
    y = camera->yMousePos;

    if (glm::length(testV) <= 0.2f) {
        camera->MouseMovementNormalized(x, y, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT);
    } else {
        x = testV.x;
        y = testV.y;
    }

    bool isInside = true;
    for (int v = 0; v < polys.size(); ++v) {
        //std::cout<< "check v at " << v << std::endl;
        isInside = true;
        for (int i = 0; i < polys[i].size() - 1; ++i) { //do it for the second last
            //(vrch[i].y -vrch[i+1].y) * (x-vrch[i].x) +
            // (vrch[i+1].x-vrch[i].x) *(y-vrch[i].y)
            if (
                    (verts[polys[v][i]][1] - verts[polys[v][i + 1]][1]) * (x - verts[polys[v][i]][0]) +
                    (verts[polys[v][i + 1]][0] - verts[polys[v][i]][0]) * (y - verts[polys[v][i]][1])
                    > 0.0) //ak mensie ako nula tak neplati, staci ze pre jeden bod to plati
            {
                //std::cout<< "inside FALSE at " << v << std::endl;
                isInside = false;
                break;
            }
        }
        //now check the last one
        int k = polys[v].size() - 1;
        //std::cout<< "k " << k << std::endl;   //check
        //ifs still true
        if (isInside &&
            (verts[polys[v][k]][1] - verts[polys[v][0]][1]) * (x - verts[polys[v][k]][0]) +
            (verts[polys[v][0]][0] - verts[polys[v][k]][0]) * (y - verts[polys[v][k]][1])
            > 0.0) //ak mensie ako nula tak neplati, staci ze pre jeden bod to plati
        {
            //std::cout<< "inside FALSE at " << v << " special"<< std::endl;
            isInside = false;
        }
        if (isInside) {
            polyId = v;
            //std::cout << "inside TRUE at " << v << std::endl;
            //and inmmediatelly return true
            return true;
        }

    }
    return false;

}

void CoinMapGame::ClickMouse(float x, float y) {
    if (usePolyMap) {
        TestPoly();
        if (testMouse) {
            camera->MouseMovementNormalized(x, y, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT);
            std::cout << "mousePos norm " << x << ", " << y << std::endl;
        }
        return;
    }

    if (isMoving) {
        std::cout << "wait to finish movement..." << std::endl;
        return;
    }
    if (testMouse) {
        camera->MouseMovementNormalized(x, y, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT);
        std::cout << "mousePos norm " << x << ", " << y << std::endl;
    }

    //std::cout << "mouse pos" << x << " "<< y <<std::endl;
    camera->MouseMovementNormalized(x, y, windowSettings->CUR_WIDTH, windowSettings->CUR_HEIGHT);
    //teraz ked klikneme na poziciu musi napisat ci sa tam da dostat cez edges
    // ak ano zmen curLocation

    float smallest = 1000000;
    //float x=0,y=0;
    float sId = -1; //clickToNext
    //find the closest point
    for (int i = 0; i < vertex.size(); ++i) {
        float s = abs(x - vertex[i].x) + abs(y - vertex[i].y);
        if (s < smallest) {
            smallest = s;
            sId = i;
        }
    }

    if (curLocation == sId) {
        std::cout << "staying on same tile" << std::endl;
    } else {

        //now find route from curLocation to clickID
        bool allowed = false;
        if (std::find(edges.begin(), edges.end(),
                      std::pair<int, int>(curLocation, sId)) != edges.end()) {
            /* v contains x */
            allowed = true;
        } else {
            /* v does not contain x */
            //check other way around
            if (std::find(edges.begin(), edges.end(),
                          std::pair<int, int>(sId, curLocation)) != edges.end()) {

                allowed = true;
            }
        }

        if (allowed) {
            std::cout << "moving from " << curLocation << " to " << sId << std::endl;

            //playerChick_OI->SetPos(TranslateToWorld(vertex[sId])); //test
            glm::vec3 target = TranslateToWorld(vertex[sId]);
            target.z = playerChick_OI->GetPos().z;
            //create vector from vertex[curLocation] to vertex[curLocation]
            glm::vec3 curPos = playerChick_OI->GetPos();
            // normalize it
            //glm::vec3 dir = glm::normalize(target-curPos);
            glm::vec3 dir = target - curPos;
            //slowly move the player onto the place
            //kazdy frejm daj poziciu playerChick_OI->SetPos(curPos*0.01f++);
            isMoving = true;
            startPos = playerChick_OI->GetPos();
            endPos = target;
            direction = dir;
            // set curLocation to the new pos
            curLocation = sId;

        } else {
            std::cout << "Unable to move " << curLocation << " to " << sId << std::endl;
        }
    }


}

void CoinMapGame::GameTimeStep() {
    //TODO rewrite it so we can make a vactor that we can scale from 0-1
    if (camera->RMBpress)
        ClickMouse(camera->xMousePos, camera->yMousePos);
    //run this code each frame
    glm::vec3 newPos = playerChick_OI->GetPos();
    if (isMoving && !usePolyMap) {
        //newPos += (glm::normalize(direction) * 0.1f * moveSpeed * Managers::deltaTime);
        moveStep += moveStepDefault * Managers::deltaTime;
        //newPos += direction * 1.0f;
        //we have to add to the startPos
        //and also normalize based on size
        //now we have problem that we need to move at the same speed regardless of
        // the lenghth of the direction vector
        //we can solve it so that we use /glm::length(direction) but then moveStep being 1 is not
        // correct we need to enable it to go further
        newPos = startPos + (direction * moveStep * moveSpeed) / glm::length(direction);
        newPos.z = -10.0f;

        playerChick_OI->SetPos(newPos);

        //check if close enough or moveStep is more than 1
        if (abs(glm::length(endPos - newPos)) <= 0.05f || moveStep >= 1.0f * glm::length(direction)) {
            isMoving = false;
            std::cout << "stopped movement ,mmoveStep " << moveStep << std::endl;
            moveStep = 0;
        }
    }
}

void CoinMapGame::CreatePolyMap() {
    // begin creation of the poly map
    // we need to create convex polygons
    // so we create Vertices
    // define a poly by ginving it a list of Vertices
    // now allow WSAD movement of the player
    // but only if the next pos we want to move to is inside some polygon
}

void CoinMapGame::Movement() {
    if (usePolyMap) {
        int iPlayer = 0; //horizontal mov
        if (camera->leftArrowHold)
            iPlayer = -1;
        if (camera->rightArrowHold)
            iPlayer = 1;

        int vertical = 0; //vertical mov
        if (camera->upArrowHold)
            vertical = 1;
        if (camera->downArrowHold)
            vertical = -1;


        glm::vec3 movement = glm::vec3(0);
        movement.x = 1.0f * iPlayer;
        movement.y = 1.0f * vertical;
        //prevent normalizing  (very close to)zero vector...
        if (glm::length(movement) > 0.00000001f)
            movement = glm::normalize(movement);
        else {
            //theres no input
            return;
        }

        movement.z = 0.0f;
        movement *= Managers::deltaTime;
        float scale = camera->orthoScale;
        float aspect = (float) windowSettings->CUR_WIDTH / (float) windowSettings->CUR_HEIGHT;
        glm::vec3 movement2 = playerChick_OI->GetPos() + movement;
        movement2.x = ((playerChick_OI->GetPos().x + movement.x) + scale * aspect) / (aspect * scale * 2);
        movement2.y = ((playerChick_OI->GetPos().y + movement.y) + scale) / (2 * scale);
        //not working huh
        //std::cout << " mov " << glm::to_string(movement2) << std::endl;
        //expensive test, we should only check if we try to move

        //now we can get stuck,allow some offset/bias so it doesn"t happen, and check that
        float bias = 1.01f;
        if (iPlayer != 0 || vertical != 0) {
            if (!TestPoly(movement2 * bias)) {
                //do nothing
                return;
            } else {
                //move the player 
                playerChick_OI->SetPos(playerChick_OI->GetPos() + movement);
                //std::cout << " mov " <<glm::to_string(movement)  << std::endl;
            }
        }
    }
}

void CoinMapGame::EndGame() {

}

void CoinMapGame::ReloadGame() {

    //only set shooting as false and reset the velocity
    shoot = false;
    v = glm::vec3(0, 0, 0);

}

void CoinMapGame::ResetGame() {

    gameOver = false;
    score = 0;
    misses = 0;
    ReloadGame();
}

void CoinMapGame::DestroyGame() {

}

void CoinMapGame::RenderSceneInstance(Shader *s, bool renderSelected) {
    if (!gameOver) {
        Movement();
        GameTimeStep();
    }
    GameUI();

    SceneInstance::RenderSceneInstance(s, renderSelected);


}

CoinMapGame::~CoinMapGame() {

}
