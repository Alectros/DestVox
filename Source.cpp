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

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
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
    stbi_set_flip_vertically_on_load(true);

    Shader program1("TextNol10T.vs","TextNoLightsMany.fs");

    Model s("Objects/Cube/cube.obj");
    Model orTest("Objects/RUP/sirFigRUL.obj");

    SingleObject cubestart(program1, "shaman2", &s, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    SingleObject Figure(program1, "Figure", &orTest, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    SingleObject Figure2(program1, "Figure", &orTest, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    
    DirectedLight dir(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.3f, 0.7f, 1.0f));
    vector<PointLight> lightP;
    vector<SpotLight> lightS;
  //  PointLight Plight(glm::vec3(0.5f, 0.5f, 1.0f), glm::vec3(0.0, 5.0, 0.0), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
//    lightP.push_back(Plight);

    vector<float> outangle;

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        // ���������� ����� ������ �� �������� ��� ������� �����
        float currentFrame = glfwGetTime();
        parameters.deltaTime = currentFrame - parameters.lastFrame;
        parameters.lastFrame = currentFrame;

        // ��������� �����
        processInput(window);

        // ���������
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        cubestart.Draw(camera, parameters, dir, lightP, lightS);


        Figure.Rotation(1.0f, 0.0f, -1.0f, -1.0f);
        

        Figure.Draw(camera, parameters, dir, lightP, lightS);

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


// glfw: ������ ���, ����� ������������ ����, ���������� ������ callback-�������
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

// glfw: ������ ���, ����� �������������� �������� ����, ���������� ������ callback-�������
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (*parameters.userView).ProcessMouseScroll(yoffset);
}