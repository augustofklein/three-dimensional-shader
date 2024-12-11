#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <iostream>
#include <vector>

// Collision map size (42x64, as previously generated)
const int COLLISION_MAP_WIDTH = 50;
const int COLLISION_MAP_HEIGHT = 50;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;

glm::vec3 lightPos(9.0f, 7.2f, 37.3f);

float specularStrength = 0.3;

glm::mat4 view;
glm::mat4 projection;

// Camera settings original
glm::vec3 cameraPos = glm::vec3(12.28f, 34.37f, 0.0f);
glm::vec3 cameraFront = glm::vec3(-0.53f, -0.66f, 0.52f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 1.0f);

float sensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;
unsigned char pixelColor[3];

int numberVertices = 222;

std::vector<float> originalVertices;

struct CarState {
    glm::vec3 position;     // Posição atual do carro
    float angle;            // Ângulo de rotação em graus
    float speed;            // Velocidade atual
    float acceleration;     // Aceleração
    float maxSpeed;         // Velocidade máxima
    float turnSpeed;        // Velocidade de rotação

    CarState() {
        position = glm::vec3(0.0f);
        angle = 0.0f;
        speed = 0.0f;
        acceleration = 0.05f;
        maxSpeed = 0.01f;
        turnSpeed = 0.08f;
    }
};

struct Color {
    unsigned char r, g, b;

    bool isYellow() const {
        return r == 8 && g == 109 && b == 5;
    }

    bool isGreen() const {
        return r == 251 && g == 203 && b == 9;
    }

    bool isObstacle() const {
        return isYellow() || isGreen();
    }
};

CarState carState;

// Dados de vértices para o chão
// Três primeiros são coordenadas, três seguintes são normalizados e dois últimos para textura
float floorVertices[] = {
    -50.0f,  -0.6f, -50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     50.0f,  -0.6f, -50.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
     50.0f,  -0.6f,  50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
     50.0f,  -0.6f,  50.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -50.0f,  -0.6f,  50.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -50.0f,  -0.6f, -50.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
};

float carVertices[] = {
    // Main body (lower part)
    // Front face
    -2.0f,  0.0f, -0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.0f, -0.8f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f, -0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f, -0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.6f, -0.8f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.0f, -0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back face
    -2.0f,  0.0f,  0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.0f,  0.8f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f,  0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f,  0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.6f,  0.8f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.0f,  0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Left side
    -2.0f,  0.0f,  0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.0f, -0.8f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.6f, -0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.6f, -0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.6f,  0.8f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.0f,  0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Right side
     2.0f,  0.0f,  0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.0f, -0.8f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f, -0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f, -0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f,  0.8f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.0f,  0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Top
    -2.0f,  0.6f, -0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f, -0.8f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f,  0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.6f,  0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.6f,  0.8f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.6f, -0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Bottom
    -2.0f,  0.0f, -0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.0f, -0.8f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.0f,  0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     2.0f,  0.0f,  0.8f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.0f,  0.8f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -2.0f,  0.0f, -0.8f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Cabin (upper part)
    // Front windshield
    -0.5f,  0.6f, -0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.8f,  0.6f, -0.7f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f, -0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f, -0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f, -0.6f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.6f, -0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back windshield
    -0.5f,  0.6f,  0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.8f,  0.6f,  0.7f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f,  0.6f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.6f,  0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Roof
    -0.3f,  1.2f, -0.6f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f, -0.6f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f,  0.6f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f, -0.6f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Upper part closure
    // Left side (connect front and back windshields to the roof)
    -0.5f,  0.6f, -0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.6f,  0.7f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f, -0.6f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.6f, -0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Right side (connect front and back windshields to the roof)
     0.8f,  0.6f, -0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.8f,  0.6f,  0.7f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f, -0.6f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.8f,  0.6f, -0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

     // Front edge (connect front windshield to roof)
    -0.5f,  0.6f, -0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.8f,  0.6f, -0.7f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f, -0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f, -0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f, -0.6f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.6f, -0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back edge (connect back windshield to roof)
    -0.5f,  0.6f,  0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.8f,  0.6f,  0.7f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     0.5f,  1.2f,  0.6f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.3f,  1.2f,  0.6f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.6f,  0.7f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Front-left wheel
    // Front face
    -1.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back face
    -1.5f,  0.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Left face
    -1.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Right face
    -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Bottom face
    -1.5f, -0.4f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Top face
    -1.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Front-right wheel
    // Front face
     1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back face
     1.0f,  0.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f, -0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Left face
     1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Right face
     1.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Bottom face
     1.0f, -0.4f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Top face
     1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f, -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f, -0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back-left wheel
    // Front face
    -1.5f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back face
    -1.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Left face
    -1.5f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Right face
    -1.0f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Bottom face
    -1.5f, -0.4f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f, -0.4f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Top face
    -1.5f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -1.5f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back-right wheel
    // Front face
     1.0f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  0.5f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Back face
     1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Left face
     1.0f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Right face
     1.5f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f,  1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Bottom face
     1.0f, -0.4f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f, -0.4f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f, -0.4f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

    // Top face
     1.0f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.5f,  0.0f,  1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
     1.0f,  0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f
};

