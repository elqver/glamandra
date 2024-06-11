const char* readShaderSource(const char* filePath);
unsigned int loadVertexShader(const char* filePath); 
unsigned int loadFragmentShader(const char* filePath); 
unsigned int linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);

