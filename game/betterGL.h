#ifndef BETTER_GL_H
#define BETTER_GL_H


#include "vendor/glad/glad.h"


#include <iostream>
#include <fstream>
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
inline void WriteToLogFile(std::string messageType, std::string msg){
    std::ofstream file;
    file.open("logs", std::ios::app);
    file << "\n\n[ " << messageType <<" ]:{ " << __TIME__ << " }\n" << "Message: \n" << msg << ".\n";
    file.close(); 
}

inline void ClearLogs(){
    std::ofstream file;
    file.open("logs");
    file << "";
    file.close();
}

#endif