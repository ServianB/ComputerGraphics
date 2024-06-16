#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <math.h>
#include <stdio.h>
#include <vector>

//#include "tiny_obj_loader.h"
#include "./SHADER/GLShader.h"

// CAMERA VARIABLES --------
float R = 5.0f; // Initial radius
float phi = 0.0f; // Initial azimuth
float theta = 0.0f; // Initial elevation

float cameraX, cameraY, cameraZ;

std::vector<float> vertices;
std::vector<unsigned int> indices;

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


struct Application
{
    GLShader m_basicProgram;

    void Initialize()
    {
        m_basicProgram.LoadVertexShader("./SHADER/basic.vs.glsl");
        m_basicProgram.LoadFragmentShader("./SHADER/basic.fs.glsl");
        m_basicProgram.Create();
    }

    // Render one frame
    void Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        uint32_t program = m_basicProgram.GetProgram();
        glUseProgram(program);

        // Update camera position
        cameraY = R * sin(theta);
        cameraX = R * cos(theta) * cos(phi);
        cameraZ = R * cos(theta) * sin(phi);

        // Print camera coordinates to console
        printf("Camera Position -> X: %f, Y: %f, Z: %f\n", cameraX, cameraY, cameraZ);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(cameraX, cameraY, cameraZ, 0, 0, 0, 0, 1, 0);

    }

    // Cleanup and terminate GLFW
    void Terminate() {
        m_basicProgram.Destroy();
    }
};



int main() {

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "projet", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // ICI !
    GLenum error = glewInit();
    if (error != GLEW_OK) {
        printf("Erreur d'initialisation de GLEW \n");
    }

    Application app;
    app.Initialize();

    // Controle de la caméra
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //glViewport(0, 0, 800, 600);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {        
        /* Render here */
        app.Render();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    app.Terminate();
    glfwTerminate();
    return 0;
}
