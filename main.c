#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

const char* readShaderSource(const char* filePath) {
    FILE* file = fopen(filePath, "r");
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filePath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* shaderSource = (char *)malloc((fileSize + 1) * sizeof(char));
    if (!shaderSource) {
        fprintf(stderr, "Failed to allocate memory for shader source\n");
        fclose(file);
        return NULL;
    }
    size_t readSize = fread(shaderSource, 1, fileSize, file);
    if (readSize != fileSize) {
        fprintf(stderr, "Failed to read shader file: %s\n", filePath);
        free(shaderSource);
        fclose(file);
        return NULL;
    }
    shaderSource[fileSize] = '\0';
    fclose(file);
    return shaderSource;
}

unsigned int loadVertexShader() {
    const char* vertexShaderCode = readShaderSource("vertex.glsl");
    if (!vertexShaderCode) return 0;

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);
    free((void *)vertexShaderCode);

    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s", infoLog);
    }

    return vertexShader;
}

unsigned int loadFragmentShader() {
    const char* fragmentShaderCode = readShaderSource("fragment.glsl");
    if (!fragmentShaderCode) return 0;

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);
    free((void *)fragmentShaderCode);

    int success;
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n %s", infoLog);
    }
    return fragmentShader;
}


unsigned int linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    unsigned int shaderProgram = glCreateProgram();
    if (vertexShader != 0) glAttachShader(shaderProgram, vertexShader);
    else fprintf(stderr, "ERROR::PROGRAM::VERTEX_SHADER_INVALID\n"); 
    if (fragmentShader != 0) glAttachShader(shaderProgram, fragmentShader);
    else fprintf(stderr, "ERROR::PROGRAM::FRAGMENT_SHADER_INVALID\n");
    glLinkProgram(shaderProgram);
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "ERROR::PROGRAM::LINKING_FAILED\n %s", infoLog);
    }
    if (vertexShader != 0) glDeleteShader(vertexShader);
    if (fragmentShader != 0) glDeleteShader(fragmentShader);
    return shaderProgram;
}

unsigned int setupTriangles() {
    float vertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays (1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW Example", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, 800, 600);

    unsigned int vertexShader = loadVertexShader();
    if (!vertexShader) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    unsigned int fragmentShader = loadFragmentShader();
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    unsigned int shaderProgram = linkShaderProgram(vertexShader, fragmentShader);
    if (!shaderProgram) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    unsigned int VAO = setupTriangles();
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

