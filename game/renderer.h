#ifndef RENDERER_H
#define RENDERER_H
#include "betterGL.h"
#include <GLFW/glfw3.h>

#include "shader.h"
#include "vertexArray.h"
#include "buffer.h"
#include "texture.h"



class World;
class Chunk;


struct GameState {

    GLFWwindow *window;

    glm::mat4 *model;
    glm::mat4 *view;
    glm::mat4 *projection;
    World* world;
};


class Renderer
{
private:
    std::string cvsPath = "shaders/vertexShaderCrosshair.vs";
    std::string cfsPath = "shaders/fragmentShaderCrosshair.fs";
    std::string texturePath = "textures/texture.png";
 
    Texture texture = Texture(texturePath, "png");
protected:
    Shader crosshairShader = Shader(cvsPath,cfsPath);
    std::vector<float> crosshairVertices = {
        -0.01f, -0.01f, 0.0f,
        0.01f, -0.01f, 0.0f,
        0.0f,  0.0f, 0.0f
    };

    VertexArray crosshairVAO = VertexArray();
    Buffer crosshairVBO = Buffer(GL_ARRAY_BUFFER);
    
    World* world;

    glm::vec3 cameraPosOnChunkUpdate = glm::vec3(0.0f);

protected:
    Renderer(){

    }
    virtual void initBuffers() = 0;

public:
    void init(World* world){
        this->world = world;
        std::cout << "renderer buffers init\n";
        initBuffers();
    };

    virtual void updateLogs() = 0;
    virtual void renderGame(GameState *gameState) = 0;
    
    void render(GameState *gameState){
       
        GLCall( glClearColor(0.68f, 0.84f, 0.9f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

        renderGame(gameState);
        renderUi();

        updateLogs();
        GLenum err;
        
        /* Swap front and back buffers */
        glfwSwapBuffers(gameState->window);
        while ((err = glGetError()) != GL_NO_ERROR) {
            printf("OpenGL Error: %x\n", err);
        }

        /* Poll for and process events */
        glfwPollEvents();
    }

    void renderUi(){
        this->crosshairShader.use();
        this->crosshairVAO.bind();
        GLCall( glDrawArrays(GL_TRIANGLES,0,3) );
    };

    virtual void fillBuffers() = 0;
    virtual void fillBuffer(BufferType bufferToFill) = 0;

    virtual bool updateChunk(Chunk* chunk) = 0;
    virtual bool addChunk(Chunk* chunk) = 0;
    virtual bool deleteChunk(Chunk* chunk, bool merge = false) = 0;

    virtual bool updateChunk(Chunk* chunk, BufferType bufferToUpdate) = 0;
    virtual bool addChunk(Chunk* chunk, BufferType bufferToUpdate) = 0;
    virtual bool deleteChunk(Chunk* chunk, BufferType bufferToUpdate, bool merge = false) = 0;

 
};

#endif