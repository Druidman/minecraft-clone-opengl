#ifndef BETTER_GL_H
#define BETTER_GL_H
#include <GL/glew.h>
#include <iostream>
#include <string>

#define GLCall(x) \
    while(glGetError() != GL_NO_ERROR){}; \
    x; \
    { \
        GLenum error = glGetError();\
        if (error != GL_NO_ERROR){ \
            std::cout << "[OPENGL_ERROR " << error << " ] " << "occured at "<< #x <<"in line: " << __LINE__ << " , FILE: " << __FILE__ << "\n";\
            std::exit(EXIT_FAILURE); \
        } \
    }

inline void ExitError(std::string errorType, std::string msg){
    std::cout << "[ " << errorType << " ]\n" << "Error message: " << msg << "\n";
    std::exit(EXIT_FAILURE);
}

#endif