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
#include "idBuffer.h"
#include "storageBuffer.h"
#include "gpuBuffer.h"

#include "meshBuffer.h"



class WebRenderer : public Renderer
{
    private:
        std::string vsPath = "shaders/vertexShaderWeb.vs";
        std::string fsPath = "shaders/fragmentShaderWeb.fs";

        Shader shader = Shader(vsPath, fsPath);
        
        VertexArray vao;
        GpuBuffer baseVbo = GpuBuffer(GL_ARRAY_BUFFER);

        IdBuffer chunkIdBuffer = IdBuffer();
        MeshBuffer meshBuffer = MeshBuffer();
        StorageBuffer chunkStorageBuffer = StorageBuffer(); 

        
        glm::vec3 lastCameraPosOnChunkPosChange = glm::vec3(0.0f);

    public:
        WebRenderer() : Renderer(){};
    protected:
        void initBuffers() override {
            this->chunkStorageBuffer.init(this->world);

            vao.bind();
            baseVbo.fillData<float>(&BLOCK_FACE_VERTICES);

            baseVbo.bind();
            vao.setAttr(0,3,GL_FLOAT,4 * sizeof(float),0);
            vao.setAttr(1,1,GL_FLOAT,4 * sizeof(float),3 * sizeof(float));

            meshBuffer.buffer.bind();
            vao.setAttr(2,1,GL_FLOAT,sizeof(CHUNK_MESH_DATATYPE),0);
            GLCall( glVertexAttribDivisor(2,1) );

            chunkIdBuffer.gpuBuffer.bind();
            vao.setAttrI(3,1,GL_INT,sizeof(int),0);
            GLCall( glVertexAttribDivisor(3,1) );


            crosshairVAO.bind();
            crosshairVBO.fillData<float>(&crosshairVertices);
            crosshairVAO.setAttr(0,3,GL_FLOAT, 3 * sizeof(float), 0);

            this->chunkStorageBuffer.setBindingPoint(0);
            GLuint blockIndex = glGetUniformBlockIndex(shader.getProgram(), "ubo");
            GLCall( glUniformBlockBinding(shader.getProgram(), blockIndex, 0) );

            GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
            GLCall( glBindVertexArray(0) );
            
        };
        
    public:
        virtual void updateLogs() override{
            std::cout << "\n\nDESKTOP_RENDERER_BUFFER_LOGS\n\n";
            
            
        };
        virtual void renderGame(GameState *gameState) override {
            this->shader.use();

            this->shader.setInt("playerState",world->player->state);
            this->shader.setMatrixFloat("projection",GL_FALSE,*(gameState->projection));
            this->shader.setMatrixFloat("view",GL_FALSE,*(gameState->view));
            this->shader.setMatrixFloat("model",GL_FALSE,*(gameState->model));

            this->shader.setVec3Float("LightPos",world->sunPosition - world->player->camera->position);
            this->shader.setVec3Float("CameraPos",lastCameraPosOnChunkPosChange - world->player->camera->position);

            

            this->meshBuffer.buffer.bind();
            GLint64 meshData;
            GLCall( glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &meshData) );

            this->chunkIdBuffer.gpuBuffer.bind();
            GLint64 idData;
            GLCall( glGetBufferParameteri64v(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &idData) );

            this->chunkStorageBuffer.gpuBuffer.bind();
            GLint64 storageData;
            GLCall( glGetBufferParameteri64v(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &storageData) );

            std::cout << "BUFFER_REAL_SIZE_MESH: " << meshData << "\n";
            std::cout << "BUFFER_REAL_SIZE_ID: " << idData << "\n";
            std::cout << "BUFFER_REAL_SIZE_STORAGE: " << storageData << "\n";

            std::cout << "THREADS: " << this->world->threads.size() << "\n";

            

            this->vao.bind();
        
