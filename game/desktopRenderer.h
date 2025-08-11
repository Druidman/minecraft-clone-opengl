#ifndef DESKTOP_RENDERER_H
#define DESKTOP_RENDERER_H

#include "vendor/glm/glm.hpp"

#include "renderer.h"
#include "block.h"
#include "chunk.h"
#include "world.h"
#include "player.h"
#include "betterGL.h"

#include "meshBuffer.h"
#include "indirectBuffer.h"
#include "storageBuffer.h"

class DesktopRenderer : public Renderer
{
    public:
        std::string vsPath = "shaders/vertexShader.vs";
        std::string fsPath = "shaders/fragmentShader.fs";

        Shader shader = Shader(vsPath, fsPath);
        
        VertexArray vao;
        Buffer baseVbo = Buffer(GL_ARRAY_BUFFER);

        MeshBuffer meshBuffer = MeshBuffer();
        IndirectBuffer chunkDrawBuffer = IndirectBuffer();
        StorageBuffer chunkStorageBuffer = StorageBuffer();
        
    private:
        void addChunkToBuffers(Chunk *chunk){
            if (!meshBuffer.insertChunkToBuffer(chunk)){
                ExitError("DESKTOP_RENDERER","error inserting chunk to meshBuffer");
                return ;
            };
            if (!chunkDrawBuffer.insertChunkToBuffer(chunk)){
                ExitError("DESKTOP_RENDERER","error inserting chunk to indirectBuffer");
                return ;
            };
            if (!chunkStorageBuffer.insertChunkToBuffer(chunk)){
                ExitError("DESKTOP_RENDERER","error inserting chunk to storage Buffer");
                return ;
            };

            chunk->buffersSetUp = true;
        }

    protected:
        void initBuffers() override {
            this->chunkStorageBuffer.init(this->world);

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
        DesktopRenderer() : Renderer(){};
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

            this->meshBuffer.allocateDynamicBuffer(sizeToAlloc);
            this->chunkDrawBuffer.allocateDynamicBuffer(sizeof(DrawArraysIndirectCommand) * this->world->chunkRenderRefs.size());
            this->chunkStorageBuffer.allocateDynamicBuffer(sizeof(StorageBufferType) * this->world->chunkRenderRefs.size());
            for (Chunk* chunk : this->world->chunkRenderRefs){
                
                addChunkToBuffers(chunk);
            }
            
        }
};
#endif