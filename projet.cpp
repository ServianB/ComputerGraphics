#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h> 
//#include "./SHADER/GLShader.h"
//#include "./tiny_obj_loader.h"  


// CAMERA VARIABLES --------
float R = 5.0f; // Initial radius
float phi = 0.0f; // Initial azimuth
float theta = 0.0f; // Initial elevation

float cameraX, cameraY, cameraZ;

// Mouse callback function
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = 400;
    static double lastY = 300;
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.005f;
    phi += xoffset * sensitivity;
    theta += yoffset * sensitivity;

    if (theta > M_PI / 2) theta = M_PI / 2;
    if (theta < -M_PI / 2) theta = -M_PI / 2;
}

// Scroll callback function
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    R += yoffset * 0.1f;
    if (R < 1.0f) R = 1.0f;
}

// Initialize GLFW and window
GLFWwindow* Initialize() {
    if (!glfwInit()) {
        return NULL;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Orbital Camera", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glViewport(0, 0, 800, 600);

    return window;
}

// Render one frame
void Render(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update camera position
    cameraY = R * sin(theta);
    cameraX = R * cos(theta) * cos(phi);
    cameraZ = R * cos(theta) * sin(phi);

    // Print camera coordinates to console
    printf("Camera Position -> X: %f, Y: %f, Z: %f\n", cameraX, cameraY, cameraZ);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraX, cameraY, cameraZ, 0, 0, 0, 0, 1, 0);

    glfwSwapBuffers(window);
}

// Cleanup and terminate GLFW
void Terminate(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
    GLFWwindow* window = Initialize();
    if (!window) return -1;

    while (!glfwWindowShouldClose(window)) {
        Render(window);
        glfwPollEvents();
    }

    Terminate(window);
    return 0;
}
