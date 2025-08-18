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
        
        glm::vec3 lastCameraPosOnChunkPosChange = glm::vec3(0.0f);

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
            this->shader.setVec3Float("CameraPos",lastCameraPosOnChunkPosChange - world->player->camera->position);
            


            this->vao.bind();
            GLCall( glMultiDrawArraysIndirect(GL_TRIANGLES,0,this->world->chunkRenderRefs.size() ,sizeof(DrawArraysIndirectCommand)) );
        }

        virtual void fillBuffers() override {
            std::cout << "\nFilling buffers with chunks\n";

            BufferInt meshSize = world->getWorldMeshSize();
            
            this->meshBuffer.allocateDynamicBuffer(
                meshSize
            );

           
            for (std::vector< Chunk > &chunkRow : this->world->chunks){
                for (Chunk &chunk : chunkRow){
                    addChunk(&chunk);
                }
               
            };

            lastCameraPosOnChunkPosChange = this->world->player->camera->position;
            this->chunkDrawBuffer.fillBufferWithChunks(&this->world->chunkRenderRefs);
            this->chunkStorageBuffer.fillBufferWithChunks(&this->world->chunkRenderRefs);

            this->chunkStorageBuffer.setBindingPoint(3);

            

        };
        virtual void fillBuffer(BufferType bufferToFill) override {
            // std::cout << "\nFilling buffer " << bufferToFill << " with chunks\n";


            BufferInt meshSize = world->getWorldMeshSize();
            switch(bufferToFill){
                case MESH_BUFFER:
                    this->meshBuffer.allocateDynamicBuffer(
                        meshSize
                    );
                    for (std::vector< Chunk > &chunkRow : this->world->chunks){
                        for (Chunk &chunk : chunkRow){
                            addChunk(&chunk);
                        }
                    };
                    break;
                case INDIRECT_BUFFER:
                    this->chunkDrawBuffer.fillBufferWithChunks(&this->world->chunkRenderRefs);
                    break;
                case STORAGE_BUFFER:
                    lastCameraPosOnChunkPosChange = this->world->player->camera->position;
                    this->chunkStorageBuffer.fillBufferWithChunks(&this->world->chunkRenderRefs);
                    break;
            }
            
            

        };

        virtual bool addChunk(Chunk *chunk) override {

            
            if (!meshBuffer.insertChunkToBuffer(chunk)){
                ExitError("DESKTOP_RENDERER","error inserting chunk to meshBuffer");
                return false;
            };
            // if (!chunkDrawBuffer.insertChunkToBuffer(chunk)){
            //     ExitError("DESKTOP_RENDERER","error inserting chunk to indirectBuffer");
            //     return false;
            // };
            // if (!chunkStorageBuffer.insertChunkToBuffer(chunk)){
            //     ExitError("DESKTOP_RENDERER","error inserting chunk to storage Buffer");
            //     return false;
            // };

            chunk->buffersSetUp = true;
            return true;
        }

        virtual bool updateChunk(Chunk *chunk) override {
            if (!meshBuffer.updateChunkBuffer(chunk)){
                ExitError("DESKTOP_RENDERER","error updating chunk to meshBuffer");
                return false;
            };
            // if (!chunkDrawBuffer.updateChunkBuffer(chunk)){
            //     ExitError("DESKTOP_RENDERER","error updating chunk to indirectBuffer");
            //     return false;
            // };
            // if (!chunkStorageBuffer.updateChunkBuffer(chunk)){
            //     ExitError("DESKTOP_RENDERER","error updating chunk to storage Buffer");
            //     return false;
            // };

            chunk->buffersSetUp = true;
            return true;
        }

        virtual bool deleteChunk(Chunk *chunk, bool merge = false) override {
            if (!meshBuffer.deleteChunkFromBuffer(chunk, merge)){
                ExitError("DESKTOP_RENDERER","error deleting chunk from meshBuffer");
                return false;
            };
            // if (!chunkDrawBuffer.deleteChunkFromBuffer(chunk)){
            //     ExitError("DESKTOP_RENDERER","error deleting chunk from indirectBuffer");
            //     return false;
            // };
            // if (!chunkStorageBuffer.deleteChunkFromBuffer(chunk)){
            //     ExitError("DESKTOP_RENDERER","error deleting chunk from storage Buffer");
            //     return false;
            // };

            chunk->buffersSetUp = false;
            return true;
        }
        
        virtual bool addChunk(Chunk *chunk, BufferType bufferToUpdate) override {
            switch(bufferToUpdate){
                case MESH_BUFFER:
                    if (!meshBuffer.insertChunkToBuffer(chunk)){
                        ExitError("DESKTOP_RENDERER","error inserting chunk to meshBuffer");
                        return false;
                    };
                    break;
                // case INDIRECT_BUFFER:
                //     if (!chunkDrawBuffer.insertChunkToBuffer(chunk)){
                //         ExitError("DESKTOP_RENDERER","error inserting chunk to indirectBuffer");
                //         return false;
                //     };
                //     break;
                // case STORAGE_BUFFER:
                //     if (!chunkStorageBuffer.insertChunkToBuffer(chunk)){
                //         ExitError("DESKTOP_RENDERER","error inserting chunk to storage Buffer");
                //         return false;
                //     };
                //     break;
            }
            chunk->buffersSetUp = true;
            return true;
        }

        virtual bool updateChunk(Chunk *chunk, BufferType bufferToUpdate) override {
            switch(bufferToUpdate){
                case MESH_BUFFER:
                    if (!meshBuffer.updateChunkBuffer(chunk)){
                        ExitError("DESKTOP_RENDERER","error updating chunk to meshBuffer");
                        return false;
                    };
                    break;
                // case INDIRECT_BUFFER:
                //     if (!chunkDrawBuffer.updateChunkBuffer(chunk)){
                //         ExitError("DESKTOP_RENDERER","error updating chunk to indirectBuffer");
                //         return false;
                //     };
                //     break;
                // case STORAGE_BUFFER:
                //     if (!chunkStorageBuffer.updateChunkBuffer(chunk)){
                //         ExitError("DESKTOP_RENDERER","error updating chunk to storage Buffer");
                //         return false;
                //     };
                //     break;
            }
            chunk->buffersSetUp = true;
            return true;
        }

        virtual bool deleteChunk(Chunk *chunk, BufferType bufferToUpdate, bool merge = false) override {
            switch(bufferToUpdate){
                case MESH_BUFFER:
                    if (!meshBuffer.deleteChunkFromBuffer(chunk, merge)){
                        ExitError("DESKTOP_RENDERER","error deleting chunk to meshBuffer");
                        return false;
                    };
                    break;
                // case INDIRECT_BUFFER:
                //     if (!chunkDrawBuffer.deleteChunkFromBuffer(chunk)){
                //         ExitError("DESKTOP_RENDERER","error deleting chunk to indirectBuffer");
                //         return false;
                //     };
                //     break;
                // case STORAGE_BUFFER:
                //     if (!chunkStorageBuffer.deleteChunkFromBuffer(chunk)){
                //         ExitError("DESKTOP_RENDERER","error deleting chunk to storage Buffer");
                //         return false;
                //     };
                //     break;
            }
            chunk->buffersSetUp = false;
            return true;
        }
        
};
#endif
