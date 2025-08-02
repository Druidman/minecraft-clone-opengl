#include "shader.h"
#include "vendor/glad/glad.h"
#include <string>
#include <iostream>
#include <fstream>

#include "vendor/glm/gtc/type_ptr.hpp"
#include "vendor/glm/glm.hpp"

#include "betterGL.h"
#include <map>

std::string Shader::readFile(std::string path)
{
    std::ifstream file;
    file.open(path,std::ifstream::in);

    std::string fileContent;
    std::string line;
    while (std::getline(file,line)){
        fileContent += line + "\n";
    }
    file.close();
    
    return fileContent;
}

unsigned int Shader::attachShader(std::string shaderSource, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    if (shader == 0) {
        std::cerr << "ERROR::SHADER::CREATION_FAILED" << std::endl;
        return 0;
    }
    const char* source = shaderSource.c_str();
    GLCall( glShaderSource(shader,1,&source,NULL) );
    GLCall( glCompileShader(shader) );

    GLint success;
    GLCall( glGetShaderiv(shader, GL_COMPILE_STATUS, &success) );
    if (!success) {
        char infoLog[512];
        GLCall( glGetShaderInfoLog(shader, 512, NULL, infoLog) );
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLCall( glAttachShader(m_programID,shader) );

    

    return shader;

}

Shader::~Shader()
{
    GLCall( glDeleteProgram(m_programID) );
}

Shader::Shader(std::string vertexPath, std::string fragmentPath)
{

    m_programID = glCreateProgram();

    // handle shaders

    std::string vertexSource = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);

    unsigned int vertexShader = attachShader(vertexSource,GL_VERTEX_SHADER);
    unsigned int fragmentShader = attachShader(fragmentSource,GL_FRAGMENT_SHADER);

    GLCall( glLinkProgram(m_programID) );
    GLint success;
    GLCall( glGetProgramiv(m_programID, GL_LINK_STATUS, &success) );
    if (!success) {
        char infoLog[512];
        GLCall( glGetProgramInfoLog(m_programID, 512, NULL, infoLog) );
        std::cerr << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    GLCall( glDeleteShader(vertexShader) );
    GLCall( glDeleteShader(fragmentShader) );  
}

void Shader::use()
{
    GLCall( glUseProgram(m_programID) );
}

GLint Shader::getUniformLocation(const std::string &uniformName){
    
    if (!this->uniforms.contains(uniformName)){
        this->uniforms[uniformName] = glGetUniformLocation(m_programID, uniformName.c_str());
    }
    return this->uniforms[uniformName];
 
}
// fix performance in looking for locations
void Shader::setInt(const std::string &uniformName, int val)
{
    
    GLCall( glUniform1i(getUniformLocation(uniformName),val) );
}

void Shader::setFloat(const std::string &uniformName, float val)
{
    GLCall( glUniform1f(getUniformLocation(uniformName),val) );
}

void Shader::setMatrixFloat(const std::string &uniformName, bool transpose, glm::mat4 &matrix)
{
    GLCall( glUniformMatrix4fv(getUniformLocation(uniformName),1,transpose,glm::value_ptr(matrix)) );
}

void Shader::setVec3Float(const std::string &uniformName, glm::vec3 vector3)
{
    GLCall( glUniform3fv(getUniformLocation(uniformName),1,glm::value_ptr(vector3)) );
}
