// #include "vendor/glad/glad.h"
// #include <GLFW/glfw3.h>

// #include <iostream>

// int main() {
//     if (!glfwInit()) return -1;

//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     GLFWwindow* window = glfwCreateWindow(640, 480, "Core Profile Test", NULL, NULL);
//     if (!window) { glfwTerminate(); return -1; }
//     glfwMakeContextCurrent(window);

//     if (!gladLoadGL(glfwGetProcAddress)) {
//         std::cerr << "Failed to initialize GLAD for OpenGL 4.6 Core" << std::endl;
//         return -1;
//     }

//     GLuint vao;
//     glGenVertexArrays(1, &vao);
//     glBindVertexArray(vao);

//     while (!glfwWindowShouldClose(window)) {
//         glClear(GL_COLOR_BUFFER_BIT);
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glDeleteVertexArrays(1, &vao);
//     glfwTerminate();
//     return 0;
// }