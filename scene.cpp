#define GL_IGNORE_DEPRECATION

#include <GL/glew.h> // Ce doit être inclus en premier
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

#include "SHADER/GLShader.h"  // Inclusion de votre propre classe GLShader
#include "tiny_obj_loader.h"  // Inclusion de la bibliothèque tinyobjloader pour charger des objets OBJ

#define INTERVAL 15  // Définition d'une constante INTERVAL avec une valeur de 15

std::chrono::high_resolution_clock::time_point startTime;  // Déclaration du point de départ du temps

double POS_X, POS_Y;  // Variables pour stocker la position du curseur

GLfloat light_pos[] = {-10.0f, 10.0f, 100.00f, 1.0f};  // Position de la lumière dans l'espace

float pos_x = 0.0f, pos_y = 0.0f, pos_z = -20.0f;  // Position de la caméra
float angle_x = 30.0f, angle_y = 0.0f;  // Angles de rotation de la caméra

int x_old = 0, y_old = 0;  // Variables pour stocker les positions précédentes du curseur
int current_scroll = 0;  // Variable pour suivre le nombre de scrolls effectués
float zoom_per_scroll = 1.0f;  // Quantité de zoom par scroll

bool is_holding_mouse = false;  // Indicateur pour savoir si le bouton de la souris est enfoncé
bool is_updated = false;  // Indicateur pour suivre si une mise à jour a eu lieu

// Définition de structures pour les vecteurs 2D et 3D, ainsi que pour les Vertex, Triangle, et Material
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

// Structure Mesh pour représenter un objet 3D
struct Mesh {
    std::vector<Vertex> vertices;      // Vecteur de vertices
    std::vector<Triangle> triangles;   // Vecteur de triangles
    std::vector<unsigned int> indices; // Indices des vertices
    std::vector<Material> materials;   // Matériaux
    Material defaultMaterial;          // Matériau par défaut
    GLuint vbo;                        // Vertex Buffer Object
    GLuint ebo;                        // Element Buffer Object
};

// Structure SceneObject pour représenter un objet dans la scène
struct SceneObject {
    Mesh mesh;                      // Mesh de l'objet
    vec3 position;                  // Position de l'objet
    vec3 rotation;                  // Rotation de l'objet
    vec3 scale;                     // Échelle de l'objet
    float translation_speed;        // Vitesse de translation
    float rotation_speed;           // Vitesse de rotation
    float scale_speed;              // Vitesse de changement d'échelle

    SceneObject() : position(0.0f, 0.0f, 0.0f), rotation(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f), translation_speed(0.0f), rotation_speed(0), scale_speed(0) {}
};

// Classe Application pour gérer l'ensemble de l'application OpenGL
struct Application {
    std::vector<SceneObject> objects;  // Vecteur d'objets dans la scène
    GLShader m_basicProgram;           // Objet GLShader pour gérer les shaders

    // Initialisation de l'application
    void Initialize() {
        // Initialisation des états OpenGL
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

        // Chargement du premier objet (Board.obj)
        SceneObject obj1;
        if (LoadObject("OBJ/Board.obj", obj1.mesh)) {
            std::cout << "Loaded OBJ file: Board.obj" << std::endl;
        }
        obj1.scale = vec3(0.1f, 0.1f, 0.1f);
        obj1.position = vec3(10.f, 0.f, 0.f);
        obj1.translation_speed = 0.01f;
        obj1.rotation_speed = 0.05f;
        obj1.scale_speed = 0.00005f;
        objects.push_back(obj1);

        // Chargement du deuxième objet (chair.obj)
        SceneObject obj2;
        obj2.position = vec3(-10.0f, 0.0f, 0.0f);
        if (LoadObject("OBJ/chair.obj", obj2.mesh)) {
            std::cout << "Loaded OBJ file: chair.obj" << std::endl;
        }
        obj2.scale = vec3(1.5f, 1.5f, 1.5f);
        obj2.translation_speed = -0.01f;
        obj2.rotation_speed = -0.05f;
        obj2.scale_speed = -0.00005f;
        objects.push_back(obj2);

        startTime = std::chrono::high_resolution_clock::now();
    }

