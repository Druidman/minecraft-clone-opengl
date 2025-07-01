#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/stb_image/stb_image.h"

#include "shaders/shader.h"

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

typedef unsigned int uint;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
glm::vec3 cameraPos = glm::vec3(0.0,0.0,3.0);
glm::vec3 cameraFront = glm::vec3(0.0,0.0,1.0);
glm::vec3 cameraUp = glm::vec3(0.0,1.0,0.0);

glm::vec2 cursorPos = glm::vec2(0.0,0.0);

glm::vec2 mouseoffset = glm::vec2(0.0,0.0);

double yaw = 90, pitch = 0;

bool firstMouse = true;
glm::mat4 model(1.0f);
glm::mat4 view = glm::lookAt(cameraPos,cameraPos + cameraFront,cameraUp);
glm::mat4 projection = glm::perspective(glm::radians(45.0),800.0/600,0.1,100.0);

void resize_callback(GLFWwindow *window, int width, int height){
    glViewport(0,0,width,height);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}
void process_input(GLFWwindow *window){
    if (glfwGetKey(window,GLFW_KEY_ESCAPE)){
        glfwSetWindowShouldClose(window,true);
    }
    if (glfwGetKey(window,GLFW_KEY_W)){
        cameraPos -= 0.1f * cameraFront;
    }
    if (glfwGetKey(window,GLFW_KEY_S)){
        cameraPos += 0.1f * cameraFront;
    }
    if (glfwGetKey(window,GLFW_KEY_D)){
        cameraPos += 0.1f * glm::normalize(glm::cross(cameraUp,cameraFront));
    }
    if (glfwGetKey(window,GLFW_KEY_A)){
        cameraPos -= 0.1f * glm::normalize(glm::cross(cameraUp,cameraFront));
    }
}
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos){
    
    if (firstMouse)
    {
        
        cursorPos.x = xpos;
        cursorPos.y = ypos;
        firstMouse = false;
    }
    
    
    
    double changeX = cursorPos.x - xpos;
    double changeY = cursorPos.y - ypos;

    

    yaw -= changeX * 0.1;
    pitch -= changeY * 0.1;

    if (pitch <= -89.0){
        pitch = -89.0;
    }
    else if (pitch >= 89.0){
        pitch = 89.0;
    }


    glm::vec3 direction;

        
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));


    cameraFront = glm::normalize(direction);

    cursorPos.x = xpos;
    cursorPos.y = ypos;

}

int main(void)
{
    
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    /* Create a windowed mode window and its OpenGL context */
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window,resize_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewInit();

    stbi_set_flip_vertically_on_load(true);
    
    GLCall( glViewport(0,0,800,600) ) ;
    GLCall( glEnable(GL_DEPTH_TEST)   );

    int width,height,nChannels;
    unsigned char *img = stbi_load("textures/dirtTexture.png",&width,&height,&nChannels,0);
    if (img == NULL){
        std::cout << "ERROR LOADING TEXTURE IMG" << "\n";
        std::exit(EXIT_FAILURE);
    }
    uint texture;
    GLCall( glGenTextures(1, &texture) );
    GLCall( glActiveTexture(GL_TEXTURE7) );
    GLCall( glBindTexture(GL_TEXTURE_2D,texture) );

    GLCall( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) );
    GLCall( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) );
    GLCall( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST) );
    GLCall( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );

    GLCall( glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,img) );
    GLCall( glGenerateMipmap(GL_TEXTURE_2D) );
    stbi_image_free(img);

    float vertices[] = {
        // Top face (Y+)
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.25f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.25f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    
        // Bottom face (Y-)
        -0.5f, -0.5f, -0.5f,  0.5f, 1.0f,  // 4
         0.5f, -0.5f, -0.5f,  0.75f, 1.0f,  // 5
         0.5f, -0.5f,  0.5f,  0.75f, 0.0f,  // 6
        -0.5f, -0.5f,  0.5f,  0.5f, 0.0f,  // 7
    
        // Front face (Z+)
        -0.5f, -0.5f,  0.5f,  0.25f, 0.0f,  // 8
         0.5f, -0.5f,  0.5f,  0.5f, 0.0f,   // 9
         0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   // 10
        -0.5f,  0.5f,  0.5f,  0.25f, 1.0f,  // 11
    
        // Back face (Z-)
        -0.5f, -0.5f, -0.5f,  0.5f, 0.0f,  // 12
         0.5f, -0.5f, -0.5f,  0.25f, 0.0f,   // 13
         0.5f,  0.5f, -0.5f,  0.25f, 1.0f,   // 14
        -0.5f,  0.5f, -0.5f,  0.5f, 1.0f,  // 15
    
        // Left face (X-)
        -0.5f, -0.5f, -0.5f,  0.25f, 0.0f,  // 16
        -0.5f, -0.5f,  0.5f,  0.5f, 0.0f,   // 17
        -0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   // 18
        -0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  // 19
    
        // Right face (X+)
         0.5f, -0.5f, -0.5f,  0.5f, 0.0f,  // 20
         0.5f, -0.5f,  0.5f,  0.25f, 0.0f,   // 21
         0.5f,  0.5f,  0.5f,  0.25f, 1.0f,   // 22
         0.5f,  0.5f, -0.5f,  0.5f, 1.0f   // 23
    };
    

    unsigned int indicies[] = {
        0, 1, 2, 2, 3, 0,        // Top
        4, 5, 6, 6, 7, 4,        // Bottom
        8, 9,10,10,11, 8,        // Front
       12,13,14,14,15,12,        // Back
       16,17,18,18,19,16,        // Left
       20,21,22,22,23,20         // Right
    };

    Shader shader = Shader("shaders/vertexShader.vs", "shaders/fragmentShader.fs");
    uint VBO, VAO, EBO;
    GLCall( glGenBuffers(1,&VBO) );
    GLCall( glGenBuffers(1,&EBO) );
    GLCall( glGenVertexArrays(1,&VAO) );

    GLCall( glBindVertexArray(VAO) );

    GLCall( glBindBuffer(GL_ARRAY_BUFFER,VBO) );
    GLCall( glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW) );

    // positions
    GLCall( glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 5 * sizeof(float), (void*)0) );
    GLCall( glEnableVertexAttribArray(0) );

    // texture uv's
    GLCall( glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))) );
    GLCall( glEnableVertexAttribArray(1) );

    GLCall( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO) );
    GLCall( glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indicies),indicies,GL_STATIC_DRAW) );

    
    
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        process_input(window);
        
        /* Render here */
        
        GLCall( glClearColor(0.2f, 0.3f, 0.3f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT) );

        shader.use();

        view = glm::lookAt(cameraPos,cameraPos - cameraFront, cameraUp);
       

        shader.setInt("texture8", 7);
        shader.setMatrixFloat("projection",GL_FALSE,projection);
        shader.setMatrixFloat("view",GL_FALSE,view);
        shader.setMatrixFloat("model",GL_FALSE,model);

        GLCall( glDrawElements(GL_TRIANGLES, sizeof(indicies), GL_UNSIGNED_INT, 0) );
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    }

    glfwTerminate();
    return 0;
}
