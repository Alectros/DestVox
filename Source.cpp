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

    Shader program1("TextNol10T.vs","TextNoLightsMany.fs");
    Shader program2("l10T.vs","l10_thing_many_lights.fs");



    DirectedLight dir(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.5f, 0.8f, 1.0f));
    vector<PointLight> lightP;
    PointLight light1(glm::vec3(1.0f), glm::vec3(2.5f, 1.0f, 2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light2(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-2.5f, 5.0f, 2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.5f, 5.0f, -2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light4(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-2.5f, 5.0f, -2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));

    lightP.push_back(light1);

    vector<SpotLight> lightS;

    Model tankModel("Objects/Circle_tank/no4/ctank.obj");
    Model mapModel("Objects/standart_map/map.obj");
    tankModel.SetInitialDirection(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

    SingleObject mapObj(program2, "map", &mapModel);

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

       // pair.LookPoint(camera.Position);
        Ctank1.RotationAxes(0.5f, 0.0f, 1.0f, 0.0f);

        Ctank1.PartLookPoint(names, camera.Position);
        Ctank2.PartLookPoint(names, camera.Position);
        Ctank1.PartLookPointLocked(names2, camera.Position);
        Ctank2.PartLookPointLocked(names2, camera.Position);

        mapObj.Draw(camera, parameters, dir, lightP, lightS);
        pair.Draw(camera, parameters, dir, lightP, lightS);

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