float lampPostVertices[] = {
    1.4f, 0.0f, 3.2f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
    0.6f, 0.0f, 3.2f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
    0.6f, 0.0f, 2.8f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
    0.6f, 0.0f, 2.8f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
    1.4f, 0.0f, 2.8f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
    1.4f, 0.0f, 3.2f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,

    1.4f, 0.48f, 3.2f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    0.6f, 0.48f, 3.2f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
    0.6f, 0.48f, 2.8f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
    0.6f, 0.48f, 2.8f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
    1.4f, 0.48f, 2.8f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
    1.4f, 0.48f, 3.2f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,

    1.2f, 0.48f, 3.1f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    0.8f, 0.48f, 3.1f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
    0.8f, 7.2f,  3.1f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    0.8f, 7.2f,  3.1f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    1.2f, 7.2f,  3.1f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    1.2f, 0.48f, 3.1f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

    1.4f, 0.0f, 3.2f, -1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    1.4f, 0.48f, 3.2f,  0.0f, 0.0f, -1.0f,  0.0f,  1.0f,
    0.6f, 0.48f, 3.2f,  1.0f, 0.0f, -1.0f,  0.0f,  1.0f,
    0.6f, 0.0f, 3.2f,  1.0f, 0.0f, -1.0f,  0.0f, -1.0f,

    1.4f, 0.0f, 2.8f,  0.0f, 0.0f,  1.0f,  0.0f, -1.0f,
    1.4f, 0.48f, 2.8f,  0.0f, 0.0f,  1.0f,  0.0f,  1.0f,
    0.6f, 0.48f, 2.8f,  1.0f, 0.0f,  1.0f,  0.0f,  1.0f,
    0.6f, 0.0f, 2.8f,  1.0f, 0.0f,  1.0f,  0.0f, -1.0f,

    1.4f, 0.0f, 3.2f,  -1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    1.4f, 0.0f, 2.8f,  -1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
    1.4f, 0.48f, 2.8f,  -1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
    1.4f, 0.48f, 3.2f,  -1.0f, 1.0f,  0.0f, -1.0f,  0.0f,

    0.6f, 0.0f, 3.2f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
    0.6f, 0.0f, 2.8f,  1.0f, 0.0f,  0.0f,  1.0f,   0.0f,
    0.6f, 0.48f, 2.8f,  1.0f, 1.0f,  0.0f,  1.0f,   0.0f,
    0.6f, 0.48f, 3.2f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f
};

