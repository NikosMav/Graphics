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
#include <camera.h>

#include <iostream>

#define WOMAN_MODEL_PATH "resources/woman/woman1.obj"
#define POINT_ZERO glm::vec3(0.0f, 0.0f, 0.0f)


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    Model womanModel(FileSystem::getPath(WOMAN_MODEL_PATH));
    Sphere litSphere(40, 40);

    Shader womanShader("shaders/woman_shader.vs", "shaders/woman_shader.fs");
    Shader lightingSphereShader("shaders/light_shader.vs", "shaders/light_shader.fs");


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {

        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        womanShader.use();
        // glm::mat4 womanPos = glm::mat4(1.0f);
        // womanPos = glm::translate(womanPos, POINT_ZERO);
        // // Down scale the woman model
        // womanPos = glm::scale(womanPos, glm::vec3(0.01f));

        womanShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
        womanShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
        womanShader.setVec3("lightPos", lightPos);
        womanShader.setVec3("viewPos", camera.Position);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        womanShader.setMat4("projection", projection);
        womanShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, POINT_ZERO);
        // down scale
        model = glm::scale(model, glm::vec3(0.05f));

        womanShader.setMat4("model", model);

        // render the woman
        womanModel.Draw(womanShader);


        // also draw the lamp object
        lightingSphereShader.use();
        lightingSphereShader.setMat4("projection", projection);
        lightingSphereShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightingSphereShader.setMat4("model", model);

        //lightingSphereShader.setMat4("model", spherePos);

        litSphere.Draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // // optional: de-allocate all resources once they've outlived their purpose:
    // // ------------------------------------------------------------------------
    // glDeleteVertexArrays(1, &cubeVAO);
    // glDeleteVertexArrays(1, &lightCubeVAO);
    // glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// #define TEXTURES_PATH_CONTAINER "resources/textures/container.png"
// #define WOMAN_MODEL_PATH "resources/woman/woman1.obj"

// #define WINDOW_WIDTH 1024
// #define WINDOW_HEIGHT 600

// #define PROJECTION_FOV glm::radians(45.0f)
// #define WOMAN_ORBITAL_RADIUS glm::vec3(0.0f, 0.0f,  5.0f)
// #define POINT_ZERO glm::vec3(0.0f, 0.0f, 0.0f)

// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// void processInput(GLFWwindow *window);
// float getActiveTime(); 
// float getRotationTime();

// // global orbital speed
// float globalRotationSpeed = 1.0f;
// float globalRotationSpeedIncrease = 0.02f;

// // camera
// const float cameraSpeed = 0.1f; 
// const float cameraRotationSpeed = 0.02f; 

// // camera
// glm::vec3 cameraPos   = glm::vec3(0.0f, 5.0f, 40.0f);
// glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
// glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
// glm::vec3 cameraUpward = glm::vec3(0.0f, 1.0f, 0.0f);
// float cameraRotation = 0.0;
// float cameraTilt = 0.0;

// // time per frame
// float deltaTime = 0.0f;
// float lastFrameTime = 0.0f; 
// float prevFrameTime = 0.0f; 
// float prevRotationTime = 0.0f; 
// float currentFrameTime = 0.0f;
// float fps = 0.0f;
// int framesRendered = 0.0f;
// int prevSec = 0;

// // pause and timers
// bool paused = false;
// float pausedTime = 0.0f;
// #define PAUSE_SLEEP_TIME 100000

// // lighting
// glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// int main(){
//     // initialize opengl core proifile 
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     // make a window
//     GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Main_window", NULL, NULL);
//     if (window == NULL)    {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);


