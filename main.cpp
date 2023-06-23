//it is in cmake compile definitions already and this doesnt even work, wait for update
#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
//#define GLEW_STATIC //couldnt build static mingw lib and link it properly so we use will glad
//#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL //to enable vec3* int,must be before the includes

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp> //to use make_vec3
#include <glm/ext.hpp>
#include "Source/shader.h"
#include "Instances/SceneInstance.h"

//#include "Instances/ObjectInstance.h"
#include <Log.h>
#include "test.h"


#include "imgui.h"
//#include <imgui_impl_glfw.h> //179
//#include <imgui_impl_opengl3.h> //179
#include <backends/imgui_impl_glfw.h> //187
#include <backends/imgui_impl_opengl3.h> //187
//#define IMGUI_ENABLE_WIN32_DEFAULT_IME_FUNCTIONS
#include <imgui_internal.h> //for more functions

#include "Scenes/ForwardScene1.h"
#include "Scenes/DeferredScene1.h"
#include "Scenes/DeferredScene2.h"
#include "Primitives/Path.h"

//for glints_ch
#define TINYEXR_USE_MINIZ 0

#include "zlib.h"
#include "tinyexr.h"

//compiler flags, idk what those do
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-void-pointer-cast"
#pragma clang diagnostic pop


void ReloadScene(int num);

void PrintShader();


#pragma region common_settings //common resources for scenes

#include "Utilities/GraphicsOptions.h"

GraphicsOptions *graphicsOptions;


#pragma endregion


#pragma region settings_params
//TODO reorganize

float backgroundClearCol[4] = {0.7f, 0.7f, 0.7f, 0.7f};

float zNear = 0.5f;
float zFar = 50.0f;
bool fullscreen = false;

bool enableCulling = false;
float cf[3];
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float cameraSpeed = 250.0f;
glm::vec3 freeCamLookDir = glm::vec3(0);
unsigned int nextCamPosIndex = 0;
float freeCamT = 0;

Path freeCamPath = Path();

glm::vec3 centroidPos;

Camera *camera;
WindowSettings windowSettings = WindowSettings();
float sunOffsetPos = 23.0f;
glm::vec3 sunDir = {0.2f, 0.500f, -0.1};  // glm::vec3(1.0f);
float lightOrthoSize = 20.0f;
//Depth related stufff
//-----------------------
const unsigned int SHADOW_WIDTH = 8192 / 4, SHADOW_HEIGHT = 8192 / 4;
unsigned int depthMapFBO;
// create depth texture
unsigned int depthMap;
Shader simpleDepthShader;
Shader debugDepthQuad;
Shader simpleShadowShader;
bool enableDebugDepthQuad = false;
bool enableDebugLightRay = true;

//scene stuff - needs to be declared earlier
std::string sceneDescription = "Put short description of scene here";
std::shared_ptr<ForwardScene1> forwardScene1;//this used to be TestScene
std::shared_ptr<DeferredScene2> deferredScene2 = NULL;
std::shared_ptr<SceneInstance> sceneInstance;

#pragma endregion


Model *centroidModel;

#pragma region IMGUI_stuff
//this should be in separate class...

bool show_demo_window = false;  // use F9


glm::vec3 dirlightCol = {0.6f, 0.2f, 0.3f};  // glm::vec3(1.0f);
float dirlightColChange[3];


typedef void (*ImGuiDemoMarkerCallback2)(const char *file, int line, const char *section, void *user_data);

extern ImGuiDemoMarkerCallback2 GImGuiDemoMarkerCallback2;
extern void *GImGuiDemoMarkerCallbackUserData2;
ImGuiDemoMarkerCallback2 GImGuiDemoMarkerCallback2 = NULL;
void *GImGuiDemoMarkerCallbackUserData2 = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback2 != NULL) GImGuiDemoMarkerCallback2(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData2); } while (0)
GLfloat lineSeg[] = {0, 0, 0,  // first vertex
                     0, 1.0f, 0};//last point
GLfloat centroidLineSeg[] = {0, 0, 0,  // first vertex
                             0, 1.0f, 0};

//TODO why si this here?
float mousePos[2] = {0, 0};
glm::vec3 cameraDrawPos;
//bool isOutline = false; not eneded
Shader colShader;
Shader stencilShader;
bool enable_culling = true;
bool changeCol = false;

float pos[3];
float rot[3];

void ImGuiObjProperties(ObjectInstance *obj) {
    ImGuiIO &io = ImGui::GetIO();

    ImGui::Text("Edit Selected Object:");
    //ImGui::GetIO().WantCaptureKeyboard = true;
    //ImGui::GetIO().WantTextInput =true;
    // position
    pos[0] = obj->GetPos().x;
    pos[1] = obj->GetPos().y;
    pos[2] = obj->GetPos().z;
    //old ImGui::InputFloat3("Obj Position", pos, "%.3f");
    ImGui::DragFloat3("Obj Position", pos, 0.05f, -10000, 10000, "%.3f");
    ImGui::SetItemUsingMouseWheel();

    //rot
    rot[0] = obj->GetDeg("X");
    rot[1] = obj->GetDeg("Y");
    rot[2] = obj->GetDeg("Z");
    ImGui::DragFloat3("Obj Rotation", rot, 0.05f, -10000, 10000, "%.3f");
    ImGui::SetItemUsingMouseWheel();

    // scale
    float scale[] = {obj->GetScale().x, obj->GetScale().y, obj->GetScale().z};
    ImGui::DragFloat3("Obj Scale", scale, 0.05f, -10000, 10000, "%.3f");
    //ImVec4

    //rotation
    //scale
    //cout << "Pos " << pos[0]<< " "<< pos[1] << " " <<pos[2]   << "Saved "<< glm::to_string(obj->position) << "\n";
    //show changes
    obj->SetPos(glm::vec3(pos[0], pos[1], pos[2]));
    obj->SetScale(glm::make_vec3(scale));
    obj->SetDeg(rot[0], "X");
    obj->SetDeg(rot[1], "Y");
    obj->SetDeg(rot[2], "Z");

    //show assigned shader
    //ImGui::Text("Shader assigned:"+ obj->GetShader());

    //obj.position =  glm::vec3(10,10,10); not working either
    //obj->position =  glm::vec3(pos[0],pos[1],pos[2]);
}