float cubeLamp[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.25f,  0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.25f, 0.25f,  0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.25f, 0.25f,  0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.25f, 0.0f,  0.0f, 0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.25f, 0.0f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.25f, 0.25f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.5f, 0.25f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.25f, 0.0f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.5f, 0.25f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  0.5f, 0.0f,  -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.5f, 0.25f,  -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.75f, 0.25f,  -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.5f, 0.0f,  -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.75f, 0.25f,  -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.75f, 0.0f,  -1.0f, 0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  0.25f, 0.25f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.25f, 0.5f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.5f, 0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 0.25f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.5f, 0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.5f, 0.25f,  1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.25f, 0.75f,  0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.5f, 0.75f,  0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.25f, 0.5f,  0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.5f, 0.75f,  0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.5f, 0.5f,  0.0f, -1.0f, 0.0f,

    -0.5f,  0.5f, -0.5f,  0.25f, 0.75f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.25f, 1.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.5f, 1.0f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.25f, 0.75f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.5f, 1.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.5f, 0.75f,  0.0f, 1.0f, 0.0f
};

const std::vector<std::vector<int>> collisionMap = {
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL : Corrida Maluca", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cout << " Ocorreu um erro iniciando GLEW!" << std::endl;
    } else {
        std::cout << "GLEW OK!" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;
    }

    glEnable(GL_DEPTH_TEST);

    Shader floorShader("vertex.glsl", "fragment.glsl");
    Shader carShader("vertex.glsl", "car_shader.glsl");
    Shader lampPostShader("vertex.glsl", "car_shader.glsl");

    GLuint VBOs[3], VAOs[3];
    glGenVertexArrays(3, VAOs);
    glGenBuffers(3, VBOs);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(VAOs[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lampPostVertices), lampPostVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int texture1, texture2;
    unsigned char *data;

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    data = stbi_load("res/images/race_track.jpeg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture of race track" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("res/images/car_texture.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture of car" << std::endl;
    }
    stbi_image_free(data);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);

    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //Shaders utilizados para a iluminação
    Shader lightCubeShader("light_cube.vs", "light_cube.fs");

    floorShader.use();
    floorShader.setInt("texture1", 0);

    carShader.use();
    carShader.setInt("texture2", 0);

    lampPostShader.use();
    lampPostShader.setInt("texture2", 0);

    while (!glfwWindowShouldClose(window)) {

        // Configuração do carro
        glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(carVertices), carVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glm::mat4 model = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        projection = glm::perspective(glm::radians(70.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 220.0f);

        floorShader.use();
        glBindVertexArray(VAOs[0]);
        floorShader.setMat4("projection", projection);
        glUniformMatrix4fv(glGetUniformLocation(floorShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(floorShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        floorShader.setVec3("lightPos", lightPos);
        floorShader.setVec3("viewPos", cameraPos);
        floorShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        floorShader.setVec3("objectColor", glm::vec3(0.83f, 0.68f, 0.21f));
        floorShader.setFloat("specularStrength", specularStrength);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(floorVertices) / (8 * sizeof(float)));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        model = glm::translate(model, glm::vec3(8.0f, 0.0f, 35.0f));

        carShader.use();
        glBindVertexArray(VAOs[1]);
        carShader.setMat4("projection", projection);
        glUniformMatrix4fv(glGetUniformLocation(carShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(carShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        carShader.setVec3("lightPos", lightPos);
        carShader.setVec3("viewPos", cameraPos);
        carShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        carShader.setVec3("objectColor", glm::vec3(0.83f, 0.68f, 0.21f));
        carShader.setFloat("specularStrength", specularStrength);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(carVertices) / (8 * sizeof(float)));

        lampPostShader.use();
        glBindVertexArray(VAOs[2]);
        lampPostShader.setMat4("projection", projection);
        glUniformMatrix4fv(glGetUniformLocation(lampPostShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(lampPostShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

        lampPostShader.setVec3("lightPos", lightPos);
        lampPostShader.setVec3("viewPos", cameraPos);
        lampPostShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        lampPostShader.setVec3("objectColor", glm::vec3(0.83f, 0.68f, 0.21f));
        lampPostShader.setFloat("specularStrength", specularStrength);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(lampPostVertices) / (8 * sizeof(float)));

        // also draw the lamp object
        lightCubeShader.use();
        glBindVertexArray(VAOs[3]);
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.015f)); // a smaller cube
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(cubeLamp) / (8 * sizeof(float)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(4, VAOs);
    glDeleteBuffers(4, VBOs);
    glfwTerminate();
    return 0;
}

void flipCamera(float x, float y)
{
    yaw += x * sensitivity;
    pitch += y * sensitivity;

    if(pitch > 89.0f)
        pitch = 89.0f;

    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

// Function to check if the car is on the track based on the collision map
bool isCarOnTrack(glm::vec3 nextPosition) {
    int carX_pixel = static_cast<int>(nextPosition.x + 7);
    int carY_pixel = static_cast<int>(nextPosition.z + 24);

    //printf("POSITION %d %d\n", nextPosition.x, nextPosition.y);

    //printf("POINTS %d %d %d\n", carX_pixel, carY_pixel, collisionMap[carX_pixel][carY_pixel]);

    //printf("---------------------------\n");

    return collisionMap[carX_pixel][carY_pixel] == 1;
}

void initializeCarVertices() {
    originalVertices.assign(carVertices, carVertices + sizeof(carVertices)/sizeof(float));
}

void updateCarVertices() {
    // Verifica se os vértices originais foram inicializados
    if (originalVertices.empty()) {
        initializeCarVertices();
    }

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, carState.position);
    transform = glm::rotate(transform, glm::radians(carState.angle), glm::vec3(0.0f, 1.0f, 0.0f));

    for (int i = 0; i < numberVertices; i++) {
        glm::vec4 vertex(
            originalVertices[i * 8],     // x
            originalVertices[i * 8 + 1], // y
            originalVertices[i * 8 + 2], // z
            1.0f
        );

        // Aplica a transformação
        glm::vec4 transformed = transform * vertex;

        // Atualiza os vértices
        carVertices[i * 8] = transformed.x;
        carVertices[i * 8 + 1] = transformed.y;
        carVertices[i * 8 + 2] = transformed.z;
        // Mantém as coordenadas de textura inalteradas
        carVertices[i * 8 + 3] = originalVertices[i * 8 + 3];
        carVertices[i * 8 + 4] = originalVertices[i * 8 + 4];
    }
}

void moveCarForward() {
    float deltaSpeed = carState.acceleration;
    float newSpeed = std::max(carState.speed - deltaSpeed, -carState.maxSpeed);

    // Calcula a próxima posição
    float angleRad = glm::radians(carState.angle);
    glm::vec3 nextPosition = carState.position;
    nextPosition.x += newSpeed * cos(angleRad);
    nextPosition.z -= newSpeed * sin(angleRad);

    // Verifica colisão antes de mover
    if (isCarOnTrack(nextPosition)) {
        carState.speed = newSpeed;
        carState.position = nextPosition;
        updateCarVertices();
    } else {
        // Em caso de colisão, para o carro
        carState.speed = 0.0f;
    }
}

void moveCarBackward() {
    float deltaSpeed = carState.acceleration;
    float newSpeed = std::min(carState.speed + deltaSpeed, carState.maxSpeed);

    // Calcula a próxima posição
    float angleRad = glm::radians(carState.angle);
    glm::vec3 nextPosition = carState.position;
    nextPosition.x += newSpeed * cos(angleRad);
    nextPosition.z -= newSpeed * sin(angleRad);

    // Verifica colisão antes de mover
    if (isCarOnTrack(nextPosition)) {
        carState.speed = newSpeed;
        carState.position = nextPosition;
        updateCarVertices();
    } else {
        // Em caso de colisão, para o carro
        carState.speed = 0.0f;
    }
}

void moveCarRight() {
    carState.angle -= carState.turnSpeed * (carState.speed != 0 ? 1.0f : 0.0f);
    updateCarVertices();
}

void moveCarLeft() {
    carState.angle += carState.turnSpeed * (carState.speed != 0 ? 1.0f : 0.0f);
    updateCarVertices();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    const float cameraSpeed = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        flipCamera(0.0f, 1.0f);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        flipCamera(0.0f, -1.0f);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        flipCamera(-1.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        flipCamera(1.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        cameraPos += glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        moveCarForward();

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        moveCarBackward();

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        moveCarLeft();

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        moveCarRight();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
