#include "betterGL.h"

#ifdef __EMSCRIPTEN__
    


#include <GLFW/glfw3.h>
#include <cstdio>

// Define the global variable
PFNGLMULTIDRAWARRAYSINSTANCEDWEBGLPROC glMultiDrawArraysInstancedWEBGL = nullptr;

void initExtensions() {
   
    glMultiDrawArraysInstancedWEBGL =
        (PFNGLMULTIDRAWARRAYSINSTANCEDWEBGLPROC) glfwGetProcAddress("multiDrawArraysInstancedWEBGL");

    if (glMultiDrawArraysInstancedWEBGL) {
        printf("WEBGL_multi_draw enabled!\n");
    } else {
        printf("WEBGL_multi_draw not supported, using fallback.\n");
    }
}
#endif