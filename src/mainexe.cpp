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
#include <math.h>
#include <iostream>

#define WOMAN_MODEL_PATH "resources/woman/woman1.obj"
#define ZERO glm::vec3(0.0f, 0.0f, 0.0f)
// Sphere's orbit circle radius
#define ORBIT_RADIUS glm::vec3(0.0f, 0.0f, 15.0f)

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

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
// initialization with (0,0,0)
glm::vec3 lightPos = ZERO;

// speed
float global_speed = 1.0f;
float speed_increase = 0.05f;

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

    // Load shaders
    Shader womanShader("shaders/woman_shader.vs", "shaders/woman_shader.fs");
    Shader lightingSphereShader("shaders/light_shader.vs", "shaders/light_shader.fs");

    // shader configuration
    // --------------------
    womanShader.use();
    womanShader.setInt("material.diffuse", 0);
    womanShader.setInt("material.specular", 1);

    float temp_angle = 0;
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
        // WOMAN OBJECT
        womanShader.use();
        womanShader.setVec3("light.position", lightPos);
        womanShader.setVec3("viewPos", camera.Position);

        // light properties
        womanShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        womanShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        womanShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // material properties
        womanShader.setFloat("material.shininess", 64.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        womanShader.setMat4("projection", projection);
        womanShader.setMat4("view", view);
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, ZERO);

        // down scale
        model = glm::scale(model, glm::vec3(0.05f));
        womanShader.setMat4("model", model);
        // render the woman
        womanModel.Draw(womanShader);

        // also draw the lamp object
        // SPHERE OBJECT
        temp_angle = glfwGetTime() * global_speed;
        lightPos.x = ORBIT_RADIUS.z * sin(PI * 2.0 * temp_angle / 360);
        lightPos.z = ORBIT_RADIUS.z * cos(PI * 2.0 * temp_angle / 360);

        lightingSphereShader.use();
        lightingSphereShader.setMat4("projection", projection);
        lightingSphereShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, ZERO);
        model = glm::scale(model, glm::vec3(0.5f));
        // Sphere position should be equal to light position
        model = glm::translate(model, lightPos);
        lightingSphereShader.setMat4("model", model);

        litSphere.Draw();
        //set it back to its default using
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
        global_speed += speed_increase;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
        global_speed -= speed_increase;
        if(global_speed < 1){
            global_speed = 1;
        }
    }
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