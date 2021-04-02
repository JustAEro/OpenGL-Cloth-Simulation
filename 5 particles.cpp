#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
//#include <Windows.h>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
//"layout (location = 0) in vec3 aPos2;\n"
"uniform mat4 view;\n"
//"uniform mat4 view2;\n"
"void main()\n"
"{\n"
"   gl_Position = view * vec4(aPos, 1.0);\n"
//"   gl_Position2 = view2 * vec4(aPos2, 1.0);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";


class Particle {
private:
    float x, y, z;
    float vx, vy, vz;
    int lifetime;
public:
    Particle (float x = 0, float y = 0, float z = 0, float vx = 0.2, float vy = 0.2, float vz = 0,  int lifetime = 1000) {
        this->x = x; this->y = y; this->z = z;
        this->vx = vx; this->vy = vy; this->vz = vz;
        this->lifetime = lifetime;
    }

    bool is_alive() { return true ? (lifetime > 0) : false; }

    void set_lifetime(int lifetime){
        this->lifetime = lifetime;
    }

    int get_lifetime() { return lifetime; }
    int get_x() { return x; }
    int get_y() { return y; }
    int get_z() { return z; }
    int get_vx() { return vx; }
    int get_vy() { return vy; }
    int get_vz() { return vz; }

    void move() {
        x += vx;    if (x >= 1) { x = 0.8;  vx *= -1; }
        y += vy;    if (y >= 1) { y = 0.8;  vy *= -1; }
        //z += vz;
    }
};


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;


int main()
{
    // glfw: инициализация и конфигурирование
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // раскомментируйте эту строку, если используете macOS
#endif

    // glfw: создание окна
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: загрузка всех указателей на OpenGL-функции
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Компилирование нашей шейдерной программы

    // Вершинный шейдер
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Проверка на наличие ошибок компилирования вершинного шейдера
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Фрагментный шейдер
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Проверка на наличие ошибок компилирования фрагментного шейдера
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Связывание шейдеров
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Проверка на наличие ошибок компилирования связывания шейдеров
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Указывание вершин (и буферов) и настройка вершинных атрибутов
    float vertices[] = {
        0.4f, 0.1f, 0.0f,
       0.1f, -0.5f, 0.0f,
        0.01f, -0.01f, 0.0f,
        0.0f,  0.01f, 0.0f,
        0.7f, 0.4f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Сначала связываем объект вершинного массива, затем связываем и устанавливаем вершинный буфер(ы), и затем конфигурируем вершинный атрибут(ы)
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Обратите внимание, что данное действие разрешено, вызов glVertexAttribPointer() зарегистрировал VBO как привязанный вершинный буферный объект для вершинного атрибута, так что после этого мы можем спокойно выполнить отвязку
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Вы можете отменить привязку VАО после этого, чтобы другие вызовы VАО случайно не изменили этот VAO (но подобное довольно редко случается).
    // Модификация других VAO требует вызов glBindVertexArray(), поэтому мы обычно не снимаем привязку VAO (или VBO), когда это не требуется напрямую
    glBindVertexArray(0);


    // Раскомментируйте следующую строку для отрисовки полигонов в режиме каркаса
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    srand(time(NULL));
    int lifetime = 1500000;
    double speed[] = {
        -0.009, 0.009, 0
        -0.005, 0.006, 0,
        0.005, 0.004 , 0,
        -0.005, -0.004 , 0,
        0.005, -0.001, 0
    };
    //double vx1 = 0.009, vy1 = -0.009;
    //double vx2 = -0.009, vy2 = 0.008;
    // Цикл рендеринга
    double border = 0.8;
    
    int step = 0;
    int n = 5; //number of particles
    while (!glfwWindowShouldClose(window))
    {
        // Обработка ввода
        processInput(window);
       glEnable(GL_DEPTH_TEST);
        // Рендеринг
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Рисуем наш первый треугольник
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); // поскольку у нас есть только один VАО, то нет необходимости связывать его каждый раз (но мы сделаем это, чтобы всё было немного организованнее)
        glPointSize(15);

        //Particle particle;
        glm::mat4 view = glm::mat4(1.0f);
        //glm::mat4 view2 = glm::mat4(1.0f);

        for (int i = 0; i < 3*n; i+=3) {

            vertices[i] += speed[i]; //+ particle.get_vx();
            if (vertices[i] > border  || vertices[i] < (-1) * border) {
                //vertices[i] = border;
                speed[i+1] *= -1;
                speed[i] *= -1;
            }

            //else if (vertices[i] < (-1) * border) {
                //vertices[i] = -1 * border;
               // speed[i] *= -1;
                //speed[i+1] *= -1;
            //}

            vertices[i+1] += speed[i+1];// + particle.get_vy();;
            if (vertices[i+1] > border || vertices[i+1] < (-1)*border) {
               // vertices[i+1] = border;
                speed[i+1] *= -1;
                speed[i] *= -1;
            }
            //else if (vertices[i+1] < -1 * border) {
                //vertices[i+1] = -1 * border;
               //speed[i+1] *= -1;
                //speed[i] *= -1;
           // }
            
            for (int j = i + 3; j < 3 * n; j+=3) {
                if (  sqrt((vertices[i] - vertices[j])*(vertices[i]-vertices[j]) + (vertices[i+1] - vertices[j+1])*(vertices[i+1]-vertices[j+1])) <= 0.1   ) {
                    speed[i] *= -1;
                    speed[i+1] *= -1;
                    speed[j] *= -1;
                    speed[j+1] *= -1;
                    vertices[i] -= 0.001;
                    vertices[i+1] -= 0.001;
                    vertices[j] += 0.001;
                    vertices[j+1] += 0.001;
              }
            }

            view = glm::translate(view, glm::vec3(vertices[i], vertices[i + 1], 0.0f));
            //view = glm::translate(view, glm::vec3(vertices[3], vertices[4], 0.0f));
            unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
            //glBindVertexArray(VAO);
            glDrawArrays(GL_POINTS, 0, 1);
            lifetime--;
            //vertices[0] = dx;
            //vertices[1] = dy;
            step++;
            
        }
        
            
           //unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");

            //view2 = glm::translate(view2, glm::vec3(vertices[3], vertices[4], 0.0f));
            //unsigned int viewLoc2 = glGetUniformLocation(shaderProgram, "view2");
           // glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, &view2[0][0]);
            //particle.move();
            //int lt = particle.get_lifetime();
           // lt -= 1.0f;
            //particle.set_lifetime(lt);
           

         //glBindVertexArray(0); // не нужно каждый раз его отвязывать

        // glfw: обмен содержимым front- и back- буферов. Отслеживание событий ввода\вывода (была ли нажата/отпущена кнопка, перемещен курсор мыши и т.п.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Опционально: освобождаем все ресурсы, как только они выполнили свое предназначение
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // glfw: завершение, освобождение всех ранее задействованных GLFW-ресурсов
    glfwTerminate();
    return 0;
}

// Обработка всех событий ввода: запрос GLFW о нажатии/отпускании кнопки мыши в данном кадре и соответствующая обработка данных событий
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: всякий раз, когда изменяются размеры окна (пользователем или операционной системой), вызывается данная callback-функция
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Убеждаемся, что окно просмотра соответствует новым размерам окна.
    // Обратите внимание, что высота будет значительно больше, чем указано, на Retina-дисплеях
    glViewport(0, 0, width, height);
}