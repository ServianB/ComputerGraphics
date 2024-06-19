#define GL_IGNORE_DEPRECATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <cmath>

#include "../SHADER/GLShader.h"
#include "../tiny_obj_loader.h"  // Include without defining TINYOBJLOADER_IMPLEMENTATION

struct vec2 { float x, y; };
struct vec3 { float x, y, z; };

struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

struct Application
{
    GLShader m_basicProgram;
    Mesh m_mesh;
    float m_scaleFactor = 0.1f; // Add this line for scaling

    void Initialize()
    {
        m_basicProgram.LoadVertexShader("../SHADER/basic.vs.glsl");
        m_basicProgram.LoadFragmentShader("../SHADER/basic.fs.glsl");
        m_basicProgram.Create();

        LoadOBJ("chair.obj", m_mesh);
        SetupMesh(m_mesh);
    }

    void Terminate() {
        glDeleteVertexArrays(1, &m_mesh.vao);
        glDeleteBuffers(1, &m_mesh.vbo);
        glDeleteBuffers(1, &m_mesh.ebo);
        m_basicProgram.Destroy();
    }

    void LoadOBJ(const std::string& filename, Mesh& mesh) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str());
        if (!warn.empty()) std::cout << "WARN : " << warn << std::endl;
        if (!err.empty()) std::cerr << "ERR : " << err << std::endl;
        if (!ret) {
            std::cerr << "Failed to load OBJ file : " << filename << std::endl;
            return;
        }

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex = {};
                vertex.position.x = attrib.vertices[3 * index.vertex_index + 0] * m_scaleFactor; // Apply scaling factor
                vertex.position.y = attrib.vertices[3 * index.vertex_index + 1] * m_scaleFactor; // Apply scaling factor
                vertex.position.z = attrib.vertices[3 * index.vertex_index + 2] * m_scaleFactor; // Apply scaling factor

                if (!attrib.normals.empty()) {
                    vertex.normal.x = attrib.normals[3 * index.normal_index + 0];
                    vertex.normal.y = attrib.normals[3 * index.normal_index + 1];
                    vertex.normal.z = attrib.normals[3 * index.normal_index + 2];
                }

                if (!attrib.texcoords.empty()) {
                    vertex.texCoord.x = attrib.texcoords[2 * index.texcoord_index + 0];
                    vertex.texCoord.y = attrib.texcoords[2 * index.texcoord_index + 1];
                }

                mesh.vertices.push_back(vertex);
                mesh.indices.push_back(mesh.indices.size());
            }
        }
    }

    void SetupMesh(Mesh& mesh) {
        glGenVertexArrays(1, &mesh.vao);
        glBindVertexArray(mesh.vao);

        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &mesh.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void Render()
    {
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(m_basicProgram.GetProgram());

        glBindVertexArray(m_mesh.vao);
        glDrawElements(GL_TRIANGLES, m_mesh.indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};

int main(void) {
    int width = 800;
    int height = 600;
    GLFWwindow* window;

    if (!glfwInit()) return -1;

    window = glfwCreateWindow(width, height, "Projet - OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Application app;
    app.Initialize();

    while (!glfwWindowShouldClose(window)) {
        app.Render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    app.Terminate();
    glfwTerminate();
    return 0;
}
