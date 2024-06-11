#include <glad/glad.h>
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

unsigned int loadVertexShader(const char* filePath) {
    const char* vertexShaderCode = readShaderSource(filePath);
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

unsigned int loadFragmentShader(const char* filePath) {
    const char* fragmentShaderCode = readShaderSource(filePath);
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