            GLCall( 
                glDrawArraysInstanced(
                    GL_TRIANGLES,
                    0,
                    BLOCK_FACE_VERTICES_COUNT,
                    this->meshBuffer.getBufferSize() / sizeof(CHUNK_MESH_DATATYPE)
                ) 
            );
            
  
        }

        virtual void fillBuffers() override {
            std::cout << "\nFilling buffers with chunks\n";

      
            
            this->meshBuffer.allocateBuffer(
                world->getWorldOpaqueMeshSize(), world->getWorldTransparentMeshSize()
            );
            this->chunkIdBuffer.allocateBuffer(
                world->getWorldOpaqueMeshSize(), world->getWorldTransparentMeshSize()
                
            );


            WriteToLogFile("BUFFER_SIZE_INFO_IN_FILLING",
                "OpaqueSize: " + std::to_string(world->getWorldOpaqueMeshSize()) + "\n"
                "TransparentSize: " + std::to_string(world->getWorldTransparentMeshSize())
            );


            this->chunkStorageBuffer.allocateDynamicBuffer(
                UNIFORM_BUFFER_LENGTH * sizeof(StorageBufferType)
            );

            lastCameraPosOnChunkPosChange = this->world->player->camera->position;
            for (Chunk* chunk : this->world->chunkRenderRefs){
                std::cout << chunk->getMeshSize() << "\n";
                if (!addChunk(chunk)){
                    ExitError("WEB_RENDERER","can't add chunk");
                };
                
            }  
           
            fillBuffer(STORAGE_BUFFER);
            fillBuffer(INDIRECT_BUFFER);
        };
        virtual void fillBuffer(ChunkBufferType bufferToFill) override {
            BufferInt meshSize = world->getWorldMeshSize();
            switch(bufferToFill){
                case MESH_BUFFER:
                    this->meshBuffer.buffer.allocateBuffer(
                        meshSize * 1.2
                    );
                    for (std::vector< Chunk > &chunkRow : this->world->chunks){
                        for (Chunk &chunk : chunkRow){
                            addChunk(&chunk, MESH_BUFFER);
                        }
                    };
                    break;
                case STORAGE_BUFFER:
                    std::cout << "CHUNKS SIZE: " << this->world->chunks.size() << " " << this->world->chunks[0].size() << "\n";
                    std::cout << "CHUNKS SIZE: " << this->world->chunkRenderRefs.size() << "\n";
                    lastCameraPosOnChunkPosChange = this->world->player->camera->position;
                    if (!this->chunkStorageBuffer.insertChunksToBuffer(&this->world->chunkRenderRefs)){
                        ExitError("WEB_RENDERER","Filling storage buffer went wrong");
                    }
                    if (!this->chunkStorageBuffer.fillGpuBuffer()){
                        ExitError("WEB_RENDERER","Filling storage GPU buffer went wrong");
                    }

                    
                    break;
                case INDIRECT_BUFFER:
                    
                    if (!this->chunkIdBuffer.fillGpuBuffer()){
                        ExitError("WEB_RENDERER","Filling id GPU buffer went wrong");
                    }
                    break;

                case OPAQUE_ID_BUFFER:
                case TRANSPARENT_ID_BUFFER:
                case OPAQUE_MESH_BUFFER:
                case TRANSPARENT_MESH_BUFFER:
                    ExitError("WEB_RENDERER","Can't fill single part of multi buffer");
                    break;
                default:
                    ExitError("WEB_RENDERER","Unknown buffer type to fill");
                    
                    
                    
            }
            
            

        };

        virtual bool addChunk(Chunk *chunk) override { 

            std::cout << "Mesh buffer insertion...\n";
            if (!meshBuffer.insertChunkToBuffer(chunk)){
                ExitError("WEB_RENDERER","error inserting chunk to meshBuffer");
                return false;
            };
            std::cout << "Storage buffer insertion...\n";
            if (!chunkStorageBuffer.insertChunkToBuffer(chunk)){
                ExitError("WEB_RENDERER","error inserting chunk to chunkStorageBuffer");
                return false;
            };

            std::cout << "Id buffer insertion...\n";
            if (!chunkIdBuffer.insertChunkToBuffer(chunk)){
                ExitError("WEB_RENDERER","error inserting chunk to chunkIdBuffer");
                return false;
            };

     

            chunk->buffersSetUp = true;
            return true;
        }

        virtual bool updateChunk(Chunk *chunk) override {
            if (!meshBuffer.updateChunkBuffer(chunk)){
                ExitError("WEB_RENDERER","error updating chunk to meshBuffer");
                return false;
            };
            if (!chunkStorageBuffer.updateChunkBuffer(chunk)){
                ExitError("WEB_RENDERER","error updating chunk to storageBuffer");
                return false;
            };
            if (!chunkIdBuffer.updateChunkBuffer(chunk)){
                ExitError("WEB_RENDERER","error updating chunk to chunkIdBuffer");
                return false;
            };
            
            chunk->buffersSetUp = true;
            return true;
        }

        virtual bool deleteChunk(Chunk *chunk, bool merge = false) override {
            if (!meshBuffer.deleteChunkFromBuffer(chunk, merge)){
                ExitError("WEB_RENDERER","error deleting chunk from meshBuffer");
                return false;
            };
            if (!chunkStorageBuffer.deleteChunkFromBuffer(chunk, false)){
                ExitError("WEB_RENDERER","error deleting chunk from storageBuffer");
                return false;
            };
            if (!chunkIdBuffer.deleteChunkFromBuffer(chunk, merge)){
                ExitError("WEB_RENDERER","error deleting chunk to chunkIdBuffer");
                return false;
            };
           

            chunk->buffersSetUp = false;
            return true;
        }
        
        virtual bool addChunk(Chunk *chunk, ChunkBufferType bufferToUpdate) override {
            switch(bufferToUpdate){
                case MESH_BUFFER:
                    if (!meshBuffer.insertChunkToBuffer(chunk)){
                        ExitError("WEB_RENDERER","error inserting chunk to meshBuffer");
                        return false;
                    };
                    break;
                case STORAGE_BUFFER:
                    if (!chunkStorageBuffer.insertChunkToBuffer(chunk)){
                        ExitError("WEB_RENDERER","error inserting chunk to storageBuffer");
                        return false;
                    };
                    break;
                case INDIRECT_BUFFER:
                    if (!chunkIdBuffer.insertChunkToBuffer(chunk)){
                        ExitError("WEB_RENDERER","error inserting chunk to chunkIdBuffer");
                        return false;
                    };
                    break;
                case OPAQUE_ID_BUFFER:
                case TRANSPARENT_ID_BUFFER:
                case OPAQUE_MESH_BUFFER:
                case TRANSPARENT_MESH_BUFFER:
                    ExitError("WEB_RENDERER","Can't add single part of multi buffer");
                    break;
                default:
                    ExitError("WEB_RENDERER","Unknown buffer type to fill");
                    return false;
                
            }
            chunk->buffersSetUp = true;
            return true;
        }

        virtual bool updateChunk(Chunk *chunk, ChunkBufferType bufferToUpdate) override {
            switch(bufferToUpdate){
                case MESH_BUFFER:
                    if (!meshBuffer.updateChunkBuffer(chunk)){
                        ExitError("WEB_RENDERER","error updating chunk to meshBuffer");
                        return false;
                    };
                    break;
                case INDIRECT_BUFFER:
                    if (!chunkIdBuffer.updateChunkBuffer(chunk)){
                        ExitError("WEB_RENDERER","error updating chunk to chunkIdBuffer");
                        return false;
                    };
                    break;
                case STORAGE_BUFFER:
                    if (!chunkStorageBuffer.updateChunkBuffer(chunk)){
                        ExitError("WEB_RENDERER","error updating chunk to chunkStorageBuffer");
                        return false;
                    };
                    break;
                case OPAQUE_ID_BUFFER:
                case TRANSPARENT_ID_BUFFER:
                case OPAQUE_MESH_BUFFER:
                case TRANSPARENT_MESH_BUFFER:
                        ExitError("WEB_RENDERER","Can't update single part of multi buffer");
                        break;
                    default:
                        ExitError("WEB_RENDERER","Unknown buffer type to update");
                        return false;
                
            }
            chunk->buffersSetUp = true;
            return true;
        }

        virtual bool deleteChunk(Chunk *chunk, ChunkBufferType bufferToUpdate, bool merge = false) override {
            switch(bufferToUpdate){
                case MESH_BUFFER:
                    if (!meshBuffer.deleteChunkFromBuffer(chunk, merge)){
                        ExitError("WEB_RENDERER", "deleting chunk from meshBuffer");
                        return false;
                    };
                    break;

                case STORAGE_BUFFER:
                    if (!chunkStorageBuffer.deleteChunkFromBuffer(chunk, false)){
                        ExitError("WEB_RENDERER", "deleting chunk from storageBuffer");
                        return false;
                    };
                    break;
                case INDIRECT_BUFFER:
                    if (!chunkIdBuffer.deleteChunkFromBuffer(chunk, merge)){
                        ExitError("WEB_RENDERER", "deleting chunk from chunkIdBuffer");
                        return false;
                    };
                    break;
                case OPAQUE_ID_BUFFER:
                case TRANSPARENT_ID_BUFFER:
                case OPAQUE_MESH_BUFFER:
                case TRANSPARENT_MESH_BUFFER:
                    ExitError("WEB_RENDERER","Can't delete single part of multi buffer");
                    break;
                default:
                    ExitError("WEB_RENDERER","Unknown buffer type to delete");
                    return false;
                
            }
            chunk->buffersSetUp = false;
            return true;
        }
        
};
#endif

