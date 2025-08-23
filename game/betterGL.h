#ifndef BETTER_GL_H
#define BETTER_GL_H


#include "vendor/glad/glad.h"


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
    std::cout << "[ " << errorType << " ]\n" << "Error message: " << msg << " " << __FILE__ << " " << __LINE__ <<"\n";
    std::exit(EXIT_FAILURE);
}

#ifdef __EMSCRIPTEN__
typedef void (*PFNGLMULTIDRAWARRAYSINSTANCEDWEBGLPROC)(
    GLenum mode,
    const GLint* firsts, GLint firstsOffset,
    const GLsizei* counts, GLint countsOffset,
    const GLsizei* instanceCounts, GLint instanceCountsOffset,
    GLsizei drawCount
);


extern PFNGLMULTIDRAWARRAYSINSTANCEDWEBGLPROC glMultiDrawArraysInstancedWEBGL;

void initExtensions();

#endif

#endif