void ImGuiLightProperties(ObjectInstance *objL) {
    /*
    glm::vec3 direction;
    // position this is copied...
    pos[0] = objL->GetPos().x;
    pos[1]= objL->GetPos().y;
    pos[2]  = objL->GetPos().z;
    ImGui::InputFloat3("ObjL Position", pos, "%.3f");
    //ImVec4

    //rotation
    //scale
    //cout << "Pos " << pos[0]<< " "<< pos[1] << " " <<pos[2]   << "Saved "<< glm::to_string(obj->position) << "\n";
    //show changes
    objL->SetPos(glm::vec3(pos[0],pos[1],pos[2]));
     */
    ImGuiObjProperties(objL); //this is much better than ^
    //now the dir

    //now try to assign
    try {
        // maybe its going to be better using enums
        auto light = dynamic_cast<DirectionalLight *>(objL->light);
        float dir[] = {light->direction.x, light->direction.y, light->direction.z};
        ImGui::DragFloat3("Light Dir", dir, 0.015f, -1, 1, "%.3f");
        dynamic_cast<DirectionalLight *>(objL->light)->direction = glm::vec3(dir[0], dir[1], dir[2]);

        static ImVec4 color = ImVec4(dirlightCol.x, dirlightCol.y, dirlightCol.z, 1);
        ImGui::ColorEdit3("Light Color", (float *) &color);
        if (ImGui::IsItemActive())  // continous edit or IsItemDeactivatedAfterEdit-// only after i lift mouse
        {
            dynamic_cast<Light *>(objL->light)->color = glm::vec3(color.x, color.y, color.z);
        }


    } catch (std::bad_cast &bc) {
        std::cerr << bc.what() << std::endl;
    }


}

