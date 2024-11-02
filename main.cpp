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

// determina as posicoes iniciais da camera
glm::vec3 cameraPos = glm::vec3(12.28f, 34.37f, 5.37f);
glm::vec3 cameraFront = glm::vec3(-0.53f, -0.66f, 0.52f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 1.0f);

float sensitivity = 0.1f;
float yaw = -90.0f;
float pitch = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL : Corrida Maluca", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glew: load all OpenGL function pointers
    // ---------------------------------------
    if(glewInit()!=GLEW_OK) {
        std::cout << "Ocorreu um erro iniciando GLEW!" << std::endl;
    } else {
        std::cout << "GLEW OK!" << std::endl;
        std::cout << glGetString(GL_VERSION) << std::endl;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("vertex.glsl", "fragment.glsl");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {

        // Chão (manter o mesmo)
        -100.0f,  -0.6f, -100.0f,  0.0f, 1.0f,
         100.0f,  -0.6f, -100.0f,  1.0f, 1.0f,
         100.0f,  -0.6f,  100.0f,  1.0f, 0.0f,
         100.0f,  -0.6f,  100.0f,  1.0f, 0.0f,
        -100.0f,  -0.6f,  100.0f,  0.0f, 0.0f,
        -100.0f,  -0.6f, -100.0f,  0.0f, 1.0f
    };

    float vertices_carro[] = {

                // Corpo do carro (base retangular)
        -1.0f, -0.5f, -2.0f,  0.0f, 0.0f, // inferior esquerdo traseiro
         1.0f, -0.5f, -2.0f,  1.0f, 0.0f, // inferior direito traseiro
         1.0f,  0.0f, -2.0f,  1.0f, 1.0f, // superior direito traseiro
         1.0f,  0.0f, -2.0f,  1.0f, 1.0f, // superior direito traseiro
        -1.0f,  0.0f, -2.0f,  0.0f, 1.0f, // superior esquerdo traseiro
        -1.0f, -0.5f, -2.0f,  0.0f, 0.0f, // inferior esquerdo traseiro

        -1.0f, -0.5f,  2.0f,  0.0f, 0.0f, // inferior esquerdo frontal
         1.0f, -0.5f,  2.0f,  1.0f, 0.0f, // inferior direito frontal
         1.0f,  0.0f,  2.0f,  1.0f, 1.0f, // superior direito frontal
         1.0f,  0.0f,  2.0f,  1.0f, 1.0f, // superior direito frontal
        -1.0f,  0.0f,  2.0f,  0.0f, 1.0f, // superior esquerdo frontal
        -1.0f, -0.5f,  2.0f,  0.0f, 0.0f, // inferior esquerdo frontal

        // Teto do carro (retângulo menor)
        -0.7f,  0.0f, -1.0f,  0.0f, 0.0f, // inferior esquerdo traseiro
         0.7f,  0.0f, -1.0f,  1.0f, 0.0f, // inferior direito traseiro
         0.7f,  0.4f, -1.0f,  1.0f, 1.0f, // superior direito traseiro
         0.7f,  0.4f, -1.0f,  1.0f, 1.0f, // superior direito traseiro
        -0.7f,  0.4f, -1.0f,  0.0f, 1.0f, // superior esquerdo traseiro
        -0.7f,  0.0f, -1.0f,  0.0f, 0.0f, // inferior esquerdo traseiro

        -0.7f,  0.0f,  1.0f,  0.0f, 0.0f, // inferior esquerdo frontal
         0.7f,  0.0f,  1.0f,  1.0f, 0.0f, // inferior direito frontal
         0.7f,  0.4f,  1.0f,  1.0f, 1.0f, // superior direito frontal
         0.7f,  0.4f,  1.0f,  1.0f, 1.0f, // superior direito frontal
        -0.7f,  0.4f,  1.0f,  0.0f, 1.0f, // superior esquerdo frontal
        -0.7f,  0.0f,  1.0f,  0.0f, 0.0f, // inferior esquerdo frontal

        // Roda traseira esquerda (caixa)
        -1.1f, -0.6f, -1.5f,  0.0f, 0.0f,
        -0.7f, -0.6f, -1.5f,  1.0f, 0.0f,
        -0.7f, -0.3f, -1.5f,  1.0f, 1.0f,
        -0.7f, -0.3f, -1.5f,  1.0f, 1.0f,
        -1.1f, -0.3f, -1.5f,  0.0f, 1.0f,
        -1.1f, -0.6f, -1.5f,  0.0f, 0.0f,

        // Roda traseira direita (caixa)
         1.1f, -0.6f, -1.5f,  0.0f, 0.0f,
         0.7f, -0.6f, -1.5f,  1.0f, 0.0f,
         0.7f, -0.3f, -1.5f,  1.0f, 1.0f,
         0.7f, -0.3f, -1.5f,  1.0f, 1.0f,
         1.1f, -0.3f, -1.5f,  0.0f, 1.0f,
         1.1f, -0.6f, -1.5f,  0.0f, 0.0f,

        // Roda dianteira esquerda (caixa)
        -1.1f, -0.6f,  1.5f,  0.0f, 0.0f,
        -0.7f, -0.6f,  1.5f,  1.0f, 0.0f,
        -0.7f, -0.3f,  1.5f,  1.0f, 1.0f,
        -0.7f, -0.3f,  1.5f,  1.0f, 1.0f,
        -1.1f, -0.3f,  1.5f,  0.0f, 1.0f,
        -1.1f, -0.6f,  1.5f,  0.0f, 0.0f,

        // Roda dianteira direita (caixa)
         1.1f, -0.6f,  1.5f,  0.0f, 0.0f,
         0.7f, -0.6f,  1.5f,  1.0f, 0.0f,
         0.7f, -0.3f,  1.5f,  1.0f, 1.0f,
         0.7f, -0.3f,  1.5f,  1.0f, 1.0f,
         1.1f, -0.3f,  1.5f,  0.0f, 1.0f,
         1.1f, -0.6f,  1.5f,  0.0f, 0.0f

    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_carro), vertices_carro, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // load and create a texture
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char *data = stbi_load("res/images/pista_corrida.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("res/images/pista_corrida.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // activate shader
        ourShader.use();

        const float radius = 2.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;

        // create transformations
        glm::mat4 model         = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 view          = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection    = glm::mat4(1.0f);
        model = glm::rotate(model, (float)0, glm::vec3(0.0f, 1.0f, 0.0f));
        view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // retrieve the matrix uniform locations
        unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
        unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");

        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        ourShader.setMat4("projection", projection);

        // render box
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/5);
        //glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices_carro)/5);


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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


