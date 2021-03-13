#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#define PI 3.14159265358979323846

#include "stb_image.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

std::vector <float> vertices(2);
std::vector<int> indices(2);
std::vector<int> lineIndices(2);

void generateVertices(std::vector <float>& vertices, std::vector <int>& indices, std::vector <int>& lineIndices,
    float R, float r, int sectorCount, int stackCount);

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
//"layout (location = 1) in vec3 aColor;\n"
//"layout (location = 2) in vec2 aTexCoord;\n"
"out vec4 vertexColor;\n"
//"out vec2 TexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
"   vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n"
//"   TexCoord = aTexCoord;\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
//"in vec3 ourColor;\n"
//"in vec2 TexCoord;\n"
//"uniform sampler2D ourTexture;\n"
//"uniform sampler2D texture1;\n"
//"uniform sampler2D texture2;\n"
"void main()\n"
"{\n"
//"    FragColor = texture(ourTexture, TexCoord);\n"
//"    FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0);\n"  
"    FragColor = ourColor;\n"
"}\n\0";
/*
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\n\0";
*/

int main()
{
    vertices.clear();
    indices.clear();
    lineIndices.clear();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 8);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Cloth Simulation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Сообщаем GLFW, чтобы он захватил наш курсор
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Конфигурирование глобального состояния OpenGL
    glEnable(GL_DEPTH_TEST);

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    generateVertices(vertices, indices, lineIndices, 0.9, 0.3, 100, 100);

    unsigned int VBO, VAO;//, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);

    //Coordinate attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(1);

    // Атрибуты текстурных координат
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glBindVertexArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    //glBindVertexArray(0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


    // Загрузка и создание текстуры
    //unsigned int texture1;
    //glGenTextures(1, &texture1);
    //glActiveTexture(GL_TEXTURE0); // сначала активируем текстурный юнит, прежде чем связывать текстуру
    //glBindTexture(GL_TEXTURE_2D, texture1); // все последующие GL_TEXTURE_2D-операции теперь будут влиять на данный текстурный объект

    // Установка параметров наложения текстуры
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // установка метода наложения текстуры GL_REPEAT (стандартный метод наложения)
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Установка параметров фильтрации текстуры
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Загрузка изображения, создание текстуры и генерирование мипмап-уровней
    //int width, height, nrChannels;
    //unsigned char* data = stbi_load("../textures/wooden_container.jpg", &width, &height, &nrChannels, 0);
    //if (data)
    //{
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //glGenerateMipmap(GL_TEXTURE_2D);
    //}
    //else
    //{
        //std::cout << "Failed to load texture" << std::endl;
    //}
    //stbi_image_free(data);



    while (!glfwWindowShouldClose(window))
    {

        glEnable(GL_MULTISAMPLE);

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очищаем буфер цвета и буфер глубины

        // Связывание текстуры
        //glBindTexture(GL_TEXTURE_2D, texture1);

        glUseProgram(shaderProgram);

        //glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        //uncomment it if you want to draw polygon as a frame
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Создаем преобразование
        glm::mat4 model = glm::mat4(1.0f); // сначала инициализируем единичную матрицу
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // Получаем местоположение uniform-матриц...
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        // ...передаем их в шейдеры (разными способами)
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);

        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        float redValue = cos(timeValue) / 2.0f + 0.5f;
        float blueValue = cos(timeValue + 2) / 2.0f + 0.5f;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, redValue, greenValue, blueValue, 1.0f);

        glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture1);

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLE_STRIP, (unsigned int)lineIndices.size(), GL_UNSIGNED_INT, lineIndices.data());
        glDrawElements(GL_TRIANGLE_STRIP, (unsigned int)indices.size(), GL_UNSIGNED_INT, indices.data());

        //glBindVertexArray(VBO);
        glUniform4f(vertexColorLocation, 0, 0, 0, 1.0f);
        glDrawElements(GL_LINE_LOOP, (unsigned int)lineIndices.size(), GL_UNSIGNED_INT, lineIndices.data());
        //glDrawElements(GL_TRIANGLE_STRIP, (unsigned int)lineIndices.size(), GL_UNSIGNED_INT, lineIndices.data());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    //glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void generateVertices(std::vector <float>& vertices, std::vector <int>& indices, std::vector <int>& lineIndices,
    float R, float r, int sectorCount, int stackCount) {

    float x, y, z, xy;                              // vertex position

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i)
    {
        stackAngle = -PI + 2 * i * stackStep;       // starting from -pi to pi
        xy = R + r * cosf(stackAngle);             // R + r * cos(u)
        z = r * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // the first and last vertices have same position and normal, but different tex coords
        for (int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }
    // indices
    //  k1--k1+1
    //  |  / |
    //  | /  |
    //  k2--k2+1
    unsigned int k1, k2;
    for (int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding 1st and last stacks
            //if (i != 0)
           // {
            indices.push_back(k1);
            indices.push_back(k2);
            indices.push_back(k1 + 1);
            //}

            //if (i != (stackCount - 1))
            //{
            indices.push_back(k1 + 1);
            indices.push_back(k2);
            indices.push_back(k2 + 1);
            //}

            // vertical lines for all stacks
            lineIndices.push_back(k1);
            lineIndices.push_back(k2);
            //if (i != 0)  // horizontal lines except 1st stack
            //{
            lineIndices.push_back(k1);
            lineIndices.push_back(k1 + 1);
            //}
        }
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}