void DrawImGui() {

    ImGuiIO &io = ImGui::GetIO();
    // feed inputs to dear imgui, start new frame, now moved to scene...
    static bool test2 = false;
    ImGui::Begin("Tool window", &test2, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar);

    if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

    // Menu Bar
    if (ImGui::BeginMenuBar()) {

        //create menu bar
        //IMGUI_DEMO_MARKER("Menu/Examples");
        if (ImGui::BeginMenu("Scenes")) {
            if (ImGui::MenuItem("Load scene 1", NULL))
                ReloadScene(1);
            if (ImGui::MenuItem("Load scene 2", NULL))
                ReloadScene(2);
            if (ImGui::MenuItem("Load scene 3", NULL))
                ReloadScene(3);
            //ImGui::MenuItem("Main menu bar", ReloadScene(2));
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Others")) {
            if (ImGui::MenuItem("Unselect", NULL))
                sceneInstance->selectedInstance = nullptr;
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::TextWrapped("%s", sceneDescription.c_str());

    if (ImGui::CollapsingHeader("Debugging Helpers")) {
        cf[0] = camera->Front.x;
        cf[1] = camera->Front.y;
        cf[2] = camera->Front.z;
        // get cam location
        ImGui::InputFloat3("Cam Front dir", cf, "%.3f");
        // get cam pos
        cf[0] = camera->Position.x;
        cf[1] = camera->Position.y;
        cf[2] = camera->Position.z;
        ImGui::InputFloat3("Cam Pos", cf, "%.3f");
        // get cam up
        cf[0] = camera->WorldUp.x;
        cf[1] = camera->WorldUp.y;
        cf[2] = camera->WorldUp.z;
        ImGui::InputFloat3("Cam World Up", cf, "%.3f");

        // get cam euler rot
        float yaw = camera->Yaw;
        float pitch = camera->Pitch;
        float eulerRot[3] = {yaw, pitch, 0};

        ImGui::InputFloat2("Cam Rotation", eulerRot, "%.3f");
        ImGui::Value("timed value freeCamT ", freeCamT);

    }

    ImGui::Checkbox("Enable face culling", &enable_culling);


    if (ImGui::Checkbox("Switch shadows on (default off)", &graphicsOptions->enableShadows)) {
        if (!(deferredScene2 = dynamic_pointer_cast<DeferredScene2>(sceneInstance)) && graphicsOptions->enableShadows)
            return;
        deferredScene2->enableSSAO = false;
        deferredScene2->graphicsOptions->rendererType = GraphicsOptions::RendererType::forward;

    }

    if (ImGui::Checkbox("Switch shadows and water on (default off)", &graphicsOptions->enableWater)) {
        if (!(deferredScene2 = dynamic_pointer_cast<DeferredScene2>(sceneInstance)))
            return;
        if (!graphicsOptions->enableWater) {
            deferredScene2->waterObjInstance->disableRender = true;
            deferredScene2->waterObjInstance->forceRenderOwnShader = false;
        } else if (graphicsOptions->enableWater) {
            deferredScene2->enableSSAO = false;
            deferredScene2->graphicsOptions->rendererType = GraphicsOptions::RendererType::forward;

        }
    }


    if (graphicsOptions->enableShadows)
        ImGui::Checkbox("Show Quad debug", &enableDebugDepthQuad);


    ImGui::Checkbox("Show Light Ray Debug", &enableDebugLightRay);
    // for debug DepthDebug on ImGUi ignore red tint, we wpuld eeed to alter textureformat
    // see https://github.com/inkyblackness/imgui-go/issues/42 to
    if (enableDebugDepthQuad) {
        ImGui::Image((void *) depthMap, ImVec2(200, 200), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1, 1, 1, 1));
    }

    // ImGui::SliderFloat4("Background Color",backgroundClearCol, 0 ,1 ,"%.3f");

    ImGui::Value("Drawcalls", PerfAnalyzer::drawcallCount);

    // now try gizmos
    if (sceneInstance->selectedInstance) {
        glm::vec3 a = sceneInstance->selectedInstance->GetPos();
        glm::vec3 b = a;
        b.x += 1;
        b.y += 1;
        b.z += 1;
    }
    ImGui::InputFloat("Sun Offset From Target", &sunOffsetPos, 0.1f, 0.5f, "%.3f");
    float sunDirUI[] = {sunDir.x, sunDir.y, sunDir.z};
    ImGui::DragFloat3("Sun Direction", sunDirUI, 0.001f, -1, 1, "%.3f");
    sunDir = glm::make_vec3(sunDirUI);
    //DirectionalLight(sceneInstance->dirLight_ObjInstance->light).direction = sunDir;
    if (sceneInstance->dirLight_ObjInstance) {
        auto dl = dynamic_cast<DirectionalLight *>(sceneInstance->dirLight_ObjInstance->light);
        if (dl)
            dl->direction = sunDir;
    }

    // far plane near plane
    float zFarUI = zFar;
    float zNearUI = zNear;
    ImGui::InputFloat("Near plane cam", &zNearUI, 0.1f, 0.5f, "%.3f");
    ImGui::InputFloat("Far plane cam", &zFarUI, 0.1f, 0.5f, "%.3f");
    zNear = zNearUI;
    zFar = zFarUI;

    float orthoSize = lightOrthoSize;
    ImGui::DragFloat("lighmap ortho size", &orthoSize, 0.001f, 2.0f, 100.0f);
    lightOrthoSize = orthoSize;


    if (sceneInstance != nullptr && sceneInstance->selectedInstance != nullptr) {
        if (sceneInstance->selectedInstance->light != nullptr)
            ImGuiLightProperties(sceneInstance->selectedInstance);
        else
            ImGuiObjProperties(sceneInstance->selectedInstance);
    }

    if (ImGui::Checkbox("Set cinematic camera", &camera->blockControls)) {
        //std::cout << "item cinamtic cam changed..."<<std::endl;
        if (camera->blockControls) {
            nextCamPosIndex = 0;

        } else {

        }
    }

    if (camera->blockControls) {
        float freeCamLookDirUI[] = {freeCamLookDir.x, freeCamLookDir.y, freeCamLookDir.z};
        ImGui::InputFloat("free cam speed", &cameraSpeed, 0.1f, 0.5f, "%.2f");
        ImGui::DragFloat3("Free cam look At", freeCamLookDirUI, 0.1f, -100, 100, "%.3f");
        freeCamLookDir = glm::make_vec3(freeCamLookDirUI);
    }

    ImGui::Value("CUR_WIDTH", windowSettings.CUR_WIDTH);
    ImGui::Value("CUR_HEIGHT", windowSettings.CUR_HEIGHT);
    ImGui::Value("width mon", glfwGetVideoMode(windowSettings.monitor)->width);
    ImGui::Value("height mon", glfwGetVideoMode(windowSettings.monitor)->height);

    ImGui::End();
}


#pragma endregion

#pragma region SceneStuff

//test planes for OBB colisions
bool TestRayOBBIntersection(glm::vec3 ray_origin,     // Ray origin, in world space
                            glm::vec3 ray_direction,  // Ray direction (NOT target position!), in world
        // space. Must be normalize()'d.
                            glm::vec3 aabb_min,       // Minimum X,Y,Z coords of the mesh when not
        // transformed at all.
                            glm::vec3 aabb_max,  // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh
        // is centered, but it's not always the case.
                            glm::mat4 ModelMatrix,  // Transformation applied to the mesh (which will
        // thus be also applied to its bounding box)
                            float &intersection_distance  // Output : distance between ray_origin and
        // the intersection with the OBB
) {
    // Intersection method from Real-Time Rendering and Essential Mathematics for Games

    float tMin = 0.0f;
    float tMax = 100000.0f;

    glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

    glm::vec3 delta = OBBposition_worldspace - ray_origin;

    // Test intersection with the 2 planes perpendicular to the OBB's X axis
    {
        glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
        float e = glm::dot(xaxis, delta);
        float f = glm::dot(ray_direction, xaxis);

        if (fabs(f) > 0.001f) {  // Standard case

            float t1 = (e + aabb_min.x) / f;  // Intersection with the "left" plane
            float t2 = (e + aabb_max.x) / f;  // Intersection with the "right" plane
            // t1 and t2 now contain distances betwen ray origin and ray-plane
            // intersections

            // We want t1 to represent the nearest intersection,
            // so if it's not the case, invert t1 and t2
            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;  // swap t1 and t2
            }

            // tMax is the nearest "far" intersection (amongst the X,Y and Z planes
            // pairs)
            if (t2 < tMax) tMax = t2;
            // tMin is the farthest "near" intersection (amongst the X,Y and Z planes
            // pairs)
            if (t1 > tMin) tMin = t1;

            // And here's the trick :
            // If "far" is closer than "near", then there is NO intersection.
            // See the images in the tutorials for the visual explanation.
            if (tMax < tMin) return false;

        } else {  // Rare case : the ray is almost parallel to the planes, so they
            // don't have any "intersection"
            if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f) return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Y axis
    // Exactly the same thing than above.
    {
        glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
        float e = glm::dot(yaxis, delta);
        float f = glm::dot(ray_direction, yaxis);

        if (fabs(f) > 0.001f) {
            float t1 = (e + aabb_min.y) / f;
            float t2 = (e + aabb_max.y) / f;

            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;
            }

            if (t2 < tMax) tMax = t2;
            if (t1 > tMin) tMin = t1;
            if (tMin > tMax) return false;

        } else {
            if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f) return false;
        }
    }

    // Test intersection with the 2 planes perpendicular to the OBB's Z axis
    // Exactly the same thing than above.
    {
        glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
        float e = glm::dot(zaxis, delta);
        float f = glm::dot(ray_direction, zaxis);

        if (fabs(f) > 0.001f) {
            float t1 = (e + aabb_min.z) / f;
            float t2 = (e + aabb_max.z) / f;

            if (t1 > t2) {
                float w = t1;
                t1 = t2;
                t2 = w;
            }

            if (t2 < tMax) tMax = t2;
            if (t1 > tMin) tMin = t1;
            if (tMin > tMax) return false;

        } else {
            if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f) return false;
        }
    }

    intersection_distance = tMin;
    return true;
}


