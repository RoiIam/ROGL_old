#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN, NONE
};
enum Camera_Perspective {
    ORTHO, PERSP
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    Camera_Perspective cameraPerspective = Camera_Perspective::PERSP;//default
    float orthoScale = 10.0f;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;


    bool hideCursor = true;
    bool lMove = false;
    bool rMove = false;
    bool fMove = false;
    bool bMove = false;
    bool uMove = false;
    bool dMove = false;

    bool slowCamControl = false;
    bool cameraControlsUnlocked = true;
    bool setCinematicCamera = false;

    int iPlayer = 0; //pouzite v gamescene
    bool leftArrow = false;
    bool rightArrow = false;
    bool leftArrowHold = false; //pouzite v CannonGame
    bool rightArrowHold = false; //pouzite v CannonGame
    bool upArrowHold = false; //pouzite v CannonGame
    bool downArrowHold = false; //pouzite v CannonGame
    bool shootSpace = false; //pouzite v CannonGame
    bool WKey = false; //pouzite v PlanetGame
    bool SKey = false; //pouzite v PlanetGame
    bool AKey = false; //pouzite v PlanetGame
    bool DKey = false; //pouzite v PlanetGame
    bool LMBpress = false; //pouzite v CoinMapGame
    bool RMBpress = false; //pouzite v CoinMapGamebool
    bool LMBhold = false;
    bool RMBhold = false;
    float xMousePos; //pouzite v CoinMapGame
    float yMousePos; //pouzite v CoinMapGame
    //vector pointing from the camera to the world, normalized
    glm::vec3 world_coordinates_ray_click;

    // constructor with vectors
    //tu nema byt takato deklaracia?
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = YAW, float pitch = PITCH);
    //prepis
    //Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    void SetPosDir(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    //lock or unlock camera movement
    void toggleCameraControls();

    void toggleCursor();

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    //constrainPitch was GLboolean from glad.h, idk why

    //returns 0-1 screen normalized x,y mouse coordinates, correctly scaled to screen/window size
    void MouseMovementNormalized(float &x, float &y,float w, float h);
    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();

    //given the front vector calc yaw and pitch
    void updateCameraVectors(glm::vec3 front);
};
