#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#define GLEW_STATIC //couldnt buld static mingqw lib and link it properly so wr use glad
//#include <GL/glew.h>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"

#include <Log.h>
#include <test.h>


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

    //glad needs to be after make context current window
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return-111;//THROW_ERROR("Failed to load functions!");
    std::cout << "Hello, World!" << std::endl;
    ::Help();
    Assimp::Importer importer;

    getchar();
    return 0;
}