//source http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
void TestMouse2(glm::vec3 dir) {
    float tempDist = 100000;
    sceneInstance->selectedInstance = nullptr;
    for (int i = 0; i < sceneInstance->selectableObjInstances.size(); i++) {
        float intersection_distance;  // Output of TestRayOBBIntersection()
        glm::vec3 aabb_min(-1.0f, -1.0f, -1.0f);
        glm::vec3 aabb_max(1.0f, 1.0f, 1.0f);
        //cout << scene->selectableObjInstances[i]->Name + "\n";
        aabb_min = sceneInstance->selectableObjInstances[i]->GetModel()->boundMin; // if there is no model, returns null which makes error
        aabb_max = sceneInstance->selectableObjInstances[i]->GetModel()->boundMax;
        // The ModelMatrix transforms :
        // - the mesh to its desired position and orientation
        // - but also the AABB (defined with aabb_min and aabb_max) into an OBB
        // glm::mat4 RotationMatrix = glm::toMat4(orientations[i]);
        // glm::mat4 TranslationMatrix = translate(mat4(), positions[i]);
        glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(40.0f), glm::vec3(0.0f,
        // 0.0f, 1.0f)); model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	//
        // it's a bit too big for our scene, so scale it down model =
        // glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

        // model = glm::rotate(model, glm::radians(scene->selectableObjInstances[i]->GetDeg()),
        //                     scene->selectableObjInstances[i]->GetRot());  /// bounds not set....
        // model = glm::scale(model, models[i]->scale);
        //model = glm::translate(model, scene->selectableObjInstances[i]->GetPos());
        model = sceneInstance->selectableObjInstances[i]->GetTransformMat();
        // std::cout << "dev rad " << models[i]->rad << " pos " <<glm::to_string(
        // models[i]->position ) <<" " << " rot "<<
        // glm::to_string(models[i]->rotation)<<"sc "
        //  <<glm::to_string(models[i]->scale) <<  std::endl;
        if (sceneInstance->selectableObjInstances[i]->Name == "shrek") {

        }

        if (TestRayOBBIntersection(camera->Position, dir, aabb_min, aabb_max, model, intersection_distance)) {
            std::cout << "picked object " << sceneInstance->selectableObjInstances[i]->GetModel()->directory
                      << sceneInstance->selectableObjInstances[i]->Name << " id " << intersection_distance << "\n"
                      //<< " mat" << std::endl
                      //<< "min "
                      //<< glm::to_string(scene->objectInstances[i]->GetModel()->boundMin)
                      //<< glm::to_string(scene->objectInstances[i]->GetModel()->boundMax)
                      //<< std::endl
                      //<< glm::to_string(scene->objectInstances[i]->GetPos()) << " "
                      << std::endl;
            // break; // instead get the closest one
            if (intersection_distance < tempDist) {
                // Now we need to enable outline rendering and then add position change
                // gimbals
                tempDist = intersection_distance;
                sceneInstance->selectedInstance = sceneInstance->selectableObjInstances[i];
            }
        }
    }
    //if we get empty selection unselect,
    //if there was selection push back.
    //no longer removing so stop if(scene->selectedInstance!= nullptr)
    //scene->selectableObjInstances.push_back(scene->selectedInstance);
    if (sceneInstance->selectedInstance == nullptr) {
        //why do this tho   isOutline = false;
        //if(scene->selectedInstance!= nullptr)
        //scene->objectInstances.push_back(scene->selectedInstance);

        sceneInstance->selectedInstance = nullptr;
    } else {
        //if(scene->selectedInstance != nullptr && scene->selectedInstance != scene->selectedObjInstance)// push back previously picked object
        //scene->objectInstances.push_back(scene->selectedInstance);

        sceneInstance->selectedInstance = sceneInstance->selectedInstance;
        /*stop doing this in favor of check if they are the same
        //scene->selectableObjInstances.erase(std::remove(scene->selectableObjInstances.begin(), scene->selectableObjInstances.end(), scene->selectedInstance)); //toto  zatial ide
        //2 test
        //std::remove(scene->objectInstances.begin(), scene->objectInstances.end(), scene->selectedInstance); //didnt work
        */
        //not needed isOutline = true;
    }

    // std::cout << "pick col " << bArray <<" mats"<< std::endl;
    // std::cout << "pick col " << int(bArray[0] + bArray[1] * 256 +  bArray[2] *
    // 256*256)<<" "<< std::endl;
}

void Drawline() {
    if (sceneInstance == nullptr)
        return;
    float x = (2.0f * mousePos[0]) / windowSettings.CUR_WIDTH - 1.0f;
    float y = 1 - (2.0f * mousePos[1]) / windowSettings.CUR_HEIGHT;
    float z = 1.0f;

    glm::vec3 ndcr = glm::vec3(x, y, z);

    // Homogeneous Clip Coordinates
    glm::vec4 homogeneous_clip_coordinates_ray = glm::vec4(ndcr.x, ndcr.y, -1.0f, 1.0f);

    // 4D Eye (Camera) Coordinates
    cameraDrawPos = camera->Position;
    glm::vec4 camera_ray = glm::mat4(glm::inverse(sceneInstance->projection)) * homogeneous_clip_coordinates_ray;
    camera_ray = glm::vec4(camera_ray.x, camera_ray.y, -1.0f, 0.0f);

    // 4D World Coordinatesf
    glm::vec3 world_coordinates_ray = glm::inverse(sceneInstance->view) * camera_ray;
    // world_coordinates_ray = world_coordinates_ray.Normalize();

    // because we are in local coords, shader sets pos to camera and we start at 0
    // here
    lineSeg[0] = 0;
    lineSeg[1] = 0;
    lineSeg[2] = 0;

    glm::vec3 naa = world_coordinates_ray;
    glm::vec3 g = glm::normalize(world_coordinates_ray) * 10;

    lineSeg[3] = g.x;
    lineSeg[4] = g.y;
    lineSeg[5] = g.z;
    // std::cout << "CamPos " << ass[0] << " " << ass[1]  << " " << ass[2]<<"dir "
    // << a.x*2 << " " << a.y*2 << " " <<a.z*2 <<std::endl; std::cout << "pos " <<
    // glm::to_string(world_coordinates_ray) << std::endl;

    // TestMouse(); //different method
    TestMouse2(glm::normalize(world_coordinates_ray));
}


//load scene based on number
void ReloadScene(int num) {
    //if(num != scene->num)
    if (true) {
        std::cout << "Reloading Scene\n";
        if (sceneInstance != nullptr) {
            sceneInstance->selectedInstance = nullptr;
            sceneInstance->DeleteSceneBuffers();
        }
        sceneInstance.reset();
        //testScene.reset();
        //scene->~Scene();
        //delete scene;

        //scene = nullptr;
        forwardScene1 = nullptr;// if we dont do this, it will detect this as not null then it will not render anythong bcs dirlight at sceneinstance is null
        switch (num) {
            case 1:
                forwardScene1 = static_cast <const std::shared_ptr<ForwardScene1> >(new ForwardScene1());
                sceneInstance = forwardScene1;
                static_cast<const std::shared_ptr<SceneInstance> >(new ForwardScene1());

                break;
            case 2:
                sceneInstance = static_cast<const std::shared_ptr<SceneInstance> >(new DeferredScene1());
                break;
            case 3:
                sceneInstance = static_cast<const std::shared_ptr<SceneInstance> >(new DeferredScene2());
                break;
            case 4:
                //scene = static_cast<const std::shared_ptrshared_ptr<SceneInstance> >(new TestScene4());
                break;
            default:
                break;


        }


        //scene = static_cast<const std::shared_ptrshared_ptr<Scene> >(new TestScene2());
        sceneInstance->windowSettings = &windowSettings;//needs to go before setup otherwise buffers are initialized before setting
        sceneInstance->Setup(camera, graphicsOptions);
        sceneDescription = sceneInstance->sceneDescription;
    }
}

#pragma endregion

#pragma region GLFWcallbacks

//glfw make big
void SwitchFullscreen() {
    fullscreen = !fullscreen;
    if (fullscreen) {
        // save previous values
        SCR_WIDTH = windowSettings.CUR_WIDTH;
        SCR_HEIGHT = windowSettings.CUR_HEIGHT;
        // set monitor sizes
        windowSettings.CUR_WIDTH = glfwGetVideoMode(windowSettings.monitor)->width;
        windowSettings.CUR_HEIGHT = glfwGetVideoMode(windowSettings.monitor)->height;

        glfwSetWindowMonitor(windowSettings.window, windowSettings.monitor, GLFW_DONT_CARE, GLFW_DONT_CARE,
                             windowSettings.CUR_WIDTH, windowSettings.CUR_HEIGHT, GLFW_DONT_CARE);
    } else {
        windowSettings.CUR_HEIGHT = SCR_WIDTH;
        windowSettings.CUR_HEIGHT = SCR_HEIGHT;
        glfwSetWindowMonitor(windowSettings.window, nullptr, 50, 50, SCR_WIDTH, SCR_HEIGHT, GLFW_DONT_CARE);
    }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        Drawline();  // uhh we neeed to keep drawing it...
        // Draw line of sight latest
        std::cout << "pressing RMB \n";
    }
}

