#define GL_IGNORE_DEPRECATION

#include <GL/glew.h> // This must be first
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>

//#include "SHADER/GLShader.h"
#include "tiny_obj_loader.h"

#define INTERVAL 15

double POS_X, POS_Y;

GLfloat light_pos[] = {-10.0f, 10.0f, 100.00f, 1.0f};

float pos_x = 0.0f, pos_y = 0.0f, pos_z = -20.0f;
float angle_x = 30.0f, angle_y = 0.0f;

int x_old = 0, y_old = 0;
int current_scroll = 0;
float zoom_per_scroll = 1.0f;

bool is_holding_mouse = false;
bool is_updated = false;

struct vec2 {
    float x, y;
    vec2() : x(0), y(0) {}
    vec2(float x, float y) : x(x), y(y) {}
};

struct vec3 {
    float x, y, z;
    vec3() : x(0), y(0), z(0) {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
};

struct Triangle {
    Vertex vertices[3];
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::vector<unsigned int> indices;
    std::vector<Material> materials;
    Material defaultMaterial;
    GLuint vbo;
    GLuint ebo;
};

struct Application {
    Mesh m_mesh;

    void Initialize() {
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, 1.0, 0.1, 2000.0);
        glMatrixMode(GL_MODELVIEW);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_LINE_SMOOTH);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);

        if (LoadObject("OBJ/Board.obj", m_mesh)) {
            std::cout << "Loaded OBJ file" << std::endl;
        }
    }

    bool LoadObject(const std::string& inputfile, Mesh& mesh) {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = ""; // Path to material files

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(inputfile, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error();
            }
            return false;
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning();
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();
        auto& materials = reader.GetMaterials();

        std::vector<vec3> vertices;
        for (size_t i = 0; i < attrib.vertices.size(); i += 3) {
            vertices.emplace_back(
                attrib.vertices[i],
                attrib.vertices[i + 1],
                attrib.vertices[i + 2]);
        }

        std::vector<vec3> normals;
        for (size_t i = 0; i < attrib.normals.size(); i += 3) {
            normals.emplace_back(
                attrib.normals[i],
                attrib.normals[i + 1],
                attrib.normals[i + 2]);
        }

        std::vector<vec2> texcoords;
        for (size_t i = 0; i < attrib.texcoords.size(); i += 2) {
            texcoords.emplace_back(
                attrib.texcoords[i],
                attrib.texcoords[i + 1]);
        }

        for (const auto& shape : shapes) {
            const auto& indices = shape.mesh.indices;
            for (size_t i = 0; i < indices.size(); i += 3) {
                Triangle triangle;
                for (int j = 0; j < 3; ++j) {
                    int vertex_index = indices[i + j].vertex_index;
                    int normal_index = indices[i + j].normal_index;
                    int texcoord_index = indices[i + j].texcoord_index;

                    triangle.vertices[j].position = vertices[vertex_index];
                    if (normal_index >= 0) {
                        triangle.vertices[j].normal = normals[normal_index];
                    }
                    if (texcoord_index >= 0) {
                        triangle.vertices[j].texCoord = texcoords[texcoord_index];
                    }
                }
                mesh.triangles.push_back(triangle);
            }
        }

        return true;
    }

    void drawMesh(const Mesh& mesh) {
        glBegin(GL_TRIANGLES);
        for (const auto& triangle : mesh.triangles) {
            for (const auto& vertex : triangle.vertices) {
                glNormal3f(vertex.normal.x, vertex.normal.y, vertex.normal.z);
                glTexCoord2f(vertex.texCoord.x, vertex.texCoord.y);
                glVertex3f(vertex.position.x, vertex.position.y, vertex.position.z);
            }
        }
        glEnd();
    }

    void Terminate() {
        // Cleanup if necessary
    }

    void display() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(pos_x, pos_y, pos_z);
        glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
        glRotatef(angle_y, 0.0f, 1.0f, 0.0f);

        drawMesh(m_mesh);
    }

    void Render() {
        display();
    }
};

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &POS_X, &POS_Y);
        x_old = POS_X;
        y_old = POS_Y;
        is_holding_mouse = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        is_holding_mouse = false;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0 && current_scroll > 0) {
        current_scroll--;
        pos_z += zoom_per_scroll;
    } else if (yoffset < 0 && current_scroll < 50) {
        current_scroll++;
        pos_z -= zoom_per_scroll;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (is_holding_mouse) {
        angle_y += (xpos - x_old);
        x_old = xpos;
        if (angle_y > 360.0f)
            angle_y -= 360.0f;
        else if (angle_y < 0.0f)
            angle_y += 360.0f;

        angle_x += (ypos - y_old);
        y_old = ypos;
        if (angle_x > 90.0f)
            angle_x = 90.0f;
        else if (angle_x < -90.0f)
            angle_x = -90.0f;
    }
}

int main(void) {
    int width = 1200;
    int height = 800;
    GLFWwindow* window;

    if (!glfwInit()) return -1;

    window = glfwCreateWindow(width, height, "Projet - OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glEnable(GL_DEPTH_TEST); // Enable depth testing for correct rendering

    Application app;
    glfwSetWindowUserPointer(window, &app);
    app.Initialize();

    // Set GLFW callbacks
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    while (!glfwWindowShouldClose(window)) {
        app.Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    app.Terminate();
    glfwTerminate();
    return 0;
}

// Compilation:
// g++ -o main main.cpp common/GLShader.cpp -framework OpenGL -lglfw
// Exécution:
// ./main