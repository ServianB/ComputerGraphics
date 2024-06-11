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

//input handling

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static double lastX = 400;  // Window width / 2
    static double lastY = 300;  // Window height / 2

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.005f;
    phi += xoffset * sensitivity;
    theta += yoffset * sensitivity;

    // Clamping values
    if (theta > M_PI / 2) theta = M_PI / 2;
    if (theta < -M_PI / 2) theta = -M_PI / 2;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    R += yoffset * 0.1f;
    if (R < 1.0f) R = 1.0f; // Prevent the camera from going too close
}


int main() {
    if (!glfwInit()) {
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "Orbital Camera", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);  // Set mouse callback
    glfwSetScrollCallback(window, scroll_callback);    // Set scroll callback

    glViewport(0, 0, 800, 600); // Set the viewport to the window size

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        
        //Camera position update
        cameraY = R * sin(theta);
        cameraX = R * cos(theta) * cos(phi);
        cameraZ = R * cos(theta) * sin(phi);

        // Print camera coordinates to console
        printf("Camera Position -> X: %f, Y: %f, Z: %f\n", cameraX, cameraY, cameraZ);


        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cameraX, cameraY, cameraZ, 0, 0, 0, 0, 1, 0); // Assuming Y is up

        
        // Render scene

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