static void keyboard_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_F9) {
            if (key == GLFW_KEY_F9) {
                show_demo_window = !show_demo_window;
                if (camera->showCursor)
                    camera->toggleCursor();
            }

            camera->toggleCursor();  // ked som v esc a stuknem f9 bic sa nestane
            // ostatne funguje fajn,idk why

            if (camera->showCursor) {  // we want to focus IMGUI
                ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
                camera->rMove = camera->lMove = camera->fMove = camera->bMove = false;
                ImGui::SetNextWindowFocus();  // set focus on first one? i guess

            } else  // we want to enable game
            {
                show_demo_window = false;
                ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
                ImGui::SetWindowFocus(NULL);  // defocus
                // ImGui::GetIO().WantCaptureMouse = false; // this should be used to
                // discard input in main app, ie in camera process input
            }
        }
        if (key == GLFW_KEY_F12) SwitchFullscreen();
        if (key == GLFW_KEY_F2) // print shader info
            PrintShader();
        if (key == GLFW_KEY_LEFT_CONTROL)
            camera->slowCamControl = !camera->slowCamControl;
    }

    if (camera->showCursor)  // later implement input class so we dont have to
        // check it like that
    {
        return;
    }
    bool set = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_W) camera->fMove = set;
    if (key == GLFW_KEY_S) camera->bMove = set;
    if (key == GLFW_KEY_A) camera->lMove = set;
    if (key == GLFW_KEY_D) camera->rMove = set;
    if (key == GLFW_KEY_E) camera->uMove = set;
    if (key == GLFW_KEY_Q) camera->dMove = set;

    /* if (key == GLFW_KEY_F) //for light
       camera->switchSpotlight();*/
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width
    // and height will be significantly larger than specified on retina displays.
    if (height == 0 || width == 0)  // prevent errors from minimizing and moving windows
        return;
    glViewport(0, 0, width, height);
    // std::cout << width << " " << height << "\n";
    windowSettings.CUR_WIDTH = width;
    windowSettings.CUR_HEIGHT = height;

    sceneInstance->ResizeScene();
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    mousePos[0] = xpos;
    mousePos[1] = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

