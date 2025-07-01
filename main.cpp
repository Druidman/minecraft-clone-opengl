#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include "betterGL.h"


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/stb_image/stb_image.h"

#include "shaders/shader.h"
#include "textures/texture.h"
#include "vertexArray.h"
#include "vertexBuffer.h"
#include "elementBuffer.h"
#include "block.h"



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
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewInit();

    stbi_set_flip_vertically_on_load(true);
    
    GLCall( glViewport(0,0,800,600) ) ;
    GLCall( glEnable(GL_DEPTH_TEST)   );
    int textures = 100;
    float* blockTextures = new float[textures * 2];
    int indd =0;
    for (int x =0; x < 10; x++){
        for (int z = 0; z < 10; z++){
            int change = rand() % 2;

            if (change){
                blockTextures[indd] = 0.0;
                blockTextures[indd + 1] = -0.25;
            }
            else{
                blockTextures[indd] = 0.0;
                blockTextures[indd + 1] = 0.0;
            }
            
            
            indd += 2;
        }   
    }

    int blocks = 100;
    float* blockPositions = new float[blocks * 3];
    
    int ind =0;
    for (int x =0; x < 10; x++){
        for (int z = 0; z < 10; z++){
            blockPositions[ind] = x;
            blockPositions[ind + 1] = 0.0;
            blockPositions[ind + 2] = z;
            ind += 3;
        }   
    }
    std::cout << ind;
    
    

    std::filesystem::path cwd = std::filesystem::current_path();

    std::filesystem::path vsPath = cwd / "shaders/vertexShader.vs";
    std::filesystem::path fsPath = cwd / "shaders/fragmentShader.fs";
    
    Shader shader = Shader(vsPath.string(), fsPath.string());


    std::filesystem::path texturePath = cwd / "textures/texture.png";

    Texture texture = Texture(texturePath, "png");

    VertexArray vao = VertexArray();
    VertexBuffer vbo = VertexBuffer();
    ElementBuffer ebo = ElementBuffer();
    VertexBuffer vboInstancedPos = VertexBuffer();
    VertexBuffer vboInstancedTex = VertexBuffer();
    

    vbo.fillData<float>(blockVertices,sizeof(blockVertices));
    ebo.fillData(blockIndicies,sizeof(blockIndicies));
    vboInstancedPos.fillData<float>(blockPositions,blocks * 3);
    vboInstancedTex.fillData<float>(blockTextures,textures * 2);

    vbo.bind();
    vao.setAttr(0,3,GL_FLOAT,5 * sizeof(float),0);
    vao.setAttr(1,2,GL_FLOAT,5 * sizeof(float),3 * sizeof(float));

    vboInstancedPos.bind();
    vao.setAttr(2,3,GL_FLOAT,3 * sizeof(float),0);
    GLCall( glVertexAttribDivisor(2,1) );

    vboInstancedTex.bind();
    vao.setAttr(3,2,GL_FLOAT,2 * sizeof(float),0);
    GLCall( glVertexAttribDivisor(3,1) );


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        process_input(window);
        
        /* Render here */
        
        GLCall( glClearColor(0.2f, 0.3f, 0.3f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT) );

        shader.use();

        view = glm::lookAt(cameraPos,cameraPos - cameraFront, cameraUp);
       
        shader.setMatrixFloat("projection",GL_FALSE,projection);
        shader.setMatrixFloat("view",GL_FALSE,view);
        shader.setMatrixFloat("model",GL_FALSE,model);

        GLCall( glDrawElementsInstanced(GL_TRIANGLES, sizeof(blockIndicies), GL_UNSIGNED_INT, 0,blocks ) );
        
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    }
    delete[] blockPositions;
    glfwTerminate();
    return 0;
}
