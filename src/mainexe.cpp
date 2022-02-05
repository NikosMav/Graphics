#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <shader_m.h>
#include <filesystem.h>
#include <model.h>

#include <Sphere.h>

#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);  
void processInput(GLFWwindow *window);
float getActiveTime(); 
float getRotationTime();

#define TEXTURES_PATH_CONTAINER "resources/textures/container.png"
#define WOMAN_MODEL_PATH "resources/woman/woman1.obj"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 600

#define PROJECTION_FOV glm::radians(45.0f)
#define WOMAN_ORBITAL_RADIUS glm::vec3(0.0f, 0.0f,  5.0f)
#define POINT_ZERO glm::vec3(0.0f, 0.0f, 0.0f)
#define NUM_OF_CONTAINERS 6

// global orbital speed
float globalRotationSpeed = 1.0f;
float globalRotationSpeedIncrease = 0.02f;

// camera
const float cameraSpeed = 0.1f; 
const float cameraRotationSpeed = 0.02f; 

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 5.0f, 40.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
glm::vec3 cameraUpward = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraRotation = 0.0;
float cameraTilt = 0.0;

// time per frame
float deltaTime = 0.0f;
float lastFrameTime = 0.0f; 
float prevFrameTime = 0.0f; 
float prevRotationTime = 0.0f; 
float currentFrameTime = 0.0f;
float fps = 0.0f;
int framesRendered = 0.0f;
int prevSec = 0;

// pause and timers
bool paused = false;
float pausedTime = 0.0f;
#define PAUSE_SLEEP_TIME 100000


