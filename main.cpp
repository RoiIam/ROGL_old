#include <iostream>
#include <Log.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"


//void Help();

int main() {

    GLFWwindow* window;

    if (! glfwInit())
        return -1;

    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (! window)
    {
        glfwTerminate();

        return -1;
    }
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    std::cout << "Hello, World!" << std::endl;
    Help();
    Assimp::Importer importer;

    getchar();
    return 0;
}
