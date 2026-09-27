
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "sphere.h"

#include <iostream>
#include <cmath>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

void drawCube(unsigned int& cubeVAO, Shader& shader, glm::mat4 model,
              glm::vec3 ambient, glm::vec3 diffuse,
              glm::vec3 specular = glm::vec3(0.5f), float shininess = 32.0f);

void drawTheatre(unsigned int& cubeVAO, Shader& shader);
void drawChair(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos, float rotY);
void drawFan(unsigned int& cubeVAO, Shader& shader, glm::vec3 center);

const unsigned int SCR_WIDTH = 1100;
const unsigned int SCR_HEIGHT = 700;

//Camera camera(glm::vec3(0.0f, 4.2f, 14.0f), glm::vec3(0,1,0), -90.0f, -12.0f);

Camera camera(glm::vec3(0.0f, 4.0f, 10.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    90.0f, 0.0f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float fanAngle = 0.0f;
bool fanOn = true;
bool lightsOn = true;
bool spotlightOn = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 pointLightPositions[] = {
    glm::vec3(-5.0f, 5.5f, 1.5f),
    glm::vec3( 5.0f, 5.5f, 1.5f)
};

PointLight pointlight1(
    -5.0f, 5.5f, 1.5f,
    0.08f,0.05f,0.05f,
    1.0f,0.35f,0.25f,
    1.0f,0.8f,0.7f,
    1.0f,0.09f,0.032f,1);

PointLight pointlight2(
     5.0f, 5.5f, 1.5f,
    0.05f,0.05f,0.08f,
    0.55f,0.65f,1.0f,
    0.7f,0.8f,1.0f,
    1.0f,0.09f,0.032f,2);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH, SCR_HEIGHT, "Movie Theatre - 3D Graphics Project",
        NULL, NULL);

    if (window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Teacher's sample structure: Phong shader + simple shader for visible light sources.
    Shader lightingShader("vertexShaderForPhongShading.vs",
                          "fragmentShaderForPhongShading.fs");
    Shader lampShader("vertexShader.vs", "fragmentShader.fs");

    // Same cube geometry style as the supplied sample.
    float cube_vertices[] = {
        0,0,0,  0,0,-1,   1,0,0,  0,0,-1,   1,1,0,  0,0,-1,   0,1,0,  0,0,-1,
        1,0,0,  1,0,0,    1,1,0,  1,0,0,    1,0,1,  1,0,0,    1,1,1,  1,0,0,
        0,0,1,  0,0,1,    1,0,1,  0,0,1,    1,1,1,  0,0,1,    0,1,1,  0,0,1,
        0,0,1, -1,0,0,    0,1,1, -1,0,0,    0,1,0, -1,0,0,    0,0,0, -1,0,0,
        1,1,1,  0,1,0,    1,1,0,  0,1,0,    0,1,0,  0,1,0,    0,1,1,  0,1,0,
        0,0,0,  0,-1,0,   1,0,0,  0,-1,0,   1,0,1,  0,-1,0,   0,0,1,  0,-1,0
    };

    unsigned int cube_indices[] = {
        0,3,2, 2,1,0,  4,5,7, 7,6,4,
        8,9,10, 10,11,8,  12,13,14, 14,15,12,
        16,17,18, 18,19,16,  20,21,22, 22,23,20
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    Sphere projectorLens(0.35f, 32, 16,
        glm::vec3(0.05f,0.05f,0.05f),
        glm::vec3(0.08f,0.08f,0.12f),
        glm::vec3(1.0f), 64.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        if (fanOn)
            fanAngle += 220.0f * deltaTime;

        glClearColor(0.025f, 0.025f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        pointlight1.setUpPointLight(lightingShader);
        pointlight2.setUpPointLight(lightingShader);

        // Spotlight mounted near the projector, aimed toward the screen.
        //lightingShader.setVec3("spotLight.position", glm::vec3(0.0f, 6.0f, 2.8f));
        //lightingShader.setVec3("spotLight.direction", glm::vec3(0.0f, -1.0f, -0.35f));
        //lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(18.0f)));
        //lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(30.0f)));

        // Projector spotlight aimed directly at the movie screen
        glm::vec3 projectorPos = glm::vec3(0.0f, 5.78f, 1.55f);
        glm::vec3 screenCenter = glm::vec3(0.0f, 3.55f, -3.76f);

        // Direction from projector to screen
        glm::vec3 projectorDirection =
            glm::normalize(screenCenter - projectorPos);

        lightingShader.setVec3("spotLight.position", projectorPos);
        lightingShader.setVec3("spotLight.direction", projectorDirection);

        lightingShader.setFloat(
            "spotLight.cutOff",
            glm::cos(glm::radians(15.0f))
        );

        lightingShader.setFloat(
            "spotLight.outerCutOff",
            glm::cos(glm::radians(25.0f))
        );

        if (spotlightOn)
        {
            lightingShader.setVec3("spotLight.ambient", glm::vec3(0.04f,0.04f,0.04f));
            lightingShader.setVec3("spotLight.diffuse", glm::vec3(1.0f,0.82f,0.55f));
            lightingShader.setVec3("spotLight.specular", glm::vec3(1.0f));
        }
        else
        {
            lightingShader.setVec3("spotLight.ambient", glm::vec3(0.0f));
            lightingShader.setVec3("spotLight.diffuse", glm::vec3(0.0f));
            lightingShader.setVec3("spotLight.specular", glm::vec3(0.0f));
        }

        if (!lightsOn)
        {
            pointlight1.turnOff();
            pointlight2.turnOff();
        }
        else
        {
            pointlight1.turnOn();
            pointlight2.turnOn();
        }

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom),
            (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view = camera.GetViewMatrix();

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        drawTheatre(cubeVAO, lightingShader);
        drawFan(cubeVAO, lightingShader, glm::vec3(0.0f, 6.15f, 2.0f));

        // Projector body
        glm::mat4 projector = glm::mat4(1.0f);
        projector = glm::translate(projector, glm::vec3(0.0f,5.65f,2.0f));
        projector = glm::scale(projector, glm::vec3(1.25f,0.45f,0.75f));
        drawCube(cubeVAO, lightingShader, projector,
                  glm::vec3(0.04f), glm::vec3(0.12f,0.14f,0.18f),
                  glm::vec3(0.9f), 64.0f);

        glm::mat4 lensModel = glm::mat4(1.0f);
        lensModel = glm::translate(lensModel, glm::vec3(0.0f,5.78f,1.55f));
        lensModel = glm::scale(lensModel, glm::vec3(1.0f));
        projectorLens.drawSphere(lightingShader, lensModel);

        // Visible light cubes (simple shader)
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        glBindVertexArray(lightCubeVAO);

        for (int i=0; i<2; ++i)
        {
            glm::mat4 m(1.0f);
            m = glm::translate(m, pointLightPositions[i]);
            m = glm::scale(m, glm::vec3(0.15f));
            lampShader.setMat4("model", m);
            lampShader.setVec3("color", i==0 ?
                glm::vec3(1.0f,0.45f,0.18f) :
                glm::vec3(0.25f,0.55f,1.0f));
            glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void drawCube(unsigned int& cubeVAO, Shader& shader, glm::mat4 model,
              glm::vec3 ambient, glm::vec3 diffuse,
              glm::vec3 specular, float shininess)
{
    shader.use();
    shader.setVec3("material.ambient", ambient);
    shader.setVec3("material.diffuse", diffuse);
    shader.setVec3("material.specular", specular);
    shader.setFloat("material.shininess", shininess);
    shader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}

void drawTheatre(unsigned int& cubeVAO, Shader& shader)
{
    // Floor
    glm::mat4 m(1.0f);
    m = glm::translate(m, glm::vec3(-7.0f,0.0f,-5.0f));
    m = glm::scale(m, glm::vec3(14.0f,0.25f,18.0f));
    drawCube(cubeVAO,shader,m,glm::vec3(0.04f),glm::vec3(0.13f,0.08f,0.06f),glm::vec3(0.3f),16);

    // Ceiling
    m=glm::mat4(1);
    m=glm::translate(m,glm::vec3(-7,7,-5));
    m=glm::scale(m,glm::vec3(14,0.25f,18));
    drawCube(cubeVAO,shader,m,glm::vec3(0.03f),glm::vec3(0.07f,0.07f,0.10f),glm::vec3(0.3f),16);

    // Side walls
    m=glm::mat4(1);
    m=glm::translate(m,glm::vec3(-7,0,-5));
    m=glm::scale(m,glm::vec3(0.3f,7,18));
    drawCube(cubeVAO,shader,m,glm::vec3(0.04f),glm::vec3(0.18f,0.05f,0.07f),glm::vec3(0.35f),24);

    m=glm::mat4(1);
    m=glm::translate(m,glm::vec3(6.7f,0,-5));
    m=glm::scale(m,glm::vec3(0.3f,7,18));
    drawCube(cubeVAO,shader,m,glm::vec3(0.04f),glm::vec3(0.18f,0.05f,0.07f),glm::vec3(0.35f),24);

    // Back wall
    m=glm::mat4(1);
    m=glm::translate(m,glm::vec3(-7,0,12.7f));
    m=glm::scale(m,glm::vec3(14,7,0.3f));
    drawCube(cubeVAO,shader,m,glm::vec3(0.03f),glm::vec3(0.08f,0.06f,0.11f),glm::vec3(0.3f),20);

    // Front stage platform
    m=glm::mat4(1);
    m=glm::translate(m,glm::vec3(-6.0f,0.25f,-3.9f));
    m=glm::scale(m,glm::vec3(12.0f,0.45f,2.0f));
    drawCube(cubeVAO,shader,m,glm::vec3(0.04f),glm::vec3(0.20f,0.12f,0.08f),glm::vec3(0.5f),32);

    // Screen frame
    m=glm::mat4(1);
    m=glm::translate(m,glm::vec3(-4.9f,1.2f,-3.55f));
    m=glm::scale(m,glm::vec3(9.8f,4.7f,0.18f));
    drawCube(cubeVAO,shader,m,glm::vec3(0.05f),glm::vec3(0.08f,0.09f,0.12f),glm::vec3(0.7f),64);

    // Screen surface
    //m=glm::mat4(1);
    //m=glm::translate(m,glm::vec3(-4.55f,1.55f,-3.76f));
    //m=glm::scale(m,glm::vec3(9.1f,4.0f,0.05f));
    //drawCube(cubeVAO,shader,m,glm::vec3(0.10f),glm::vec3(0.32f,0.38f,0.48f),glm::vec3(0.8f),96);

    m = glm::mat4(1);
    m = glm::translate(m, glm::vec3(-4.55f, 1.55f, -3.76f));
    m = glm::scale(m, glm::vec3(9.1f, 4.0f, 0.05f));

    drawCube(cubeVAO, shader, m,
        glm::vec3(0.25f, 0.25f, 0.25f),
        glm::vec3(0.65f, 0.70f, 0.80f),
        glm::vec3(1.0f), 96);

    // Red curtain pillars
    for(int s : {-1,1})
    {
        m=glm::mat4(1);
        m=glm::translate(m,glm::vec3(s*5.0f-0.5f,1.0f,-3.8f));
        m=glm::scale(m,glm::vec3(0.8f,4.8f,0.25f));
        drawCube(cubeVAO,shader,m,glm::vec3(0.06f,0.01f,0.01f),glm::vec3(0.5f,0.02f,0.04f),glm::vec3(0.4f),32);
    }

    // Ceiling decorative beams
    for(int x=-5;x<=5;x+=2)
    {
        m=glm::mat4(1);
        m=glm::translate(m,glm::vec3((float)x,6.65f,-2.0f));
        m=glm::scale(m,glm::vec3(0.12f,0.12f,12.0f));
        drawCube(cubeVAO,shader,m,glm::vec3(0.05f),glm::vec3(0.22f,0.18f,0.06f),glm::vec3(0.5f),32);
    }

    // 4 rows x 8 seats
    float zRows[4]={0.0f,2.4f,4.8f,7.2f};
    for(int r=0;r<4;r++)
        for(int c=0;c<8;c++)
        {
            float x=-5.25f+c*1.5f;
            drawChair(cubeVAO,shader,glm::vec3(x,0.45f,zRows[r]),0.0f);
        }
}

void drawChair(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos, float rotY)
{
    glm::mat4 base(1.0f);
    base=glm::translate(base,pos);
    base=glm::rotate(base,glm::radians(rotY),glm::vec3(0,1,0));

    // Seat
    glm::mat4 m=base;
    m=glm::scale(m,glm::vec3(1.05f,0.25f,1.0f));
    drawCube(cubeVAO,shader,m,glm::vec3(0.04f,0.01f,0.015f),glm::vec3(0.45f,0.03f,0.06f),glm::vec3(0.35f),24);

    // Backrest
    m=base;
    m=glm::translate(m,glm::vec3(0,0.75f,0.38f));
    m=glm::scale(m,glm::vec3(1.05f,1.35f,0.20f));
    drawCube(cubeVAO,shader,m,glm::vec3(0.04f,0.01f,0.015f),glm::vec3(0.55f,0.03f,0.07f),glm::vec3(0.4f),32);

    // Left and right armrests
    for(float x : {-0.48f,0.48f})
    {
        m=base;
        m=glm::translate(m,glm::vec3(x,0.60f,0.0f));
        m=glm::scale(m,glm::vec3(0.12f,0.35f,0.85f));
        drawCube(cubeVAO,shader,m,glm::vec3(0.03f),glm::vec3(0.08f,0.08f,0.10f),glm::vec3(0.4f),32);
    }
}

void drawFan(unsigned int& cubeVAO, Shader& shader, glm::vec3 center)
{
    // Motor housing
    glm::mat4 m=glm::mat4(1);
    m=glm::translate(m,center);
    m=glm::scale(m,glm::vec3(0.35f,0.22f,0.35f));
    drawCube(cubeVAO,shader,m,glm::vec3(0.04f),glm::vec3(0.15f,0.16f,0.18f),glm::vec3(0.9f),64);

    // Five rotating blades: modelling transformation = translation * rotation * scale
    for(int i=0;i<5;i++)
    {
        float a=fanAngle+i*72.0f;
        m=glm::mat4(1);
        m=glm::translate(m,center);
        m=glm::rotate(m,glm::radians(a),glm::vec3(0,1,0));
        m=glm::translate(m,glm::vec3(0.0f,0.0f,-1.0f));
        m=glm::scale(m,glm::vec3(0.22f,0.08f,1.05f));
        drawCube(cubeVAO,shader,m,glm::vec3(0.05f),glm::vec3(0.75f,0.75f,0.78f),glm::vec3(1.0f),96);
    }

    // Center cap
    m=glm::mat4(1);
    m=glm::translate(m,center);
    m=glm::scale(m,glm::vec3(0.22f));
    drawCube(cubeVAO,shader,m,glm::vec3(0.04f),glm::vec3(0.2f,0.2f,0.22f),glm::vec3(1.0f),64);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS)
        glfwSetWindowShouldClose(window,true);

    if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD,deltaTime);
    if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD,deltaTime);
    if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS)
        camera.ProcessKeyboard(LEFT,deltaTime);
    if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT,deltaTime);

    if(glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS)
        camera.ProcessMouseMovement(0,-40.0f*deltaTime);
    if(glfwGetKey(window,GLFW_KEY_DOWN)==GLFW_PRESS)
        camera.ProcessMouseMovement(0,40.0f*deltaTime);
    if(glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS)
        camera.ProcessMouseMovement(-40.0f*deltaTime,0);
    if(glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS)
        camera.ProcessMouseMovement(40.0f*deltaTime,0);
}

void key_callback(GLFWwindow* window,int key,int scancode,int action,int mods)
{
    if(action==GLFW_PRESS)
    {
        if(key==GLFW_KEY_F) fanOn=!fanOn;
        if(key==GLFW_KEY_L)
        {
            lightsOn=!lightsOn;
            if(!lightsOn){pointlight1.turnOff();pointlight2.turnOff();}
            else {pointlight1.turnOn();pointlight2.turnOn();}
        }
        if(key==GLFW_KEY_P) spotlightOn=!spotlightOn;
    }
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height)
{
    glViewport(0,0,width,height);
}

void mouse_callback(GLFWwindow* window,double xposIn,double yposIn)
{
    float xpos=(float)xposIn, ypos=(float)yposIn;
    if(firstMouse){lastX=xpos;lastY=ypos;firstMouse=false;}
    float xoffset=xpos-lastX;
    float yoffset=lastY-ypos;
    lastX=xpos;lastY=ypos;
    // Camera mouse control follows the teacher sample.
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS)
        camera.ProcessMouseMovement(xoffset,yoffset);
}

void scroll_callback(GLFWwindow* window,double xoffset,double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}
