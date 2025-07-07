#ifndef CHUNK_H
#define CHUNK_H

#include "betterGL.h"
#include "vendor/glm/glm.hpp"
#include <vector>
#include <algorithm>

#include "block.h"
#include "vertexBuffer.h"


const int CHUNK_WIDTH = 16;
const int CHUNK_HEIGHT = 256;

// block system works that first platform is bottom platform of blocks.
// We will be indexing it in a way that platform index corresponds to block y coord
// Block x coord will correspond to column
// Block z coord will correspond to row
//
// To summarize:
//  - 3D array of blocks 
//  - PLATFORM is blocks y coord as: y = platform + 0.5 + chunkPosition.y
//  - COLUMN is blocks x coord as: x = column + 0.5 + (chunkPosition.x - (CHUNK_WIDTH / 2))
//  - ROW is blocks z coord as: z = row + 0.5 + (chunkPosition.z - (CHUNK_WIDTH / 2))

struct BlockSortData{
    float distance;
    glm::vec3 position;
};
class Chunk{
        
    public:
        glm::vec3 position;
        
        std::vector< std::vector< Block > > platform = std::vector< std::vector< Block > >(CHUNK_WIDTH, std::vector< Block >(CHUNK_WIDTH,Block(NONE_BLOCK,glm::vec3(1.0f))));

        std::vector< std::vector< std::vector< Block > > > blocks = std::vector< std::vector< std::vector< Block > > >(100,platform);
            
        VertexBuffer vboPos;
        VertexBuffer vboTex;

        std::vector<float> blockPositions = genBlockPositions();
        std::vector<float> blockUVs = genBlockUVs();
        

        Chunk(glm::vec3 chunkPosition, VertexBuffer vboPositions, VertexBuffer vboTextureUVs){
            this->position = chunkPosition;
            this->vboPos = vboPositions;
            this->vboTex = vboTextureUVs;
        };

        void update(){
            this->blockUVs = genBlockUVs();
            this->blockPositions = genBlockPositions();
        }
        std::vector<float> genBlockPositions(){
            std::vector<float> blockPositionsN;

            

            
            for (int i = 0; i<this->blocks.size(); i++){
                for (int j = 0; j<this->blocks[i].size(); j++){
                    for (int x = 0; x<this->blocks[i][j].size(); x++){
                        Block block = blocks[i][j][x];
                        if (block.type != NONE_BLOCK && block.type != WATER){
                            blockPositionsN.push_back(block.position.x);
                            blockPositionsN.push_back(block.position.y);
                            blockPositionsN.push_back(block.position.z);
                        }
                    }
                }
            }
            //transparent
            std::vector<BlockSortData> distances;
            for (int i = 0; i<this->blocks.size(); i++){
                for (int j = 0; j<this->blocks[i].size(); j++){
                    for (int x = 0; x<this->blocks[i][j].size(); x++){
                        Block block = blocks[i][j][x];
                        if (block.type == WATER){
                            blockPositionsN.push_back(block.position.x);
                            blockPositionsN.push_back(block.position.y);
                            blockPositionsN.push_back(block.position.z);
                        }
                    }
                }
            }


            return blockPositionsN;
        }
        std::vector<float> genBlockUVs(){
            std::vector<float> blockUVsN;

            

            for (int i = 0; i<this->blocks.size(); i++){
                for (int j = 0; j<this->blocks[i].size(); j++){
                    for (int x = 0; x<this->blocks[i][j].size(); x++){
                        if (blocks[i][j][x].type != NONE_BLOCK && blocks[i][j][x].type != WATER){
                            blockUVsN.push_back(0);
                            blockUVsN.push_back(-(blocks[i][j][x].type / 4.0));
                        }
                        
                    }
                }
            }
            for (int i = 0; i<this->blocks.size(); i++){
                for (int j = 0; j<this->blocks[i].size(); j++){
                    for (int x = 0; x<this->blocks[i][j].size(); x++){
                        Block block = blocks[i][j][x];
                        if (block.type == WATER){
                            blockUVsN.push_back(0);
                            blockUVsN.push_back(-(blocks[i][j][x].type / 4.0));
                        }
                    }
                }
            }

        

            return blockUVsN;
        }
        
        void render(){
            
           

            vboPos.fillData<float>(&blockPositions[0],blockPositions.size());
            vboTex.fillData<float>(&blockUVs[0],blockUVs.size());

            glDrawElementsInstanced(GL_TRIANGLES, BLOCK_INDICIES_COUNT, GL_UNSIGNED_INT, 0,blockPositions.size() / 3 );

        };
        void addBlock(Block block){
            
            if (block.position.x > this->position.x + (CHUNK_WIDTH/2) || 
                block.position.x < this->position.x - CHUNK_WIDTH  ||
                block.position.z > this->position.z + CHUNK_WIDTH ||
                block.position.z < this->position.z - CHUNK_WIDTH ||
                block.position.y < this->position.y
            ){
                ExitError("CHUNK","adding block to chunk outside its borders");
                return ;
            }
            int platform = block.position.y - 0.5 - position.y;
            int column = block.position.x - 0.5 - (position.x - (CHUNK_WIDTH / 2));
            int row =  block.position.z - 0.5 - (position.z - (CHUNK_WIDTH / 2));

            
            if (row > CHUNK_WIDTH - 1 || 
                column > CHUNK_WIDTH - 1 ||
                row < 0 ||
                column < 0 ||
                platform < 0
            )
            {
                ExitError("CHUNK","invalid row or column");
                return;
            }
          
            if (platform > this->blocks.size() - 1){
                int platformsToAdd = platform - this->blocks.size() + 1;
                for (int i=0; i<platformsToAdd; i++){
                    this->blocks.push_back(std::vector< std::vector< Block > >(CHUNK_WIDTH, std::vector< Block >(CHUNK_WIDTH,Block(NONE_BLOCK,glm::vec3(1.0f)))));
                }
                
            } 
            this->blocks[platform][row][column] = block;
          

        }
        void fillWater(){
            for (int platform = 0; platform < std::min(30, (int)blocks.size()); platform++){
                for (int row = 0; row< blocks[platform].size(); row++){
                    for (int col = 0; col< blocks[platform][row].size(); col++){
                        if (blocks[platform][row][col].type == NONE_BLOCK){
                            glm::vec3 blockPos = glm::vec3(col,platform,row) + glm::vec3(0.5,0.5,0.5) + position - glm::vec3(CHUNK_WIDTH / 2, 0.0, CHUNK_WIDTH / 2);
                            blocks[platform][row][col] = Block(WATER,blockPos);
                        }
                    }   
                }
            }
            update();
        }

};

#endif