int main(){
    // initialize opengl core proifile 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // make a window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Main_window", NULL, NULL);
    if (window == NULL)    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    // initialize glad to handle pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    

    // adjust the view port size
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

    // enable global depth
    glEnable(GL_DEPTH_TEST) ;


    // float containerVertices[] = {
    //     // positions          // normals           // texture coords
    //     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

    //     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    //     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //     -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    //     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

    //     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
    //      0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //      0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
    //     -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
    //     -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    // };
    // // orbital radius for the containers
    // glm::vec3 containerOrbits[] = {
    //     glm::vec3( 0.0f,  0.0f,  7.0f),
    //     glm::vec3( 0.0f,  0.0f,  10.0f),
    //     glm::vec3( 0.0f,  0.0f,  14.0f),
    //     glm::vec3( 0.0f,  0.0f,  17.0f),
    //     glm::vec3( 0.0f,  0.0f,  19.0f),
    //     glm::vec3( 0.0f,  0.0f,  21.0f)
    // };

    // // orbital axes for the containers
    // glm::vec3 containerOrbitAxes[] = {
    //     glm::vec3( sin(glm::radians(0.0)),   cos(glm::radians(0.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(30.0)),  cos(glm::radians(30.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(45.0)),  cos(glm::radians(45.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(0.0)),   cos(glm::radians(0.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(0.0)),   cos(glm::radians(0.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(15.0)),  cos(glm::radians(15.0)),  0.0f)
    // };

    // // orbital speeds for the containers
    // float containerOrbitSpeeds[] = {
    //     2.0,1.0,0.3,1.0,0.1,0.5
    // };

    // // orbital offset for the containers
    // float containerOrbitOffsets[] = {
    //     2.0,5.0,1.0,2.0,4.0,8.0
    // };

    // // rotational axes for the containers
    // glm::vec3 containerRotationAxes[] = {
    //     glm::vec3( sin(glm::radians(45.0)),  cos(glm::radians(45.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(30.0)),  cos(glm::radians(30.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(45.0)),  cos(glm::radians(45.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(90.0)),  cos(glm::radians(90.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(75.0)),  cos(glm::radians(75.0)),  0.0f),
    //     glm::vec3( sin(glm::radians(15.0)),  cos(glm::radians(15.0)),  0.0f)
    // };

    // // rotational axes speeds for the containers
    // float containerRotationAxesSpeeds[] = {
    //     5.0,1.0,2.0,2.0,3.0,1.0
    // };

    // // rotational offset for the containers
    // float containerRotationAxesOffsets[] = {
    //     2.0,5.0,1.0,2.0,4.0,8.0
    // };


    // // generate container VAO
    // unsigned int VBO, containerVAO;
    // glGenVertexArrays(1, &containerVAO);
    // glGenBuffers(1, &VBO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(containerVertices), containerVertices, GL_STATIC_DRAW);

    // // strides for container VAO
    // glBindVertexArray(containerVAO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // glEnableVertexAttribArray(2);

    // // initialize the light VAO 
    // // VBO stays the same
    // unsigned int lightVAO;
    // glGenVertexArrays(1, &lightVAO);
    // glBindVertexArray(lightVAO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);


    
    // // generate and bind texture
    // unsigned int containerTexture;
    // glGenTextures(1, &containerTexture);  
    // glBindTexture(GL_TEXTURE_2D, containerTexture);
    // // texture warpping
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // // texture filtering
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // // get textures
    // int width, height, nrChannels;
    // unsigned char *texture_image_container = stbi_load(TEXTURES_PATH_CONTAINER, &width, &height, &nrChannels, 0); 
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_image_container);
    // glGenerateMipmap(GL_TEXTURE_2D);
    // stbi_image_free(texture_image_container);


    Model womanModel(FileSystem::getPath(WOMAN_MODEL_PATH));
    Sphere litSphere(40, 40, 0);

    Shader womanShader("shaders/woman_shader.vs", "shaders/woman_shader.fs");
    Shader lightingSphereShader("shaders/light_shader.vs", "shaders/light_shader.fs");

    //unsigned int m_sphereProgramID = LoadShaders("shader/sphereShader.vert", "shader/sphereShader.frag");
    // GLuint vertexPosition_modelspaceID = glGetAttribLocation(m_sphereProgramID, "vertexPosition_modelspace");


    // rendering loop
    while(!glfwWindowShouldClose(window)){

        // input process
        processInput(window);

        // render process
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the depth buffer and the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

        
        // camera view trasnforms 
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // projection transform
        glm::mat4 projection(1.0f);
        projection = glm::perspective(PROJECTION_FOV, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
  
        // *************************************************
        // ************ render the woman model ************
        // *************************************************
        womanShader.use();
        glm::mat4 womanPos = glm::mat4(1.0f);
        womanPos = glm::translate(womanPos, POINT_ZERO);
        // Down scale the woman model
        womanPos = glm::scale(womanPos, glm::vec3(0.1f, 0.1f, 0.1f));

        womanShader.setMat4("projection", projection);
        womanShader.setMat4("view", view);
        womanShader.setMat4("model", womanPos);
        womanModel.Draw(womanShader);

        // ***********************************************
        // ************ render the containers ************
        // ***********************************************
        // containerShader.use();
        // // light has the same position with the woman
        // containerShader.setVec3("light.position", glm::vec3(womanPos * glm::vec4(POINT_ZERO, 1.0)));
        // containerShader.setVec3("viewPos", cameraPos);

        // // light properties
        // containerShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
        // containerShader.setVec3("light.diffuse", 0.3f, 0.3f, 0.3f);
        // containerShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // containerShader.setFloat("light.constant", 1.0f);
        // containerShader.setFloat("light.linear", 0.007f);
        // containerShader.setFloat("light.quadratic", 0.0002f);

        // // material properties
        // containerShader.setFloat("material.shininess", 32.0f);

        // // pass view, projection information
        // containerShader.setMat4("view", view);
        // containerShader.setMat4("projection", projection);

        // // bind the textures
        // glBindTexture(GL_TEXTURE_2D, containerTexture);

        // // transformations
        // glBindVertexArray(containerVAO);
        // for (int container = 0; container < NUM_OF_CONTAINERS; container++){
        //     // calculate the orbit around the woman
        //     glm::mat4 cotainerPos = glm::mat4(1.0f);
        //     // move to the center of the woman
        //     cotainerPos = glm::translate(cotainerPos, glm::vec3(womanPos * glm::vec4(POINT_ZERO, 1.0)));
        //     cotainerPos = glm::rotate(cotainerPos,  containerOrbitSpeeds[container] * getRotationTime() + containerOrbitOffsets[container], containerOrbitAxes[container]);
        //     // move relative to the woman
        //     cotainerPos = glm::translate(cotainerPos, containerOrbits[container] );

        //     // calculate the containers rotation around themself
        //     cotainerPos = glm::rotate(cotainerPos,  containerRotationAxesSpeeds[container] * getRotationTime() + containerRotationAxesOffsets[container], containerRotationAxes[container]);

        //     containerShader.setMat4("model", cotainerPos);

        //     glDrawArrays(GL_TRIANGLES, 0, 36);
        // }

        lightingSphereShader.use();
        lightingSphereShader.setMat4("projection", projection);
        lightingSphereShader.setMat4("view", view);

        glm::mat4 spherePos = glm::mat4(1.0f);
        spherePos = glm::translate(spherePos, glm::vec3(3.0f, 3.0f, 1.0f));
        spherePos = glm::scale(spherePos, glm::vec3(1.0f, 1.0f, 1.0f));

        lightingSphereShader.setMat4("model", spherePos);


        litSphere.Draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // *************************************************
        // ************ render the light source ************
        // *************************************************

        // lightShader.use();
        // lightShader.setMat4("projection", projection);
        // lightShader.setMat4("view", view);
        // // light position = woman position
        // glm::mat4 lightPos = womanPos;
        // lightPos = glm::scale(lightPos, glm::vec3(2.0f, 2.0f, 2.0f));
        // lightShader.setMat4("model", lightPos);

        // glBindVertexArray(lightVAO);
        // glDrawArrays(GL_TRIANGLES, 0, 36);



        // get the frame rate
        currentFrameTime = getActiveTime(); 
        deltaTime = currentFrameTime - prevFrameTime;
        // get the fps
        framesRendered++;
        if ((int)currentFrameTime- prevSec == 1){
            printf("FPS %d\n",framesRendered);
            framesRendered = 0;
        }
        prevSec = (int)getActiveTime();
        prevFrameTime = currentFrameTime;
        prevRotationTime = getRotationTime();


        // check if paused
        processInput(window);
        while(paused){
            // reset the timer
            pausedTime = (float)glfwGetTime();
            lastFrameTime = currentFrameTime;
            prevFrameTime = currentFrameTime;
            // sleep a framerate time
            usleep(PAUSE_SLEEP_TIME);
            // input process
            processInput(window);
            glfwSwapBuffers(window);
            glfwPollEvents(); 
        }
        // check call events
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
  
    // // delete all the variables, shaders, buffers
    // glDeleteVertexArrays(1, &containerVAO);
    // glDeleteVertexArrays(1, &lightVAO);
    // glDeleteBuffers(1, &VBO);

    glfwTerminate();    
    return 0;
}

// rotation time different for the speed up and slow down of the animation
float getRotationTime(){
    return prevRotationTime + (globalRotationSpeed * deltaTime);
}

float getActiveTime(){
    return (float)glfwGetTime() - (pausedTime - lastFrameTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}  


// call backs
void processInput(GLFWwindow *window){   

    // if pressed escape end the rendering or exit pause and end the rendering
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
        paused = false;
    }
    
    // forward and backward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    // up and down
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS )
        cameraPos += cameraSpeed * cameraUpward;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUpward;

    // rotate left and right
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        // change the front of x an z 
        cameraFront = glm::vec3(sin(cameraRotation += cameraRotationSpeed*cameraSpeed), 0.0f, -cos(cameraRotation += cameraRotationSpeed*cameraSpeed)); 
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        // change the front of x an z 
        cameraFront = glm::vec3(sin(cameraRotation -= cameraRotationSpeed*cameraSpeed), 0.0f, -cos(cameraRotation -= cameraRotationSpeed*cameraSpeed)); 

    // left and right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    // pause
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        paused = true;        
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        paused = false;

    // increase speed
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
        globalRotationSpeed += globalRotationSpeedIncrease;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
        globalRotationSpeed -= globalRotationSpeedIncrease;
    }


    // // tilt up and down
    // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
    //     cameraFront = glm::vec3(sin(cameraRotation), sin(cameraTilt += cameraRotationSpeed*cameraSpeed), -cos(cameraTilt += cameraRotationSpeed*cameraSpeed)-cos(cameraRotation)); 
    //     // cameraUp    = glm::vec3(sin(cameraTilt += cameraRotationSpeed*cameraSpeed), cos(cameraTilt += cameraRotationSpeed*cameraSpeed), 0.0f); 
    // }
    // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
    //     cameraFront = glm::vec3(sin(cameraRotation), sin(cameraTilt -= cameraRotationSpeed*cameraSpeed), -cos(cameraTilt -= cameraRotationSpeed*cameraSpeed)-cos(cameraRotation)); 
    //     // cameraUp    = glm::vec3(sin(cameraTilt -= cameraRotationSpeed*cameraSpeed), cos(cameraTilt -= cameraRotationSpeed*cameraSpeed), -cos(cameraRotation)); 
    // }

}
