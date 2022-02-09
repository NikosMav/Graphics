#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

// settings
#define SCR_WIDTH 800
#define SCR_HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);