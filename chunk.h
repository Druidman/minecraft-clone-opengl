#ifndef CHUNK_H
#define CHUNK_H

#include "betterGL.h"
#include "vendor/glm/glm.hpp"
#include <vector>

#include "block.h"
#include "vertexBuffer.h"

const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 256;

class Chunk{

    public:
        glm::vec3 position;
        std::vector< Block > blocks;
        VertexBuffer vboPos;
        VertexBuffer vboTex;

        float *blockPositions = genBlockPositions();
        float *blockUVs = genBlockUVs();
        

        Chunk(glm::vec3 chunkPosition, VertexBuffer vboPositions, VertexBuffer vboTextureUVs){
            this->position = chunkPosition;
            this->vboPos = vboPositions;
            this->vboTex = vboTextureUVs;

    

        };
        float* genBlockPositions(){
            float* blockPositions = new float[this->blocks.size() * 3];

            
            for (int i = 0, ind = 0; i<this->blocks.size(); i++, ind += 3){
                blockPositions[ind] = blocks[i].position.x;
                blockPositions[ind + 1] = blocks[i].position.y;
                blockPositions[ind + 2] = blocks[i].position.z;
            }

            return blockPositions;
        }
        float* genBlockUVs(){
            float* blockUVs = new float[this->blocks.size() * 2];

            for (int i = 0, ind = 0; i<this->blocks.size(); i++, ind += 2){
                blockUVs[ind] = 0;
                blockUVs[ind + 1] = -(blocks[i].type / 4.0);
            }

        

            return blockUVs;
        }
        
        void render(){
            
            
            vboPos.fillData<float>(blockPositions,blocks.size() * 3);
            vboTex.fillData<float>(blockUVs,blocks.size() * 2);

            glDrawElementsInstanced(GL_TRIANGLES, BLOCK_INDICIES_COUNT, GL_UNSIGNED_INT, 0, blocks.size());

        };
        void addBlock(Block block){
            if (block.position.x > this->position.x + CHUNK_WIDTH - 1 || 
                block.position.x < this->position.x - CHUNK_WIDTH + 1 ||
                block.position.z > this->position.z + CHUNK_WIDTH - 1 ||
                block.position.z < this->position.z - CHUNK_WIDTH + 1 ||
                block.position.y > CHUNK_HEIGHT - 1 //|| block.position.y < 0
            ){
                ExitError("CHUNK","adding block to chunk outside its borders");
                return ;
            }
            
            this->blocks.push_back(block);
            blockPositions = genBlockPositions();
            blockUVs = genBlockUVs();

        }
        void setBlocksTo(BlockType type){
            for (int i =0; i < blocks.size(); i++){
                this->blocks[i].type = type;
            }
            this->blockUVs = genBlockUVs();

        }


};

#endif
