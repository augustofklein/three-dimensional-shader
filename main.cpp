#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;

// Camera settings
glm::vec3 cameraPos = glm::vec3(12.28f, 34.37f, 5.37f);
glm::vec3 cameraFront = glm::vec3(-0.53f, -0.66f, 0.52f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 1.0f);

float sensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;

int main()
{
    // GLFW: initialize and configure
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

    // Shaders para chão e carro
    Shader ourShader("vertex.glsl", "fragment.glsl");
    Shader carShader("vertex.glsl", "car_shader.glsl");

    // Dados de vértices para o chão
    float vertices[] = {
        -100.0f,  -0.6f, -100.0f,  0.0f, 1.0f,
         100.0f,  -0.6f, -100.0f,  1.0f, 1.0f,
         100.0f,  -0.6f,  100.0f,  1.0f, 0.0f,
         100.0f,  -0.6f,  100.0f,  1.0f, 0.0f,
        -100.0f,  -0.6f,  100.0f,  0.0f, 0.0f,
        -100.0f,  -0.6f, -100.0f,  0.0f, 1.0f
    };

    // Dados de vértices para o carro
    float vertices_carro[] = {

        // Corpo do carro (base retangular)
         2.0f, -0.5f,  1.0f,  0.0f, 0.0f, // inferior esquerdo traseiro
         2.0f, -0.5f, -1.0f,  1.0f, 0.0f, // inferior direito traseiro
         2.0f,  0.0f, -1.0f,  1.0f, 1.0f, // superior direito traseiro
         2.0f,  0.0f, -1.0f,  1.0f, 1.0f, // superior direito traseiro
         2.0f,  0.0f,  1.0f,  0.0f, 1.0f, // superior esquerdo traseiro
         2.0f, -0.5f,  1.0f,  0.0f, 0.0f, // inferior esquerdo traseiro

        -2.0f, -0.5f,  1.0f,  0.0f, 0.0f, // inferior esquerdo frontal
        -2.0f, -0.5f, -1.0f,  1.0f, 0.0f, // inferior direito frontal
        -2.0f,  0.0f, -1.0f,  1.0f, 1.0f, // superior direito frontal
        -2.0f,  0.0f, -1.0f,  1.0f, 1.0f, // superior direito frontal
        -2.0f,  0.0f,  1.0f,  0.0f, 1.0f, // superior esquerdo frontal
        -2.0f, -0.5f,  1.0f,  0.0f, 0.0f, // inferior esquerdo frontal

        // Retângulo traseiro
         2.0f, -0.5f,  1.0f, 0.0f, 0.0f, // inferior esquerdo traseiro
         2.0f, -0.5f, -1.0f, 1.0f, 0.0f, // inferior direito traseiro
         2.0f,  0.0f, -1.0f, 1.0f, 1.0f, // superior direito traseiro
         2.0f,  0.0f,  1.0f, 1.0f, 1.0f, // superior esquerdo traseiro

        // Retângulo frontal
        -2.0f, -0.5f,  1.0f, 0.0f, 0.0f,   // inferior esquerdo frontal
        -2.0f, -0.5f, -1.0f, 1.0f, 0.0f,   // inferior direito frontal
        -2.0f,  0.0f, -1.0f, 1.0f, 1.0f,   // superior direito frontal
        -2.0f,  0.0f,  1.0f, 1.0f, 1.0f,   // superior esquerdo frontal

        // Face esquerda
         2.0f, -0.5f,  1.0f,  0.0f, 0.0f,  // inferior esquerdo traseiro
         2.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // superior esquerdo traseiro
        -2.0f,  0.0f,  1.0f,  1.0f, 1.0f,  // superior esquerdo frontal
        -2.0f, -0.5f,  1.0f,  1.0f, 1.0f,  // inferior esquerdo frontal

        // Face direita
         2.0f, -0.5f, -1.0f, 0.0f, 0.0f,   // inferior direito traseiro
         2.0f,  0.0f, -1.0f, 1.0f, 0.0f,   // superior direito traseiro
        -2.0f,  0.0f, -1.0f, 1.0f, 1.0f,   // superior direito frontal
        -2.0f, -0.5f, -1.0f, 1.0f, 1.0f,   // inferior direito frontal

        // Face inferior
         2.0f, -0.5f,  1.0f, 0.0f, 0.0f,   // inferior esquerdo traseiro
         2.0f, -0.5f, -1.0f, 1.0f, 0.0f,   // inferior direito traseiro
        -2.0f, -0.5f, -1.0f, 1.0f, 1.0f,   // inferior direito frontal
        -2.0f, -0.5f,  1.0f, 1.0f, 1.0f,   // inferior esquerdo frontal

        // Face superior
         2.0f,  0.0f,  1.0f, 0.0f, 0.0f,   // superior esquerdo traseiro
         2.0f,  0.0f, -1.0f, 1.0f, 0.0f,   // superior direito traseiro
        -2.0f,  0.0f, -1.0f, 1.0f, 1.0f,   // superior direito frontal
        -2.0f,  0.0f,  1.0f, 1.0f, 1.0f,   // superior esquerdo frontal

        // Teto do carro (retângulo menor)
         1.0f,  0.0f,  0.7f,  0.0f, 0.0f, // inferior esquerdo traseiro
         1.0f,  0.0f, -0.7f,  1.0f, 0.0f, // inferior direito traseiro
         1.0f,  0.4f, -0.7f,  1.0f, 1.0f, // superior direito traseiro
         1.0f,  0.4f, -0.7f,  1.0f, 1.0f, // superior direito traseiro
         1.0f,  0.4f,  0.7f,  0.0f, 1.0f, // superior esquerdo traseiro
         1.0f,  0.0f,  0.7f,  0.0f, 0.0f, // inferior esquerdo traseiro

        -1.0f,  0.0f,  0.7f,  0.0f, 0.0f, // inferior esquerdo frontal
        -1.0f,  0.0f, -0.7f,  1.0f, 0.0f, // inferior direito frontal
        -1.0f,  0.4f, -0.7f,  1.0f, 1.0f, // superior direito frontal
        -1.0f,  0.4f, -0.7f,  1.0f, 1.0f, // superior direito frontal
        -1.0f,  0.4f,  0.7f,  0.0f, 1.0f, // superior esquerdo frontal
        -1.0f,  0.0f,  0.7f,  0.0f, 0.0f  // inferior esquerdo frontal

    };

    GLuint VBOs[2], VAOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);

    // Configuração do chão
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Configuração do carro
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_carro), vertices_carro, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texturas
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("res/images/pista_corrida.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
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
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture1", 0);
    carShader.use();
    carShader.setInt("texture2", 0);

    // Loop de renderização
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        ourShader.use();
        glBindVertexArray(VAOs[0]);
        ourShader.setMat4("projection", projection);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / (5 * sizeof(float)));

        glBindTexture(GL_TEXTURE_2D, texture2);
        model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 30.0f));

        carShader.use();
        glBindVertexArray(VAOs[1]);
        carShader.setMat4("projection", projection);
        glUniformMatrix4fv(glGetUniformLocation(carShader.ID, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(carShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices_carro) / (5 * sizeof(float)));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glfwTerminate();
    return 0;
}

void viraCamera(float x, float y)
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    const float cameraSpeed = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

     // ajustar de acordo com a velocidade do computador
     if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;

     if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

     if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

     if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        viraCamera(0.0f, 1.0f);

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        viraCamera(0.0f, -1.0f);

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        viraCamera(-1.0f, 0.0f);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        viraCamera(1.0f, 0.0f);

     if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;

     if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        cameraPos += glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


