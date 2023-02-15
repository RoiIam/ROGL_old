#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#define GLEW_STATIC //couldnt build static mingw lib and link it properly so we use will glad
//#include <GL/glew.h>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "Source/shader.h"

#include "Instances/SceneInstance.h"
//#include "Instances/ObjectInstance.h"
#include <Log.h>
#include "test.h"


int main() {

    GLFWwindow* window;

    if (! glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (! window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return -1;
    }
    glfwMakeContextCurrent(window);

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
    while(!glfwWindowShouldClose(window))
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    delete light_shader;

    return 0;


}