static void character_callback(GLFWwindow *window, unsigned int codepoint) {
    if (ImGui::GetIO().WantCaptureKeyboard)
        ImGui::GetIO().AddInputCharacter(codepoint);

}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera->ProcessMouseScroll(yoffset);
}

void window_size_callback(GLFWwindow *window, int width, int height) {
    // do nothing, unused
}

#pragma endregion

#pragma region Depth

//renders depthmap for directional light
glm::mat4 RenderDepth() {
    //first pass, render scene from lights point of view in ortho perspective
    glm::mat4 modelMat = glm::mat4(1.0f);
    float near_plane = 1.0f, far_plane = sunOffsetPos;

//glm::vec3 lightPos =  dynamic_cast<DirectionalLight *>(scene->dirLight)->direction;
    glm::vec3 lightPos = sceneInstance->dirLight_ObjInstance->GetPos();

    glm::vec3 g = glm::normalize(sunDir) * 10;
//assign debug line points
    centroidLineSeg[3] = g.x;
    centroidLineSeg[4] = g.y;
    centroidLineSeg[5] = g.z;

//when we want to cover bigger area we need to either move the light projection based on cam pos,
// increase ortho size to cover bigger area(but dont make it too big otherwise we need bigger shadowmap resolution to compensate for smaller texel)
// i.e bigger ortho means more screen pixels draw from one pixel of depthmap creating aliasing issues
    float res = lightOrthoSize;
    glm::mat4 lightProjection = glm::ortho(-res, res, -res, res, near_plane, zFar);

    //spravme to jednoducho lightprojection bude 1-200
    //lightProjection
    //lightView bude len na kameru a lightPos je niekde za chrbtom, farplane*direction
    lightPos = camera->Position + glm::normalize(sunDir) * sunOffsetPos;
    glm::mat4 lightView = glm::lookAt(lightPos, camera->Position, glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    sceneInstance->dirLight_ObjInstance->SetPos(lightPos);
    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    //simpleDepthShader.setMat4("model", modelMat);


    //if (forwardScene1 != nullptr)
    //    forwardScene1->SetupShaderMaterial();

//now render all receiving objects into the shadowmap
    sceneInstance->RenderObjectsS(&simpleDepthShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return lightSpaceMatrix;
}


void RenderTest(glm::mat4 lightSpaceMatrix, bool renderSelected = false) {
//if(scene->disableShadows)
    //return;
    //second pass, render scene as normal with shadow mapping (using depth map)
    glViewport(0, 0, windowSettings.CUR_WIDTH, windowSettings.CUR_HEIGHT);
    // now try shading it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glm::vec3 lightPos = sceneInstance->dirLight_ObjInstance->GetPos();
    simpleShadowShader.use();
    simpleShadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    simpleShadowShader.setVec3("viewPos", camera->Position);
    simpleShadowShader.setVec3("lightPos", lightPos);
    simpleShadowShader.setVec3("lightDir", glm::normalize(sunDir));
    simpleShadowShader.setVec3("lightColor", sceneInstance->dirLight_ObjInstance->light->color);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    simpleShadowShader.setInt("shadowMap", 11);


    if (forwardScene1 != nullptr) //special cases for first scene
        forwardScene1->SetupShaderMaterial();


    sceneInstance->RenderSceneInstance(&simpleShadowShader, renderSelected);
    glActiveTexture(GL_TEXTURE0);

}


#pragma endregion

#pragma region Other_debug

//only testing this function, geting uniforms from shader
void PrintShader() {
    auto prog = colShader.ID;
    GLint numUniforms = 0;
    glGetProgramInterfaceiv(prog, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
    GLenum properties[4] = {GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION};

    for (int unif = 0; unif < numUniforms; ++unif) {
        GLint values[4];
        glGetProgramResourceiv(prog, GL_UNIFORM, unif, 4, properties, 4, NULL, values);

        // Skip any uniforms that are in a block.
        if (values[0] != -1)
            continue;

        // Get the name.  use a std::vector rather for C++03 compatibility
        // with C++11 we can use std::string directly
        std::vector<char> nameData(values[2]);
        glGetProgramResourceName(prog, GL_UNIFORM, unif, nameData.size(), NULL, &nameData[0]);

        //glGetProgramParameterdvNV
        std::string name(nameData.begin(), nameData.end() - 1);
        std::cout << "string " << name;

    }

}


#pragma endregion

void MoveCamera() {
    //get camera pos, calculate new destination
    glm::vec3 curCamPos = camera->Position;
    glm::vec3 destination = freeCamPath.finalPoints[nextCamPosIndex];
    glm::vec3 destinationDir = destination - curCamPos;

    //might cause issues if it goes too fast and overshoots
    //if destination is close enough, pick next point
    if (glm::length(destinationDir) <= 0.1f)//.length() is wrong
    {
        nextCamPosIndex += 1;
        //loop the next point if out of range
        nextCamPosIndex = nextCamPosIndex % (freeCamPath.finalPoints.size() - 1);
        std::cout << "reset Cam, new index is " << nextCamPosIndex << "\n";
    }
    freeCamT = cameraSpeed * deltaTime;
    glm::vec3 nexCamPos =
            curCamPos + glm::normalize(freeCamPath.finalPoints[nextCamPosIndex] - curCamPos) * 0.01 * freeCamT;


    camera->Position = nexCamPos;
    glm::vec3 target = freeCamLookDir - curCamPos;
    if (sceneInstance->selectedInstance)
        target = sceneInstance->selectedInstance->GetPos() - curCamPos;
    camera->updateCameraVectors(target);
}

int main() {

    /* check c++ version
    if (__cplusplus == 201703L) std::cout << "C++17\n";
    else if (__cplusplus == 201402L) std::cout << "C++14\n";
    else if (__cplusplus == 201103L) std::cout << "C++11\n";
    else if (__cplusplus == 199711L) std::cout << "C++98\n";
    else std::cout << "pre-standard C++\n";
    */

    //GLFWwindow* window;

    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //make it decl. WindowSettings windowSettings = WindowSettings();
    //dont forget scene, window and monitor

    windowSettings.monitor = glfwGetPrimaryMonitor();
    windowSettings.window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);
    if (!windowSettings.window) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();

        return -1;
    }


    glfwMakeContextCurrent(windowSettings.window);
    glfwSetFramebufferSizeCallback(windowSettings.window, framebuffer_size_callback);
    glfwSetKeyCallback(windowSettings.window, keyboard_callback);
    //glfwSetCharCallback(windowSettings.window, character_callback);//use either char or key not both-doubled inputs
    glfwSetCursorPosCallback(windowSettings.window, mouse_callback);
    glfwSetScrollCallback(windowSettings.window, scroll_callback);
    glfwSetMouseButtonCallback(windowSettings.window, mouse_button_callback);
    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetInputMode(windowSettings.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    //glad needs to be after make context current window
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true); //this is needed for models, but screws our edited skybox

    std::cout << "Hello, World!" << std::endl;
    ::Help();
    stencilShader = Shader("..\\Assets\\Shaders\\Forward\\stencil.vert",
                           "..\\Assets\\Shaders\\Forward\\stencil.frag"); //maybe add as a ponter and then new too



    //sc->windowSettings = &windowSettings; //toto nebude upne dobre treba to?
    camera = new Camera(glm::vec3(0.0f, .0f, 3.0f));
    graphicsOptions = new GraphicsOptions();


    centroidModel = new Model("../Assets/Models/LightCube/LightCube.obj");
    //light_shader = new Shader("..\\Assets\\Shaders\\Forward\\MultipleLights\\s_light.vert",
    //                         "..\\Assets\\Shaders\\Forward\\MultipleLights\\s_light.frag");;

    // init IMGUI
    //  Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(windowSettings.window,
                                 true);  // remove callbacks with false, to manually control them, but let it be true to allow for input (of chars prolly)
    ImGui_ImplOpenGL3_Init(NULL);
    ImGuiIO &io = ImGui::GetIO();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGui::SetWindowFocus(nullptr);  // start unfocussed

    // build and compile shaders
    // -------------------------
    int enableSpotlight = 1;
    colShader = Shader("..\\Assets\\Shaders\\Forward\\basic.vert", "..\\Assets\\Shaders\\Forward\\basic.frag");
    //TODO raycasting z misi nejde, ked zapnem lights tak je stale z obrazovky
    //TODO depth map FBO shaders are wrong
    // configure depth map FBO
    simpleDepthShader = Shader("..\\Assets\\Shaders\\Forward\\ShadowMap\\ShadowMapDepth.vert",
                               "..\\Assets\\Shaders\\Forward\\ShadowMap\\ShadowMapDepth.frag");
    debugDepthQuad = Shader("..\\Assets\\Shaders\\Debug\\DebugQuad.vert", "..\\Assets\\Shaders\\Debug\\DebugQuad.frag");
    simpleShadowShader = Shader("..\\Assets\\Shaders\\Forward\\ShadowMap\\meshShadow.vert",
                                "..\\Assets\\Shaders\\Forward\\ShadowMap\\meshShadow.frag");
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                 NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //modify this so it isnt dark outside of depth map
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //setup shader
    debugDepthQuad.use();
    debugDepthQuad.setInt("depthMap", 10);

    //drawnline config
    GLuint lineVAO, lineVBO;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


    ReloadScene(3);
    camera->toggleCursor(); //set to hidden by default

    //create free camera path
    freeCamPath.bezierShape(1);

    //*************------------***************
    // ----------- render loop ---------------
    // ------------************---------------
    while (!glfwWindowShouldClose(windowSettings.window)) {

        PerfAnalyzer::drawcallCount = 0; //reset for new frame
        float currentFrame = glfwGetTime(); // delta time used for camera now
        deltaTime = currentFrame - lastFrame;


        if (camera->blockControls) {
            MoveCamera();
            //std::cout<< "movCam \n" ;
        }

        uniforms.view = camera->GetViewMatrix(); //uniformy su ine tu ako v drawskybox //see translation unit, .cpp is one TU...
        uniforms.projection = glm::perspective(glm::radians(camera->Zoom),
                                               (float) windowSettings.CUR_WIDTH / (float) windowSettings.CUR_HEIGHT,
                                               zNear, zFar);
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //std::cout<< "uniV main\n" << glm::to_string(uniforms.view);
        glm::mat4 modelMat = glm::mat4(1.0f);

        glClearColor(backgroundClearCol[0], backgroundClearCol[1], backgroundClearCol[2], backgroundClearCol[3]);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (enableCulling)
            glEnable(GL_CULL_FACE);
        else
            glDisable(GL_CULL_FACE);

        if (!camera->showCursor) {
            glfwSetInputMode(windowSettings.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            ImGui::SetWindowFocus(nullptr);
        } else {
            glfwSetInputMode(windowSettings.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        simpleShadowShader.use();
        simpleShadowShader.setVec4("plane", glm::vec4(0, 1, 0, 0));


        if (sceneInstance != nullptr && sceneInstance->dirLight_ObjInstance != nullptr) {
            if (graphicsOptions->enableShadows && !graphicsOptions->enableWater) {
                //TODO make this code better...
                //std::cout << "test\n";
                //renderbackfaces to fix peter panning for opaque objects, page 294
                glCullFace(GL_FRONT);
                glm::mat4 LSM = RenderDepth();
                glCullFace(GL_BACK);

                RenderTest(LSM, true);

            }
                //TODO make this better / not in main...
            else if (graphicsOptions->enableWater) {
                //TODO this is baaad code...

                //check if we are in correct scene

                deferredScene2 = dynamic_pointer_cast<DeferredScene2>(
                        sceneInstance);// if we want to chache the result, we need to clear this
                //bool same = dynamic_cast<DeferredScene2*>(sceneInstance.get()) != nullptr;

                //if(deferredScene2 || same)
                //if(same) {
                if (deferredScene2) {


                    deferredScene2->waterObjInstance->disableRender = true;
                    deferredScene2->waterObjInstance->forceRenderOwnShader = false;
                    glCullFace(GL_FRONT);
                    glm::mat4 LSM = RenderDepth();
                    glCullFace(GL_BACK);

                    //create plane, and clipping distance - modify meshShadow.vert
                    glEnable(GL_CLIP_DISTANCE0);
                    simpleShadowShader.use();
                    //set the height
                    float height = deferredScene2->waterObjInstance->GetPos().y;
                    simpleShadowShader.setVec4("plane", glm::vec4(0, 1, 0, -height));

                    //also position the camera for the reflection
                    float camHeight = 2 * (camera->Position.y - height);
                    //glm::vec3 origCamPos = camera->Position;
                    camera->Position.y -= camHeight;
                    camera->Pitch = -camera->Pitch;
                    //dont forget to update the view matrix
                    camera->updateCameraVectors();
                    uniforms.view = camera->GetViewMatrix();
                    //reflection

                    glBindFramebuffer(GL_FRAMEBUFFER,
                                      deferredScene2->reflectionFrameBuffer);// or deferredScene2->bindReflectionFrameBuffer();
                    RenderTest(LSM);
                    //return the cam
                    camera->Position.y += camHeight;
                    camera->Pitch = -camera->Pitch;
                    //dont forget to update the view matrix
                    camera->updateCameraVectors();
                    uniforms.view = camera->GetViewMatrix();
                    //unbind();
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);


                    //refraction
                    simpleShadowShader.use();
                    simpleShadowShader.setVec4("plane", glm::vec4(0, -1, 0, height));
                    glBindFramebuffer(GL_FRAMEBUFFER,
                                      deferredScene2->refractionFrameBuffer);// or deferredScene2->bindReflectionFrameBuffer();

                    //also position the camera for the refraction

                    RenderTest(LSM);
                    //unbind();
                    glBindFramebuffer(GL_FRAMEBUFFER, 0);

                    //render normally
                    //disable clipping
                    glDisable(GL_CLIP_DISTANCE0); //might not work?
                    //enable water rendering + to draw with its own shader
                    deferredScene2->waterObjInstance->disableRender = false;
                    deferredScene2->waterObjInstance->forceRenderOwnShader = true;
                    //set the shader vals
                    deferredScene2->waterShader.use();
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, deferredScene2->reflectionTexture); //do i need this line?
                    deferredScene2->waterShader.setInt("reflectionTexture", 0);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, deferredScene2->refractionTexture); //do i need this line?
                    deferredScene2->waterShader.setInt("refractionTexture", 1);


                    auto sun = deferredScene2->dirLight_ObjInstance;
                    deferredScene2->waterShader.setVec3("lightPos", sun->GetPos());
                    deferredScene2->waterShader.setVec3("lightColor", dynamic_cast<Light *>(sun->light)->color);

                    deferredScene2->waterShader.setFloat("zNear", zNear);
                    deferredScene2->waterShader.setFloat("zFar", zFar);

                    RenderTest(LSM, true);
                    //render water quad
                    //deferredScene2->RenderWater();

                    //sceneInstance->RenderSceneInstance(nullptr);
                } else {
                    if (forwardScene1 != nullptr)
                        forwardScene1->SetupShaderMaterial();
                    sceneInstance->RenderSceneInstance(nullptr, true);
                }
            } else {
                if (forwardScene1 != nullptr)
                    forwardScene1->SetupShaderMaterial();
                sceneInstance->RenderSceneInstance(nullptr, true);
            }

            //also
            sceneInstance->RenderLights();
            // draw debug line pointing from light pos to dir

            //TODO memory leak...
            if (enableDebugLightRay) {
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
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *) 0);
                glLineWidth(6.3f);
                glDrawArrays(GL_LINE_STRIP, 0, 3);
                PerfAnalyzer::drawcallCount++;
                glBindVertexArray(0);
            }

            glm::mat4 model = glm::mat4(1.0f);
            stencilShader.use();
            model = glm::translate(model, centroidPos);
            stencilShader.setMat4("model", model);
            stencilShader.setMat4("view", uniforms.view);
            stencilShader.setMat4("projection", uniforms.projection);
            centroidModel->Draw(stencilShader, true);

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
                colShader.setMat4("view", camera->GetViewMatrix());
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

        }

        //TODO add more working parameters for objects
        //TODO test loading of object
        //TODO maybe allow changing shader params
        //TODO make sun color functional again
        //TODO separate,cleanup,refactor the code for shadowmaps...
        lastFrame = currentFrame;

        DrawImGui();

        if (sceneInstance != nullptr)
            sceneInstance->ImGuiHierarchy();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        camera->ProcessKeyboard(Camera_Movement::NONE, deltaTime);
        glfwSwapBuffers(windowSettings.window);
        std::cout.flush();
    }
    glfwTerminate();
    return 0;
}
