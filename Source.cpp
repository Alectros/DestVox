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

    Camera camera(glm::vec3(5.0f, 3.0f, 5.0f));
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

    Model s("Objects/Cube/cube.obj");
    Model orTest("Objects/RUP/sirFigRUL.obj");
    Model Stank("Objects/STank/tank.obj");
    Model Ctank("Objects/Circle_tank/no4/Ctank.obj");
    Model mapModel("Objects/standart_map/map.obj");

    SingleObject Ktank(program2, "turret", &Ctank, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f));


    CombinedObject Otank(program2,"Ctank");
   

    SingleObject cubestart(program1, "shaman2", &s, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    SingleObject map(program2, "map", &mapModel);


    DirectedLight dir(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.5f, 0.8f, 1.0f));
    vector<PointLight> lightP;
    PointLight light1(glm::vec3(1.0f), glm::vec3(2.5f, 1.0f, 2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light2(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(-2.5f, 5.0f, 2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light3(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(2.5f, 5.0f, -2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));
    PointLight light4(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-2.5f, 5.0f, -2.5f), 1.0f, 0.045f, 0.0075f, glm::vec3(0.1f, 0.7f, 1.0f));

    //ofstream models("CtankMeshes.txt");

    //vector<string> meshes = Ctank.GetMeshNames();

    //for (int i = 0; i < meshes.size(); i++)
    //{
    //    models << meshes[i] << endl;
    //}
    //models.close();
    

    
    Model gun = Ctank.GetSubModel(vector<string>(1,"Gun_Cylinder"), "gun");
    Model body = Ctank.GetSubModel(vector<string>(1,"Body_Cube.001"), "gun");
    Model turret = Ctank.GetSubModel(vector<string>(1,"Turret_Sphere.001"), "gun");

    SingleObject Cgun(program2, "map", &gun, glm::vec3(0.0, 1.6f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0));
    SingleObject Cbody(program2, "map", &body, glm::vec3(0.0, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0));
    SingleObject Cturret(program2, "map", &turret, glm::vec3(0.0, 1.6f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0));

    Cgun.SetInitialMovement(glm::vec3(0.0f, -1.6f, 0.0f));
    Cturret.SetInitialMovement(glm::vec3(0.0f, -1.6f, 0.0f));

    Cgun.SetInitialDirection(glm::vec3(1.0f, 0.0f, 0.0f));
    Cbody.SetInitialDirection(glm::vec3(1.0f, 0.0f, 0.0f));
    Cturret.SetInitialDirection(glm::vec3(1.0f, 0.0f, 0.0f));

   

    Otank.AddObject(&Cgun);
    Otank.AddObject(&Cbody);
    Otank.AddObject(&Cturret);

    

    lightP.push_back(light1);

    vector<SpotLight> lightS;


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

        Cgun.LookPoint(camera.Position);
        Cturret.LookPointLocked(camera.Position, STATE_AROUND_UP);

        Otank.LookPointLocked(camera.Position, STATE_AROUND_UP);

        map.Draw(camera, parameters, dir, lightP, lightS);
        Otank.Draw(camera, parameters, dir, lightP, lightS);
        //Cgun.Draw(camera, parameters, dir, lightP, lightS);
       // Cbody.Draw(camera, parameters, dir, lightP, lightS);
        //Cturret.Draw(camera, parameters, dir, lightP, lightS);

        
        
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