//     // initialize glad to handle pointers
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))    {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         return -1;
//     }    

//     // adjust the view port size
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 

//     // enable global depth
//     glEnable(GL_DEPTH_TEST) ;

//     Model womanModel(FileSystem::getPath(WOMAN_MODEL_PATH));
//     Sphere litSphere(4, 4);

//     Shader womanShader("shaders/woman_shader.vs", "shaders/woman_shader.fs");
//     Shader lightingSphereShader("shaders/light_shader.vs", "shaders/light_shader.fs");

//     // rendering loop
//     while(!glfwWindowShouldClose(window)){

//         // input process
//         processInput(window);

//         // render process
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the depth buffer and the color buffer
//         glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
//         //glClear(GL_COLOR_BUFFER_BIT);

        
//         // camera view trasnforms 
//         glm::mat4 view = glm::mat4(1.0f);
//         view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

//         // projection transform
//         glm::mat4 projection(1.0f);
//         projection = glm::perspective(PROJECTION_FOV, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
  
//         // *************************************************
//         // ************ render the woman model ************
//         // *************************************************
//         womanShader.use();
//         glm::mat4 womanPos = glm::mat4(1.0f);
//         womanPos = glm::translate(womanPos, POINT_ZERO);
//         // Down scale the woman model
//         womanPos = glm::scale(womanPos, glm::vec3(0.1f, 0.1f, 0.1f));

//         womanShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
//         womanShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
//         womanShader.setVec3("lightPos", lightPos);

//         // view/projection transformations
//         //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//         glm::mat4 view = camera.GetViewMatrix();
//         womanShader.setMat4("projection", projection);
//         womanShader.setMat4("view", view);


//         womanShader.setMat4("projection", projection);
//         womanShader.setMat4("view", view);
//         womanShader.setMat4("model", womanPos);

//         womanShader.setMat4("lightColor", 1.0f, 1.0f, 1.0f);
//         womanShader.setMat4("lightPos", lightPos);

//         // world transformation
//         glm::mat4 model = glm::mat4(1.0f);
//         womanShader.setMat4("model", model);

//         womanModel.Draw(womanShader);

//         // ***********************************************
//         // ************ render the containers ************
//         // ***********************************************
//         // containerShader.use();
//         // // light has the same position with the woman
//         // containerShader.setVec3("light.position", glm::vec3(womanPos * glm::vec4(POINT_ZERO, 1.0)));
//         // containerShader.setVec3("viewPos", cameraPos);

//         // // light properties
//         // containerShader.setVec3("light.ambient", 0.3f, 0.3f, 0.3f);
//         // containerShader.setVec3("light.diffuse", 0.3f, 0.3f, 0.3f);
//         // containerShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
//         // containerShader.setFloat("light.constant", 1.0f);
//         // containerShader.setFloat("light.linear", 0.007f);
//         // containerShader.setFloat("light.quadratic", 0.0002f);

//         // // material properties
//         // containerShader.setFloat("material.shininess", 32.0f);

//         // // pass view, projection information
//         // containerShader.setMat4("view", view);
//         // containerShader.setMat4("projection", projection);

//         // // bind the textures
//         // glBindTexture(GL_TEXTURE_2D, containerTexture);

//         // // transformations
//         // glBindVertexArray(containerVAO);
//         // for (int container = 0; container < NUM_OF_CONTAINERS; container++){
//         //     // calculate the orbit around the woman
//         //     glm::mat4 cotainerPos = glm::mat4(1.0f);
//         //     // move to the center of the woman
//         //     cotainerPos = glm::translate(cotainerPos, glm::vec3(womanPos * glm::vec4(POINT_ZERO, 1.0)));
//         //     cotainerPos = glm::rotate(cotainerPos,  containerOrbitSpeeds[container] * getRotationTime() + containerOrbitOffsets[container], containerOrbitAxes[container]);
//         //     // move relative to the woman
//         //     cotainerPos = glm::translate(cotainerPos, containerOrbits[container] );

//         //     // calculate the containers rotation around themself
//         //     cotainerPos = glm::rotate(cotainerPos,  containerRotationAxesSpeeds[container] * getRotationTime() + containerRotationAxesOffsets[container], containerRotationAxes[container]);

//         //     containerShader.setMat4("model", cotainerPos);

//         //     glDrawArrays(GL_TRIANGLES, 0, 36);
//         // }

//         lightingSphereShader.use();
//         lightingSphereShader.setMat4("projection", projection);
//         lightingSphereShader.setMat4("view", view);

//         glm::mat4 spherePos = glm::mat4(1.0f);
//         spherePos = glm::translate(spherePos, glm::vec3(3.0f, 3.0f, 1.0f));
//         spherePos = glm::scale(spherePos, glm::vec3(1.0f, 1.0f, 1.0f));

//         lightingSphereShader.setMat4("model", spherePos);


//         litSphere.Draw();
//         glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//         // *************************************************
//         // ************ render the light source ************
//         // *************************************************

//         // lightShader.use();
//         // lightShader.setMat4("projection", projection);
//         // lightShader.setMat4("view", view);
//         // // light position = woman position
//         // glm::mat4 lightPos = womanPos;
//         // lightPos = glm::scale(lightPos, glm::vec3(2.0f, 2.0f, 2.0f));
//         // lightShader.setMat4("model", lightPos);

//         // glBindVertexArray(lightVAO);
//         // glDrawArrays(GL_TRIANGLES, 0, 36);



//         // get the frame rate
//         currentFrameTime = getActiveTime(); 
//         deltaTime = currentFrameTime - prevFrameTime;
//         // get the fps
//         framesRendered++;
//         if ((int)currentFrameTime- prevSec == 1){
//             printf("FPS %d\n",framesRendered);
//             framesRendered = 0;
//         }
//         prevSec = (int)getActiveTime();
//         prevFrameTime = currentFrameTime;
//         prevRotationTime = getRotationTime();


//         // check if paused
//         processInput(window);
//         while(paused){
//             // reset the timer
//             pausedTime = (float)glfwGetTime();
//             lastFrameTime = currentFrameTime;
//             prevFrameTime = currentFrameTime;
//             // sleep a framerate time
//             usleep(PAUSE_SLEEP_TIME);
//             // input process
//             processInput(window);
//             glfwSwapBuffers(window);
//             glfwPollEvents(); 
//         }
//         // check call events
//         glfwSwapBuffers(window);
//         glfwPollEvents();    
//     }
  
//     // // delete all the variables, shaders, buffers
//     // glDeleteVertexArrays(1, &containerVAO);
//     // glDeleteVertexArrays(1, &lightVAO);
//     // glDeleteBuffers(1, &VBO);

//     glfwTerminate();    
//     return 0;
// }

// // rotation time different for the speed up and slow down of the animation
// float getRotationTime(){
//     return prevRotationTime + (globalRotationSpeed * deltaTime);
// }

// float getActiveTime(){
//     return (float)glfwGetTime() - (pausedTime - lastFrameTime);
// }

// void framebuffer_size_callback(GLFWwindow* window, int width, int height){
//     glViewport(0, 0, width, height);
// }  


// // call backs
// void processInput(GLFWwindow *window){   

//     // if pressed escape end the rendering or exit pause and end the rendering
//     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
//         glfwSetWindowShouldClose(window, true);
//         paused = false;
//     }
    
//     // forward and backward
//     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//         cameraPos += cameraSpeed * cameraFront;
//     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//         cameraPos -= cameraSpeed * cameraFront;

//     // up and down
//     if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS )
//         cameraPos += cameraSpeed * cameraUpward;
//     if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
//         cameraPos -= cameraSpeed * cameraUpward;

//     // rotate left and right
//     if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
//         // change the front of x an z 
//         cameraFront = glm::vec3(sin(cameraRotation += cameraRotationSpeed*cameraSpeed), 0.0f, -cos(cameraRotation += cameraRotationSpeed*cameraSpeed)); 
//     if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
//         // change the front of x an z 
//         cameraFront = glm::vec3(sin(cameraRotation -= cameraRotationSpeed*cameraSpeed), 0.0f, -cos(cameraRotation -= cameraRotationSpeed*cameraSpeed)); 

//     // left and right
//     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//         cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
//     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//         cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

//     // pause
//     if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
//         paused = true;        
//     if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
//         paused = false;

//     // increase speed
//     if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
//         globalRotationSpeed += globalRotationSpeedIncrease;
//     }
//     if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
//         globalRotationSpeed -= globalRotationSpeedIncrease;
//     }


//     // // tilt up and down
//     // if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
//     //     cameraFront = glm::vec3(sin(cameraRotation), sin(cameraTilt += cameraRotationSpeed*cameraSpeed), -cos(cameraTilt += cameraRotationSpeed*cameraSpeed)-cos(cameraRotation)); 
//     //     // cameraUp    = glm::vec3(sin(cameraTilt += cameraRotationSpeed*cameraSpeed), cos(cameraTilt += cameraRotationSpeed*cameraSpeed), 0.0f); 
//     // }
//     // if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
//     //     cameraFront = glm::vec3(sin(cameraRotation), sin(cameraTilt -= cameraRotationSpeed*cameraSpeed), -cos(cameraTilt -= cameraRotationSpeed*cameraSpeed)-cos(cameraRotation)); 
//     //     // cameraUp    = glm::vec3(sin(cameraTilt -= cameraRotationSpeed*cameraSpeed), cos(cameraTilt -= cameraRotationSpeed*cameraSpeed), -cos(cameraRotation)); 
//     // }

// }





