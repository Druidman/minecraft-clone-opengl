#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>


#include "betterGL.h"


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/stb_image/stb_image.h"

#include "shader.h"
#include "texture.h"
#include "vertexArray.h"
#include "vertexBuffer.h"
#include "elementBuffer.h"
#include "block.h"
#include "chunk.h"



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
glm::mat4 projection = glm::perspective(glm::radians(45.0),(double)WINDOW_WIDTH/WINDOW_HEIGHT,0.1,100.0);

bool removeChunk = false;


void resize_callback(GLFWwindow *window, int width, int height){
    glViewport(0,0,width,height);
    projection = glm::perspective(glm::radians(45.0),(double)width/height,0.1,100.0);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

void process_key_release(GLFWwindow *window, int key){
    if (key ==GLFW_KEY_ESCAPE){
        glfwSetWindowShouldClose(window,true);
    }
    
    if (key == GLFW_KEY_ENTER){
        removeChunk = true;
    }
}

void process_key_press(GLFWwindow *window, int key){
    
}

void process_input(GLFWwindow *window){
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

void input_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
    if (action == GLFW_RELEASE){
        process_key_release(window,key);
    }
    else if (action == GLFW_PRESS){
        process_key_press(window,key);
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

std::vector<float> world_gen(int sizex , int sizey){
    std::vector<float> world(sizex * sizey, 0.0f);

    for (int i =0; i<sizex; i++){
        for (int j =0; j<sizey; j++){
            world[(i*sizey) + j] = 0.0f;
        }   
    }

    return world;

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
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window,resize_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window,input_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glewInit();

    stbi_set_flip_vertically_on_load(true);
    
    GLCall( glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT) ) ;
    GLCall( glEnable(GL_DEPTH_TEST) );



    std::vector< Chunk > chunks;


    std::filesystem::path cwd = std::filesystem::current_path();

    std::filesystem::path vsPath = cwd / "shaders/vertexShader.vs";
    std::filesystem::path fsPath = cwd / "shaders/fragmentShader.fs";
    
    Shader shader = Shader(vsPath.string(), fsPath.string());


    std::filesystem::path texturePath = cwd / "textures/texture.jpg";

    Texture texture = Texture(texturePath, "jpg");

    VertexArray vao = VertexArray();
    VertexBuffer vbo = VertexBuffer();
    ElementBuffer ebo = ElementBuffer();
    VertexBuffer vboInstancedPos = VertexBuffer();
    VertexBuffer vboInstancedTex = VertexBuffer();
    
 
    vbo.fillData<float>(blockVertices,BLOCK_VERTICES_COUNT);
    ebo.fillData(blockIndicies,BLOCK_INDICIES_COUNT);

    vbo.bind();
    vao.setAttr(0,3,GL_FLOAT,5 * sizeof(float),0);
    vao.setAttr(1,2,GL_FLOAT,5 * sizeof(float),3 * sizeof(float));

    vboInstancedPos.bind();
    vao.setAttr(2,3,GL_FLOAT,3 * sizeof(float),0);
    GLCall( glVertexAttribDivisor(2,1) );

    vboInstancedTex.bind();
    vao.setAttr(3,2,GL_FLOAT,2 * sizeof(float),0);
    GLCall( glVertexAttribDivisor(3,1) );

    GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    GLCall( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    GLCall( glBindVertexArray(0) );


    const int WORLD_WIDTH = 256;
    const unsigned long long WORLD_BLOCKS_COUNT = WORLD_WIDTH * WORLD_WIDTH;
    const unsigned long long WORLD_CHUNKS_COUNT = WORLD_BLOCKS_COUNT / (CHUNK_WIDTH * CHUNK_WIDTH);
    std::vector<float> world = world_gen(WORLD_WIDTH,WORLD_WIDTH);


    int startX = 0;
    int startY = 0;
    int blockInd = 0;
    for (int chunkInd =0; chunkInd < WORLD_CHUNKS_COUNT; chunkInd++){
        if (startX >= WORLD_WIDTH){
            startX = 0;
            startY += CHUNK_WIDTH;
        }

        glm::vec3 chunkPos = glm::vec3(startX + (CHUNK_WIDTH / 2), 0.0, startY + (CHUNK_WIDTH / 2));
        Chunk chunk = Chunk(chunkPos,vboInstancedPos,vboInstancedTex);
        
        
        for (int i =startX; i< startX + CHUNK_WIDTH ; i++){
            for (int j =startY; j< startY + CHUNK_WIDTH; j++){
                Block block(GRASS_DIRT,glm::vec3(i,world[blockInd],j));
                chunk.addBlock(block);
                blockInd++;
            }
        }

        chunks.push_back(chunk);
        startX += CHUNK_WIDTH;
        
    }
    
        
    

    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {

        process_input(window);
        /* Render here */
        if (removeChunk){
            chunks.pop_back();
            removeChunk = false;
        }
        
        GLCall( glClearColor(0.2f, 0.3f, 0.3f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT) );

        shader.use();

        view = glm::lookAt(cameraPos,cameraPos - cameraFront, cameraUp);
       
        shader.setMatrixFloat("projection",GL_FALSE,projection);
        shader.setMatrixFloat("view",GL_FALSE,view);
        shader.setMatrixFloat("model",GL_FALSE,model);

        vao.bind();
        vbo.bind();
        ebo.bind();

        for (int i = 0; i <chunks.size(); i++){
            chunks[i].render();
        }
        
        
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    }
    
    glfwTerminate();
    return 0;
}