    // Fonction pour charger un objet à partir d'un fichier OBJ
    bool LoadObject(const std::string& inputfile, Mesh& mesh) {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = ""; // Chemin d'accès aux fichiers de matériaux

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

    // Fonction pour dessiner un Mesh
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

    // Fonction pour terminer l'application
    void Terminate() {
        m_basicProgram.Destroy(); // Destruction du programme shader
    }

    // Fonction pour rendre la scène
    void Render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Effacement du buffer

        glLoadIdentity(); // Réinitialisation de la matrice de modèle-vue

        // Translation et rotation de la caméra
        glTranslatef(pos_x, pos_y, pos_z);
        glRotatef(angle_x, 1.0f, 0.0f, 0.0f);
        glRotatef(angle_y, 0.0f, 1.0f, 0.0f);

        // Calcul du temps écoulé depuis le début de l'application
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsedTime = currentTime - startTime;
        float time = elapsedTime.count();

        // Boucle sur tous les objets dans la scène pour les dessiner
        for (auto& obj : objects) {
            // Modification de la position, de la rotation et de l'échelle de l'objet au fil du temps
            // Translation
            obj.position.y += obj.translation_speed * std::sin(time);
            // Rotation
            obj.rotation.y += obj.rotation_speed;
            // Échelle
            obj.scale.x += obj.scale_speed * std::sin(time);
            obj.scale.y += obj.scale_speed * std::sin(time);
            obj.scale.z += obj.scale_speed * std::sin(time);

            glPushMatrix(); // Sauvegarde de la matrice courante

            // Translation de l'objet
            glTranslatef(obj.position.x, obj.position.y, obj.position.z);
            // Rotation de l'objet
            glRotatef(obj.rotation.x, 1.0f, 0.0f, 0.0f);
            glRotatef(obj.rotation.y, 0.0f, 1.0f, 0.0f);
            glRotatef(obj.rotation.z, 0.0f, 0.0f, 1.0f);
            // Échelle de l'objet
            glScalef(obj.scale.x, obj.scale.y, obj.scale.z);

            // Dessin du Mesh de l'objet
            drawMesh(obj.mesh);

            glPopMatrix(); // Restauration de la matrice précédente
        }
    }

};

// Fonction de rappel pour la gestion des clics de souris
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

// Fonction de rappel pour la gestion du scrolling de la souris
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0 && current_scroll > 0) {
        current_scroll--;
        pos_z += zoom_per_scroll;
    } else if (yoffset < 0 && current_scroll < 50) {
        current_scroll++;
        pos_z -= zoom_per_scroll;
    }
}

// Fonction de rappel pour la gestion de la position du curseur
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

// Fonction principale
int main(void) {
    int width = 1200;
    int height = 800;
    GLFWwindow* window;

    // Initialisation de GLFW
    if (!glfwInit()) return -1;

    // Création de la fenêtre GLFW
    window = glfwCreateWindow(width, height, "Projet - OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Définition de la fenêtre comme contexte courant
    glfwMakeContextCurrent(window);

    glEnable(GL_DEPTH_TEST); // Activation du test de profondeur pour le rendu correct

    Application app; // Création de l'objet Application
    glfwSetWindowUserPointer(window, &app); // Liaison de l'objet app à la fenêtre GLFW
    app.Initialize(); // Initialisation de l'application OpenGL

    // Définition des rappels GLFW
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Boucle principale de rendu
    while (!glfwWindowShouldClose(window)) {
        app.Render(); // Rendu de la scène
        glfwSwapBuffers(window); // Échange des buffers avant de rendre la prochaine image
        glfwPollEvents(); // Vérification des événements
    }

    app.Terminate(); // Fermeture de l'application (désallocation des ressources, etc.)
    glfwTerminate(); // Fermeture de GLFW

    return 0;
}

// Commande de compilation: g++ -o scene scene.cpp SHADER/GLShader.cpp tiny_obj_loader.cc -lGL -lGLEW -lglfw -lGLU -lm -ldl
             
