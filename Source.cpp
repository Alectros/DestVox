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
    Shader program2("l10T.vs","l10_thing_many_lights.fs");

    Model s("Objects/Cube/cube.obj");
    Model orTest("Objects/RUP/sirFigRUL.obj");
    Model Stank("Objects/STank/tank.obj");
   // Model backpack("Objects/Backpack/backpack.obj");
    //Model shaman("Objects/Shaman_models/Notext/shaman.obj");

    SingleObject cubestart(program1, "shaman2", &s, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    SingleObject tank1(program1, "shaman2", &Stank, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    SingleObject tank2(program1, "shaman2", &Stank, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    SingleObject tank3(program1, "shaman2", &Stank, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    tank1.SetInitial(glm::vec3(0.0f, 0.0f, -1.0f));
    tank2.SetInitial(glm::vec3(0.0f, 0.0f, -1.0f));
    tank3.SetInitial(glm::vec3(0.0f, 0.0f, -1.0f));

    CombinedObject sprite(program1, "object");
    //sprite.MoveTo(1.0f,1.0f,1.0f);
    sprite.AddObject(&tank1);
    sprite.AddObject(&tank2);
    sprite.AddObject(&tank3);

  //   sprite.SetUpVector(0.0, 1.0f, 1.0f);

    //Figure.SetInitial(glm::vec3(0.0f, 0.0f, -1.0f));


    DirectedLight dir(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.3f, 0.7f, 1.0f));
    vector<PointLight> lightP;
    vector<SpotLight> lightS;

    vector<float> outangle;

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

       // tank.Draw(camera, parameters, dir, lightP, lightS);
        //tank.LookPoint(camera.Position);
        //tank.LookDirection(camera.Front);

        sprite.LookDirection(camera.Front);
        sprite.Draw(camera, parameters, dir, lightP, lightS);
        
      
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