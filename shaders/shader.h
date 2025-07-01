#ifndef SHADER_C
#define SHADER_C

#include "../vendor/glm/glm.hpp"
#include <string>
#include <iostream>
#include <GL/glew.h>
// load shader from file name
// program operations
// uniforms

class Shader{
    private:
        unsigned int m_programID;

        std::string readFile(std::string path);
        unsigned int attachShader(std::string shaderSource, GLenum shaderType);
    public:
        ~Shader();
        Shader(std::string vertexPath, std::string fragmentPath);

        void use();

        void setInt(const char* uniformName, int val);
        void setFloat(const char* uniformName, float val);
        void setMatrixFloat(const char* uniformName, bool transpose, glm::mat4 &matrix);
};
#endif