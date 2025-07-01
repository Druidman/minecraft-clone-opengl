#include "shader.h"
#include "vendor/glm/gtc/type_ptr.hpp"
#include <fstream>

#include "betterGL.h"

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
    const char* source = shaderSource.c_str();
    GLCall( glShaderSource(shader,1,&source,NULL) );
    GLCall( glCompileShader(shader) );
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

    GLCall( glDeleteShader(vertexShader) );
    GLCall( glDeleteShader(fragmentShader) );  
}

void Shader::use()
{
    GLCall( glUseProgram(m_programID) );
}

// fix performance in looking for locations
void Shader::setInt(const char *uniformName, int val)
{
    GLCall( glUniform1i(glGetUniformLocation(m_programID, uniformName),val) );
}

void Shader::setFloat(const char *uniformName, float val)
{
    GLCall( glUniform1f(glGetUniformLocation(m_programID, uniformName),val) );
}

void Shader::setMatrixFloat(const char *uniformName, bool transpose, glm::mat4 &matrix)
{
    GLCall( glUniformMatrix4fv(glGetUniformLocation(m_programID, uniformName),1,transpose,glm::value_ptr(matrix)) );
}
