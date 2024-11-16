#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <iostream>
#include <vector>

/*
    TODOS: ROTACIONAR O CARRO EM TODAS AS DIRECOES;
           APLICAR SHADER NAS FACES DO CARRO;
           VERIFICAR O PIXEL QUE O CARRO ESTA PASSANDO;
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;

// Camera settings original
glm::vec3 cameraPos = glm::vec3(12.28f, 34.37f, 0.0f);
glm::vec3 cameraFront = glm::vec3(-0.53f, -0.66f, 0.52f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 1.0f);

// Posição para desenhar o carro
//glm::vec3 cameraPos = glm::vec3(13.80f, 3.80f, 60.67f);
//glm::vec3 cameraFront = glm::vec3(0.08f, -0.36f, 0.92f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 1.0f);

float sensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;
unsigned char pixelColor[3];

float movingSpeed = 0.05f;
int numberVertices = 60;

float carAngle = 0.0f;

std::vector<float> originalVertices;

// Estrutura para manter o estado do carro
struct CarState {
    glm::vec3 position;     // Posição atual do carro
    float angle;            // Ângulo de rotação em graus
    float speed;            // Velocidade atual
    float acceleration;     // Aceleração
    float maxSpeed;         // Velocidade máxima
    float turnSpeed;        // Velocidade de rotação
    float friction;         // Fator de fricção

    CarState() {
        position = glm::vec3(0.0f);
        angle = 0.0f;
        speed = 0.0f;
        acceleration = 0.02f;
        maxSpeed = 0.01f;
        turnSpeed = 0.08f;
        friction = 0.98f;
    }
};

CarState carState;

// Dados de vértices para o chão
float vertices[] = {
    -50.0f,  -0.6f, -50.0f,  0.0f, 1.0f,
     50.0f,  -0.6f, -50.0f,  1.0f, 1.0f,
     50.0f,  -0.6f,  50.0f,  1.0f, 0.0f,
     50.0f,  -0.6f,  50.0f,  1.0f, 0.0f,
    -50.0f,  -0.6f,  50.0f,  0.0f, 0.0f,
    -50.0f,  -0.6f, -50.0f,  0.0f, 1.0f
};

float carVertices[] = {
    // Main body (lower part)
    // Front face
    -2.0f, 0.0f, -0.8f,   0.0f, 0.0f,
     2.0f, 0.0f, -0.8f,   1.0f, 0.0f,
     2.0f, 0.6f, -0.8f,   1.0f, 1.0f,
     2.0f, 0.6f, -0.8f,   1.0f, 1.0f,
    -2.0f, 0.6f, -0.8f,   0.0f, 1.0f,
    -2.0f, 0.0f, -0.8f,   0.0f, 0.0f,

    // Back face
    -2.0f, 0.0f,  0.8f,   0.0f, 0.0f,
     2.0f, 0.0f,  0.8f,   1.0f, 0.0f,
     2.0f, 0.6f,  0.8f,   1.0f, 1.0f,
     2.0f, 0.6f,  0.8f,   1.0f, 1.0f,
    -2.0f, 0.6f,  0.8f,   0.0f, 1.0f,
    -2.0f, 0.0f,  0.8f,   0.0f, 0.0f,

    // Left side
    -2.0f, 0.0f,  0.8f,   0.0f, 0.0f,
    -2.0f, 0.0f, -0.8f,   1.0f, 0.0f,
    -2.0f, 0.6f, -0.8f,   1.0f, 1.0f,
    -2.0f, 0.6f, -0.8f,   1.0f, 1.0f,
    -2.0f, 0.6f,  0.8f,   0.0f, 1.0f,
    -2.0f, 0.0f,  0.8f,   0.0f, 0.0f,

    // Right side
     2.0f, 0.0f,  0.8f,   0.0f, 0.0f,
     2.0f, 0.0f, -0.8f,   1.0f, 0.0f,
     2.0f, 0.6f, -0.8f,   1.0f, 1.0f,
     2.0f, 0.6f, -0.8f,   1.0f, 1.0f,
     2.0f, 0.6f,  0.8f,   0.0f, 1.0f,
     2.0f, 0.0f,  0.8f,   0.0f, 0.0f,

    // Top
    -2.0f, 0.6f, -0.8f,   0.0f, 0.0f,
     2.0f, 0.6f, -0.8f,   1.0f, 0.0f,
     2.0f, 0.6f,  0.8f,   1.0f, 1.0f,
     2.0f, 0.6f,  0.8f,   1.0f, 1.0f,
    -2.0f, 0.6f,  0.8f,   0.0f, 1.0f,
    -2.0f, 0.6f, -0.8f,   0.0f, 0.0f,

    // Bottom
    -2.0f, 0.0f, -0.8f,   0.0f, 0.0f,
     2.0f, 0.0f, -0.8f,   1.0f, 0.0f,
     2.0f, 0.0f,  0.8f,   1.0f, 1.0f,
     2.0f, 0.0f,  0.8f,   1.0f, 1.0f,
    -2.0f, 0.0f,  0.8f,   0.0f, 1.0f,
    -2.0f, 0.0f, -0.8f,   0.0f, 0.0f,

    // Cabin (upper part)
    // Front windshield
    -0.5f, 0.6f, -0.7f,   0.0f, 0.0f,
     0.8f, 0.6f, -0.7f,   1.0f, 0.0f,
     0.5f, 1.2f, -0.6f,   1.0f, 1.0f,
     0.5f, 1.2f, -0.6f,   1.0f, 1.0f,
    -0.3f, 1.2f, -0.6f,   0.0f, 1.0f,
    -0.5f, 0.6f, -0.7f,   0.0f, 0.0f,

    // Back windshield
    -0.5f, 0.6f,  0.7f,   0.0f, 0.0f,
     0.8f, 0.6f,  0.7f,   1.0f, 0.0f,
     0.5f, 1.2f,  0.6f,   1.0f, 1.0f,
     0.5f, 1.2f,  0.6f,   1.0f, 1.0f,
    -0.3f, 1.2f,  0.6f,   0.0f, 1.0f,
    -0.5f, 0.6f,  0.7f,   0.0f, 0.0f,

    // Roof
    -0.3f, 1.2f, -0.6f,   0.0f, 0.0f,
     0.5f, 1.2f, -0.6f,   1.0f, 0.0f,
     0.5f, 1.2f,  0.6f,   1.0f, 1.0f,
     0.5f, 1.2f,  0.6f,   1.0f, 1.0f,
    -0.3f, 1.2f,  0.6f,   0.0f, 1.0f,
    -0.3f, 1.2f, -0.6f,   0.0f, 0.0f,
};

/*void checkCarPosition() {

    // Leia o pixel na posição do carro
    glReadPixels(static_cast<int>(carX), static_cast<int>(carY), 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixelColor);

    // Verifique a cor do pixel para determinar a posição
    if (pixelColor[0] == 247 && pixelColor[1] == 247 && pixelColor[2] == 247) {
        printf("O carro está fora da pista.");
    } else {
        std::cout << "O carro está dentro da pista!" << std::endl;
    }
}*/

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
        std::cout << "Ocorreu um erro iniciando GLEW!" << std::endl;
    } else {
        std::cout << "GLEW OK!" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;
    }

    glEnable(GL_DEPTH_TEST);

    Shader ourShader("vertex.glsl", "fragment.glsl");
    Shader carShader("vertex.glsl", "car_shader.glsl");

    GLuint VBOs[2], VAOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);

    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

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

    data = stbi_load("res/images/pista_nova.jpeg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    data = stbi_load("res/images/opengl.png", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture1", 0);

    carShader.use();
    carShader.setInt("texture2", 0);

    while (!glfwWindowShouldClose(window)) {

        // Configuração do carro
        glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(carVertices), carVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 220.0f);

        ourShader.use();
        glBindVertexArray(VAOs[0]);
        ourShader.setMat4("projection", projection);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (5 * sizeof(float)));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        model = glm::translate(model, glm::vec3(8.0f, 0.0f, 35.0f));  // Translada o carro para a posição desejada
        model = glm::rotate(model, glm::radians(carAngle), glm::vec3(0.0f, 1.0f, 0.0f));  // Roda o carro no seu próprio eixo

        carShader.use();
        glBindVertexArray(VAOs[1]);
        carShader.setMat4("projection", projection);
        glUniformMatrix4fv(glGetUniformLocation(carShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(carShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, sizeof(carVertices) / (5 * sizeof(float)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
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

// Função de inicialização para configurar os vértices originais
void initializeCarVertices() {
    originalVertices.assign(carVertices, carVertices + sizeof(carVertices)/sizeof(float));
}

// Função updateCarVertices atualizada
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
            originalVertices[i * 5],     // x
            originalVertices[i * 5 + 1], // y
            originalVertices[i * 5 + 2], // z
            1.0f
        );

        // Aplica a transformação
        glm::vec4 transformed = transform * vertex;

        // Atualiza os vértices
        carVertices[i * 5] = transformed.x;
        carVertices[i * 5 + 1] = transformed.y;
        carVertices[i * 5 + 2] = transformed.z;
        // Mantém as coordenadas de textura inalteradas
        carVertices[i * 5 + 3] = originalVertices[i * 5 + 3];
        carVertices[i * 5 + 4] = originalVertices[i * 5 + 4];
    }
}

// Funções de movimento atualizadas
void moveCarForward() {
    float deltaSpeed = carState.acceleration;
    carState.speed = std::min(carState.speed + deltaSpeed, carState.maxSpeed);

    // Atualiza a posição baseado na direção atual
    float angleRad = glm::radians(carState.angle);
    carState.position.x += carState.speed * cos(angleRad);  // Trocado sin por cos
    carState.position.z -= carState.speed * sin(angleRad);  // Trocado cos por sin e invertido sinal

    updateCarVertices();
}

void moveCarBackward() {
    float deltaSpeed = carState.acceleration;
    carState.speed = std::max(carState.speed - deltaSpeed, -carState.maxSpeed);

    // Atualiza a posição baseado na direção atual
    float angleRad = glm::radians(carState.angle);
    carState.position.x += carState.speed * cos(angleRad);  // Trocado sin por cos
    carState.position.z -= carState.speed * sin(angleRad);  // Trocado cos por sin e invertido sinal

    updateCarVertices();
}

void moveCarRight() {
    carState.angle -= carState.turnSpeed * (carState.speed != 0 ? 1.0f : 0.0f);
    updateCarVertices();
}

void moveCarLeft() {
    carState.angle += carState.turnSpeed * (carState.speed != 0 ? 1.0f : 0.0f);
    updateCarVertices();
}

// Função para aplicar fricção e atualizar o estado do carro
void updateCarPhysics() {
    // Aplica fricção
    if (carState.speed != 0.0f) {
        carState.speed *= carState.friction;

        // Se a velocidade for muito pequena, pare completamente
        if (std::abs(carState.speed) < 0.001f) {
            carState.speed = 0.0f;
        } else {
            // Continua o movimento na direção atual
            float angleRad = glm::radians(carState.angle);
            carState.position.x -= carState.speed * sin(angleRad);
            carState.position.z -= carState.speed * cos(angleRad);
            updateCarVertices();
        }
    }
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
