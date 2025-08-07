#ifndef DESKTOP_RENDERER_H
#define DESKTOP_RENDERER_H

#include "vendor/glm/glm.hpp"

#include "renderer.h"
#include "block.h"
#include "chunk.h"
#include "world.h"
#include "player.h"
#include "betterGL.h"

class DesktopRenderer : public Renderer
{
    public:
        std::string vsPath = "shaders/vertexShader.vs";
        std::string fsPath = "shaders/fragmentShader.fs";

        Shader shader = Shader(vsPath, fsPath);
        
        VertexArray vao;
        Buffer baseVbo = Buffer(GL_ARRAY_BUFFER);
        Buffer meshBuffer = Buffer(GL_ARRAY_BUFFER);
        Buffer chunkDrawBuffer = Buffer(GL_DRAW_INDIRECT_BUFFER);
    private:
        void addChunkToBuffers(Chunk *chunk){
            DrawArraysIndirectCommand data = {
                BLOCK_FACE_VERTICES_COUNT,
                (uint)chunk->transparentMesh.size() + (uint)chunk->opaqueMesh.size(),
                0,
                (uint)(this->meshBuffer.getFilledDataSize() / sizeof(CHUNK_MESH_DATATYPE))
            };
            this->chunkDrawBuffer.addData<DrawArraysIndirectCommand>(data);

            // vec4 due to std430 in shader
            // this approach passes regular chunk coord to buffer but if we place cam always at 0,0,0 then it won't work
            // this->chunkStorageBuffer->addData<glm::vec4>(glm::vec4(chunk->position,0.0)); 
            // SO we shift chunks pos by camera position
            this->chunkStorageBuffer.addData<glm::vec4>(glm::vec4(chunk->position - this->world->player->camera->position,0.0)); 

            this->meshBuffer.addData< CHUNK_MESH_DATATYPE >(chunk->getOpaqueMesh());
            this->meshBuffer.addData< CHUNK_MESH_DATATYPE >(chunk->getTransparentMesh());
        }

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

            crosshairVAO.bind();
            crosshairVBO.fillData<float>(&crosshairVertices);
            crosshairVAO.setAttr(0,3,GL_FLOAT, 3 * sizeof(float), 0);

            GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
            GLCall( glBindVertexArray(0) );
            
        };
    public:
        DesktopRenderer() : Renderer(Buffer(GL_SHADER_STORAGE_BUFFER)){};
    public:
        void renderGame(GameState *gameState) override {
            this->shader.use();

            this->shader.setInt("playerState",world->player->state);
            this->shader.setMatrixFloat("projection",GL_FALSE,*(gameState->projection));
            this->shader.setMatrixFloat("view",GL_FALSE,*(gameState->view));
            this->shader.setMatrixFloat("model",GL_FALSE,*(gameState->model));

            this->shader.setVec3Float("LightPos",world->sunPosition - world->player->camera->position);
            


            this->vao.bind();
            GLCall( glMultiDrawArraysIndirect(GL_TRIANGLES,0,this->world->chunkRenderRefs.size() ,sizeof(DrawArraysIndirectCommand)) );
        }

        void fillBuffers() override {
            unsigned long long sizeToAlloc = this->world->getWorldMeshSize();

            this->meshBuffer.allocateBuffer(sizeToAlloc);
            this->chunkDrawBuffer.allocateBuffer(sizeof(DrawArraysIndirectCommand) * this->world->chunkRenderRefs.size());
            this->chunkStorageBuffer.allocateBuffer(sizeof(glm::vec4) * this->world->chunkRenderRefs.size()); // vec4 due to std430 in shader
            for (Chunk* chunk : this->world->chunkRenderRefs){
                
                addChunkToBuffers(chunk);
            }
            GLCall( glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->chunkStorageBuffer.getId()) );
        }
   
};
#endif