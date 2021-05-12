#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ProgramParams.h"
#include "Tools/stb_image.h"
#include "Tools/Camera.h"
#include "Scene.h"
#include "World3DObject.h"
#include <fstream>
#include <Windows.h>
#include "Octree.h"
#include "Voxel.h"
#include "Templates.cpp"
#include "VoxelObject.h"
#include "VoxelMaterial.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

ProgramParams parameters;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(parameters.SCR_WIDTH, parameters.SCR_HEIGHT, "DestVox", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    Camera camera(glm::vec3(0.0f, 3.0f, 7.0f));
    parameters.userView = &camera;

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //stbi_set_flip_vertically_on_load(true);


     
    auto materialsVXL = LoadMaterials("VoxelMaterials/vxmat.txt");

    Shader program1("Shaders/Models/TextNol10T.vs","Shaders/Models/TextNoLightsMany.fs");
    Shader program2("Shaders/Models/l10T.vs","Shaders/Models/l10_thing_many_lights.fs");
    Shader voxelProgram("Shaders/Voxel/Materials/VoxelVertexMaterialShader.vs","Shaders/Voxel/Materials/VoxelFragmentMaterialShader.fs");

    OctreeNode<Voxel> testOcto(4);

    Voxel* testVoxel = new Voxel(materialsVXL["cyan_plastic"],glm::vec3(0.5f, -1.5f, 1.5f), 1);
    Voxel* testVoxel1 = new Voxel(materialsVXL["cyan_plastic"], glm::vec3(0.5f, 0.5f, -0.5f), 1);
    Voxel* testVoxel2 = new Voxel(materialsVXL["cyan_plastic"], glm::vec3(-0.5f, 0.5f, 0.5f), 1);
    Voxel* testVoxel3 = new Voxel(materialsVXL["cyan_plastic"], glm::vec3(-0.5f, -0.5f, -0.5f), 1);
    Voxel* testVoxel4 = new Voxel(materialsVXL["cyan_plastic"], glm::vec3(-1.5f, -1.5f, -1.5f), 1);
    Voxel* testVoxel5 = new Voxel(materialsVXL["cyan_plastic"], glm::vec3( 1.5f,  1.5f,  1.5f), 1);

    testOcto.SetLeaf(testVoxel->position, testVoxel);
    testOcto.SetLeaf(testVoxel1->position, testVoxel1);
    testOcto.SetLeaf(testVoxel2->position, testVoxel2);
    testOcto.SetLeaf(testVoxel3->position, testVoxel3);
    testOcto.SetLeaf(testVoxel4->position, testVoxel4);
    testOcto.SetLeaf(testVoxel5->position, testVoxel5);

    Model voxcubeModel("Objects/voxCube/voxcube.obj");

    VoxelObject voxTest(voxelProgram, "voxelTest", &voxcubeModel , &testOcto);
    voxTest.Move(0.0f, 2.0f, 0.0f);

    voxTest.SetShell();

    OctreeNode<Voxel> circleVox(4);

    VoxelObject circle3(voxelProgram, "voxelCircle", &voxcubeModel, &circleVox);

    Voxel* testV1 = new Voxel(materialsVXL["gold"], glm::vec3(0.5f, 0.5f, 0.5f), 1);
    Voxel* testV2 = new Voxel(materialsVXL["gold"], glm::vec3(-0.5f, 0.5f, 0.5f), 1);
    Voxel* testV3 = new Voxel(materialsVXL["gold"], glm::vec3(0.5f, -0.5f, 0.5f), 1);
    Voxel* testV4 = new Voxel(materialsVXL["gold"], glm::vec3(0.5f, 0.5f, -0.5f), 1);
    Voxel* testV5 = new Voxel(materialsVXL["gold"], glm::vec3(1.5f, 0.5f, 0.5f), 1);
    Voxel* testV6 = new Voxel(materialsVXL["gold"], glm::vec3(0.5f, 1.5f, 0.5f), 1);
    Voxel* testV7 = new Voxel(materialsVXL["gold"], glm::vec3(0.5f, 0.5f, 1.5f), 1);

    circleVox.SetLeaf(testV1->position, testV1);
    circleVox.SetLeaf(testV2->position, testV2);
    circleVox.SetLeaf(testV3->position, testV3);
    circleVox.SetLeaf(testV4->position, testV4);
    circleVox.SetLeaf(testV5->position, testV5);
    circleVox.SetLeaf(testV6->position, testV6);
    circleVox.SetLeaf(testV7->position, testV7);

    circle3.SetShell();

    circle3.Move(2.0f, 1.0f, 2.0f);

    circle3.ScaleTo(0.2f, 0.2f, 0.2f);

    DirectedLight dir(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.5f, 0.8f, 1.0f));
    vector<PointLight*> lightP;
    PointLight light1(glm::vec3(1.0f, 0.2f, 1.0f), glm::vec3(2.5f, 1.0f, 2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light2(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-2.5f, 5.0f, 2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.5f, 5.0f, -2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light4(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-2.5f, 5.0f, -2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));

    //lightP.push_back(&light1);

    SpotLight camSPL(glm::vec3(1.0f), camera.Position, camera.Front, 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)));
    vector<SpotLight*> lightS;
    lightS.push_back(&camSPL);

    Model tankModel("Objects/Circle_tank/no4/ctank.obj");
    tankModel.SetInitialDirection(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    Model tankGun = tankModel.GetSubModel(vector<string>(1, "Gun_Cylinder"), "gun");
    tankGun.SetInitialMovement(glm::vec3(0.0f, -1.6f, 0.0f));
    Model tankBody = tankModel.GetSubModel(vector<string>(1, "Body_Cube.001"), "body");
    Model tankTurret = tankModel.GetSubModel(vector<string>(1, "Turret_Sphere.001"), "turret");
    tankTurret.SetInitialMovement(glm::vec3(0.0f, -1.6f, 0.0f));

    SingleObject gunObj(program2,"gun",&tankGun);
    SingleObject bodyObj(program2,"body",&tankBody);
    SingleObject turretObj(program2,"turret",&tankTurret);

    SingleObject gunObj2(program2, "gun", &tankGun);
    SingleObject bodyObj2(program2, "body", &tankBody);
    SingleObject turretObj2(program2, "turret", &tankTurret);

    gunObj.Move(0.0f, 1.6f, 0.0f);
    turretObj.Move(0.0f, 1.6f, 0.0f);

    gunObj2.Move(0.0f, 1.6f, 0.0f);
    turretObj2.Move(0.0f, 1.6f, 0.0f);

    CombinedObject Ctank1(program2,"tank1");
    Ctank1.AddObject(&gunObj);
    Ctank1.AddObject(&bodyObj);
    Ctank1.AddObject(&turretObj);

    Ctank1.MoveTo(-5.0f, 0.0f, 0.0f);

    CombinedObject Ctank2(program2, "tank2");
    Ctank2.AddObject(&gunObj2);
    Ctank2.AddObject(&bodyObj2);
    Ctank2.AddObject(&turretObj2);

    Ctank2.MoveTo(5.0f, 0.0f, 0.0f);

    CombinedObject pair(program2, "pair");

    pair.AddObject(&Ctank1);
    pair.AddObject(&Ctank2);


    vector<string> names(1, "gun");
    vector<string> names2(1, "turret");

    pair.MoveTo(-15.0f, 0.2f, 0.0f);

    SingleObject plates(program2,"plates", "Objects/Airdrome/plates.obj");
    SingleObject grass(program2,"plates", "Objects/Airdrome/grass.obj");
    CombinedObject map(program2, "map");
    map.AddObject(&plates);
    map.AddObject(&grass);

    SingleObject cubestart(program1, "cube", "Objects/Cube/cube.obj");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        // Логическая часть работы со временем для каждого кадра
        float currentFrame = glfwGetTime();
        parameters.deltaTime = currentFrame - parameters.lastFrame;
        parameters.lastFrame = currentFrame;

        // Обработка ввода
        processInput(window);

        // Рендеринг
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camSPL.position = camera.Position;
        camSPL.direction = camera.Front;

       // pair.LookPoint(camera.Position);
        //Ctank1.RotationAxes(0.5f, 0.0f, 1.0f, 0.0f);

        //Ctank1.PartLookPoint(names, camera.Position - pair.position);
        //Ctank2.PartLookPoint(names, camera.Position - pair.position);
        //Ctank1.PartLookPointLocked(names2, camera.Position - pair.position);
        //Ctank2.PartLookPointLocked(names2, camera.Position - pair.position);

        pair.Draw(camera, parameters, dir, lightP, lightS);
        map.Draw(camera, parameters, dir, lightP, lightS);
        voxTest.Draw(camera, parameters, dir, lightP, lightS);
        circle3.Draw(camera, parameters, dir, lightP, lightS);
        cubestart.Draw(camera, parameters, dir, lightP, lightS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


	return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        (*parameters.userView).ProcessKeyboard(FORWARD, parameters.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        (*parameters.userView).ProcessKeyboard(BACKWARD, parameters.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        (*parameters.userView).ProcessKeyboard(LEFT, parameters.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        (*parameters.userView).ProcessKeyboard(RIGHT, parameters.deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// glfw: всякий раз, когда перемещается мышь, вызывается данная callback-функция
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (parameters.firstMouse)
    {
        parameters.lastX = xpos;
        parameters.lastY = ypos;
        parameters.firstMouse = false;
    }

    float xoffset = xpos - parameters.lastX;
    float yoffset = parameters.lastY - ypos;

    parameters.lastX = xpos;
    parameters.lastY = ypos;

    (*parameters.userView).ProcessMouseMovement(xoffset, yoffset);
}

// glfw: всякий раз, когда прокручивается колесико мыши, вызывается данная callback-функция
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (*parameters.userView).ProcessMouseScroll(yoffset);
}