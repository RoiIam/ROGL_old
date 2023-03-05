#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#define GLEW_STATIC //couldnt build static mingw lib and link it properly so we use will glad
//#include <GL/glew.h>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include "imgui.h"
#include "Source/shader.h"

#include "Instances/SceneInstance.h"
//#include "Instances/ObjectInstance.h"
#include <Log.h>
#include "test.h"


float backgroundClearCol[4] = {0.7f, 0.7f, 0.7f, 0.7f};

float zNear = 0.5f;
float zFar = 2000.0f;
bool fullscreen = false;
bool enableShading = false;
bool enableCulling = true;
float cf[3];


int main() {

    //GLFWwindow* window;

    if (! glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    WindowSettings windowSettings = WindowSettings();
    //dont forget scene, window and monitor

    windowSettings.monitor = glfwGetPrimaryMonitor();
    windowSettings.window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (! windowSettings.window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return -1;
    }


    glfwMakeContextCurrent(windowSettings.window);

    //glad needs to be after make context current window
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout << "Hello, World!" << std::endl;
    ::Help();
    Assimp::Importer importer;

    glViewport(0, 0, 800, 600);
    Shader* light_shader;
    //Model* lightCube;
    light_shader = new Shader(R"(C:\Users\robko\Documents\CLion\OGLR\Assets\Shaders\MultipleLights\s_light.vert)",
                              R"(C:\Users\robko\Documents\CLion\OGLR\Assets\Shaders\MultipleLights\s_light.frag)");
    /*
     //test pridania sceneInstance
    auto* s = new SceneInstance();
    s->DrawSky();
    */
    SceneInstance* sc = new SceneInstance();
    sc->windowSettings = & windowSettings; //toto nebude upne dobre
    auto camera = new Camera(glm::vec3(0.0f, .0f, 3.0f));

    sc->Setup(camera);
    while(!glfwWindowShouldClose(windowSettings.window))
    {
        /*glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glfwSwapBuffers(window);
        glfwPollEvents();*/

        PerfAnalyzer::drawcallCount = 0; //reset for new frame
        float currentFrame = glfwGetTime(); // delta time used for camera now
        //deltaTime = currentFrame - lastFrame;
        //lastFrame = currentFrame;

        uniforms.view = camera->GetViewMatrix(); //uniformy su ine tu ako v drawskybox //see translation unit, .cpp is one TU...
        uniforms.projection = glm::perspective(glm::radians(camera->Zoom),(float)windowSettings.CUR_WIDTH / (float)windowSettings.CUR_HEIGHT, zNear, zFar+100);
        //std::cout<< glm::to_string(uniforms.view)<< "\n";

        glClearColor(backgroundClearCol[0], backgroundClearCol[1],
                     backgroundClearCol[2], backgroundClearCol[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (enableCulling)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        if (!camera->showCursor) {
            glfwSetInputMode(windowSettings.window, GLFW_CURSOR,
                             GLFW_CURSOR_DISABLED);
            //ImGui::SetWindowFocus(nullptr);
        }
        else {
            glfwSetInputMode(windowSettings.window, GLFW_CURSOR,
                             GLFW_CURSOR_NORMAL);
        }

        sc->RenderSceneInstance(nullptr);
        glfwPollEvents();
        glfwSwapBuffers(windowSettings.window);
        //TODO continue with adding functionality below

        //yeah testscene is special case
        //if(testScene == nullptr)
        //    scene->RenderScene(nullptr);


        //if(scene != nullptr && scene->dirLight_ObjInstance!= nullptr) {
           /* if (enableShading) {
                glm::mat4 LSM = RenderDepth();

                RenderTest(LSM);

            } else {
                if(testScene != nullptr)
                    testScene->SetupShaderMaterial();
                scene->RenderScene(nullptr);
            }
            //also
            scene->RenderLights();
            // draw debug line pointing from light pos to dir
            if(enableDebugLightRay) {
                colShader.use();
                modelMat = glm::mat4(1.0f);
                modelMat = glm::translate(modelMat, centroidPos);
                colShader.setMat4("model", modelMat);
                colShader.setMat4("view", uniforms.view);
                colShader.setMat4("projection", uniforms.projection);
                glGenVertexArrays(1, &lineVAO);
                glGenBuffers(1, &lineVBO);
                glBindVertexArray(lineVAO);
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6, &centroidLineSeg, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
                glLineWidth(6.3f);
                glDrawArrays(GL_LINE_STRIP, 0, 3);
                PerfAnalyzer::drawcallCount++;
                glBindVertexArray(0);
            }
            */
            // drawline pointing from screen click
            /*
             *
             * this block of code is leaking memory constantly need to fix
                t->mesh_shader->use();
                //or
                colShader.use();
                modelMat = glm::mat4(1.0f);
                modelMat = glm::translate(modelMat, cameraDrawPos);
                colShader.setMat4("model", modelMat);
                colShader.setMat4("view", camera.GetViewMatrix());
                colShader.setMat4("projection", t->projection);
                // glEnable(GL_LINE_SMOOTH);
                // glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
                glGenVertexArrays(1, &lineVAO);
                glGenBuffers(1, &lineVBO);
                glBindVertexArray(lineVAO);
                glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6, &lineSeg,
                             GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                                      (void*)0);
                /// glBindVertexArray(lineVAO);
                glLineWidth(6.3f);
                glDrawArrays(GL_LINE_STRIP, 0, 3);
                PerfAnalyzer::drawcallCount++;  // after every draw call
                // glDrawElements(GL_LINE_STRIP,2, GL_FLOAT, nullptr);
            */



        //}

        //TODO add more working parameters for objects
        //TODO test loading of object
        //TODO maybe allow changing shader params
        //TODO make sun color functional again


        //TODO separate,cleanup,refactor the code for shadowmaps...
        /*
        // draw GUI last
        io = ImGui::GetIO();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        glfwPollEvents();
        ImGui::NewFrame();
        DrawImGui();

        if(scene!= nullptr)
            scene->ImGuiHierarchy();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        */
        //camera->ProcessKeyboard(Camera_Movement::NONE, deltaTime);


    }
    glfwTerminate();
    delete light_shader;

    return 0;


}
