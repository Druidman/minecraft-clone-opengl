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
struct DrawArraysIndirectCommand{
    unsigned int  count;
    unsigned int  instanceCount;
    unsigned int  first;
    unsigned int  baseInstance;
};

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
    
    Buffer chunkStorageBuffer;
    
    World* world;

    glm::vec3 cameraPosOnChunkUpdate = glm::vec3(0.0f);

protected:
    Renderer(Buffer csb) : chunkStorageBuffer(csb){

    }
    virtual void initBuffers() = 0;

public:
    void init(World* world){
        this->world = world;
        std::cout << "renderer buffers init\n";
        initBuffers();
    };

    virtual void renderGame(GameState *gameState) = 0;
    
    void render(GameState *gameState){
       
        GLCall( glClearColor(0.68f, 0.84f, 0.9f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

        renderGame(gameState);
        renderUi();

        /* Swap front and back buffers */
        glfwSwapBuffers(gameState->window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    void renderUi(){
        this->crosshairShader.use();
        this->crosshairVAO.bind();
        GLCall( glDrawArrays(GL_TRIANGLES,0,3) );
    };

    virtual void fillBuffers() = 0;
    
    void fillChunkStorageBuffer()
    {   
        std::cout << "CHUNK STORAGE BUFFER UPDATE\n";
        std::vector<glm::vec4> chunkPositions;
        for (Chunk* chunk : this->world->chunkRenderRefs){
            
            chunkPositions.push_back(glm::vec4(chunk->position - this->world->player->camera->position,0.0));
            
        }       
        
        this->chunkStorageBuffer.fillData<glm::vec4>(&chunkPositions, sizeof(glm::vec4) * 1024);
        
        cameraPosOnChunkUpdate = this->world->player->camera->position;
    }
};

#endif