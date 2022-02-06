#include <Aux.h>

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
glm::vec3 light_pos = ZERO;

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
    Model woman_model(FileSystem::getPath(WOMAN_MODEL_PATH));
    Sphere light_sphere(40, 40);

    // Load shaders
    Shader woman_shader("shaders/woman_shader.vs", "shaders/woman_shader.fs");
    Shader light_shader("shaders/light_shader.vs", "shaders/light_shader.fs");

    // shader configuration
    // --------------------
    woman_shader.use();
    woman_shader.setInt("material.diffuse", 0);
    woman_shader.setInt("material.specular", 1);

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
        woman_shader.use();
        woman_shader.setVec3("light.position", light_pos);
        woman_shader.setVec3("viewPos", camera.Position);

        // light properties
        woman_shader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        woman_shader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        woman_shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
        // material properties
        woman_shader.setFloat("material.shininess", 64.0f);

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        woman_shader.setMat4("projection", projection);
        woman_shader.setMat4("view", view);
        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, ZERO);

        // down scale
        model = glm::scale(model, glm::vec3(0.05f));
        woman_shader.setMat4("model", model);
        // render the woman
        woman_model.Draw(woman_shader);

        // also draw the sphere object
        // SPHERE OBJECT
        temp_angle = glfwGetTime() * global_speed;
        light_pos.x = ORBIT_RADIUS.z * sin(PI * 2.0 * temp_angle / 360);
        light_pos.z = ORBIT_RADIUS.z * cos(PI * 2.0 * temp_angle / 360);

        light_shader.use();
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, ZERO);
        model = glm::scale(model, glm::vec3(0.5f));
        // Sphere position should be equal to light position
        model = glm::translate(model, light_pos);
        light_shader.setMat4("model", model);

        light_sphere.Draw();
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

// Auxiliary Functions
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