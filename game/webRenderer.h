#ifndef WEB_RENDERER_H
#define WEB_RENDERER_H


#include "vendor/glm/glm.hpp"

#include "renderer.h"
#include "block.h"
#include "chunk.h"
#include "world.h"
#include "player.h"
#include "shader.h"
#include "betterGL.h"



class WebRenderer : public Renderer
{
    private:
        std::string vsPath = "shaders/vertexShaderWeb.vs";
        std::string fsPath = "shaders/fragmentShaderWeb.fs";

        Shader shader = Shader(vsPath, fsPath);
       

        VertexArray vao;
        Buffer baseVbo = Buffer(GL_ARRAY_BUFFER);
        Buffer meshBuffer = Buffer(GL_ARRAY_BUFFER);
        Buffer chunkIDBuffer = Buffer(GL_ARRAY_BUFFER);
    private:
        void addChunkToBuffers(Chunk *chunk, int id){
            

            int idCount = chunk->transparentMesh.size() + chunk->opaqueMesh.size();
            
            std::vector<int> data(idCount, id);
            
            

            this->chunkIDBuffer.addData< int >(&data);

            // vec4 due to std430 in shader
            // this approach passes regular chunk coord to buffer but if we place cam always at 0,0,0 then it won't work
            // this->chunkStorageBuffer->addData<glm::vec4>(glm::vec4(chunk->position,0.0)); 
            // SO we shift chunks pos by camera position

            this->chunkStorageBuffer.addData<glm::vec4>(glm::vec4(chunk->position - this->world->player->camera->position,0.0)); 
   
            this->meshBuffer.addData< CHUNK_MESH_DATATYPE >(chunk->getOpaqueMesh());
   
            this->meshBuffer.addData< CHUNK_MESH_DATATYPE >(chunk->getTransparentMesh());
        }


        
    public:
        WebRenderer() : Renderer(Buffer(GL_UNIFORM_BUFFER)){};
    protected:
        void initBuffers() override {
            
            vao.bind();
            baseVbo.fillData<float>(&BLOCK_FACE_VERTICES);

            baseVbo.bind();
            vao.setAttr(0,3,GL_FLOAT,4 * sizeof(float),0);
            vao.setAttr(1,1,GL_FLOAT,4 * sizeof(float),3 * sizeof(float));

            meshBuffer.bind();
            vao.setAttr(2,1,GL_FLOAT,sizeof(CHUNK_MESH_DATATYPE),0);
            GLCall( glVertexAttribDivisor(2,1) );

            chunkIDBuffer.bind();
            vao.setAttrI(3,1,GL_INT,sizeof(int),0);
            GLCall( glVertexAttribDivisor(3,1) );

            crosshairVAO.bind();
            crosshairVBO.fillData<float>(&crosshairVertices);
            crosshairVAO.setAttr(0,3,GL_FLOAT, 3 * sizeof(float), 0);

            GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
            GLCall( glBindVertexArray(0) );
            
        };
        
    public:
        void renderGame(GameState *gameState) override {
            
            this->shader.use();

            this->shader.setInt("playerState",world->player->state);
            this->shader.setMatrixFloat("projection",GL_FALSE,*(gameState->projection));
            this->shader.setMatrixFloat("view",GL_FALSE,*(gameState->view));
            this->shader.setMatrixFloat("model",GL_FALSE,*(gameState->model));
        
            this->shader.setVec3Float("LightPos",world->worldMiddle + glm::vec3(0.0,100,0.0) - world->player->camera->position);

            
            
        
            this->vao.bind();
            std::cout << "MESH: " << this->meshBuffer.getFilledDataCount() << "\n";
            GLCall( glDrawArraysInstanced(GL_TRIANGLES,0,BLOCK_FACE_VERTICES_COUNT,this->meshBuffer.getFilledDataCount()) );
            
  
        }

        void fillBuffers() override {
            unsigned long long sizeToAlloc = this->world->getWorldMeshSize();
            
            std::cout << "Allocating buffers...\n";
            this->meshBuffer.allocateBuffer(sizeToAlloc);
            this->chunkIDBuffer.allocateBuffer(sizeof(int) * (sizeToAlloc / sizeof(CHUNK_MESH_DATATYPE)));
            this->chunkStorageBuffer.allocateBuffer(sizeof(glm::vec4) * 1024); // vec4 due to std430 in shader
            std::cout << "Allocated buffers\n";
            int i=0; 
            for (Chunk* chunk : this->world->chunkRenderRefs){
                
                addChunkToBuffers(chunk, i);
                i++;
            }
            GLuint blockIndex = glGetUniformBlockIndex(shader.getProgram(), "ubo");
            glUniformBlockBinding(shader.getProgram(), blockIndex, 0);
            GLCall( glBindBufferBase(GL_UNIFORM_BUFFER, 0, this->chunkStorageBuffer.getId()) );
        }
     
};
#endif