#ifndef RENDERER_H
#define RENDERER_H
#include "player.h"
#include "world.h"
#include "shader.h"
#include "vertexArray.h"
#include "buffer.h"

struct DrawArraysIndirectCommand{
    unsigned int  count;
    unsigned int  instanceCount;
    unsigned int  first;
    unsigned int  baseInstance;
};

struct GameState {
    Player *player;
    World *world;
    Shader *shader;
    VertexArray *vao;
    GLFWwindow *window;

    Shader *crosshairShader;
    VertexArray *crosshairVAO;

    glm::mat4 *model;
    glm::mat4 *view;
    glm::mat4 *projection;
};

class Renderer
{
    
public:
    void renderRegularGame(GameState *gameState){
        gameState->shader->use();
        
        gameState->shader->setInt("playerState",gameState->player->state);
        gameState->shader->setMatrixFloat("projection",GL_FALSE,*(gameState->projection));
        gameState->shader->setMatrixFloat("view",GL_FALSE,*(gameState->view));
        gameState->shader->setMatrixFloat("model",GL_FALSE,*(gameState->model));
       
        gameState->shader->setVec3Float("LightPos",glm::vec3(256,100,256));
       
        gameState->vao->bind();
        GLCall( glMultiDrawArraysIndirect(GL_TRIANGLES,0,gameState->world->chunkRenderRefs.size() ,sizeof(DrawArraysIndirectCommand)) );
    }
    void renderWebGame(GameState *gameState){
        return; //todo
    }
    void render(GameState *gameState){
       
        GLCall( glClearColor(0.68f, 0.84f, 0.9f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

        renderGame(gameState);
        renderUi(gameState);

        /* Swap front and back buffers */
        glfwSwapBuffers(gameState->window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    void renderGame(GameState *gameState){
        #ifdef __EMSCRIPTEN__
            renderWebGame(gameState);
        #else
            renderRegularGame(gameState);
        #endif
        
    };
    void renderUi(GameState *gameState){
        gameState->crosshairShader->use();
        gameState->crosshairVAO->bind();
        GLCall( glDrawArrays(GL_TRIANGLES,0,3) );
    };

};
#endif