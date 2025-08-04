#ifndef SHADER_H
#define SHADER_H


#include <string>


#include "vendor/glm/gtc/type_ptr.hpp"
#include "vendor/glm/glm.hpp"
#include "betterGL.h"
#include <map>
// load shader from file name
// program operations
// uniforms

class Shader{
    private:
        unsigned int m_programID;
        std::map<std::string, int> uniforms;

        std::string readFile(std::string path);
        unsigned int attachShader(std::string shaderSource, GLenum shaderType);
        GLint getUniformLocation(const std::string &name);
    public:
        ~Shader();
        Shader(std::string vertexPath, std::string fragmentPath);

        void use();
        unsigned int getProgram(){return m_programID;};
        void setInt(const std::string &uniformName, int val);
        void setFloat(const std::string &uniformName, float val);
        void setMatrixFloat(const std::string &uniformName, bool transpose, glm::mat4 &matrix);
        
        void setVec3Float(const std::string &uniformName,glm::vec3 vector3);
};
#endif