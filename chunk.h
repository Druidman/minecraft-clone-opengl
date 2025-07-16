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


// NOW we are gonna do a lot we will have buffer of faces

struct BlockSortData{
    float distance;
    Block block;
};
class Chunk{
        
    public:
        glm::vec3 position;
        glm::vec3* cameraPos;
        
        std::vector< std::vector< Block > > platform = std::vector< std::vector< Block > >(CHUNK_WIDTH, std::vector< Block >(CHUNK_WIDTH,Block(NONE_BLOCK,glm::vec3(1.0f))));

        std::vector< std::vector< std::vector< Block > > > blocks = std::vector< std::vector< std::vector< Block > > >(100,platform);
            
        VertexBuffer vboInst;
   

        std::vector<VertexDataInt> blockFaceVertices;


        Chunk(glm::vec3 chunkPosition, VertexBuffer vboInst, glm::vec3 *cameraPos){
            this->position = chunkPosition;
            
            this->vboInst = vboInst;
           
            this->cameraPos = cameraPos;
    
        };
        
        void genBlockFacesVertices(Block block){
            glm::vec3 blockPositionChunk = block.position - position + glm::vec3(CHUNK_WIDTH / 2, 0.0, CHUNK_WIDTH / 2);
            for (uint8_t face =0; face < 6; face++){
                int textId;
                if (face == 0){ //top
                    textId = block.type * 4;
                }
                else if (face == 1){ //bottom
                    textId = block.type * 4 + 2;
                }
                else { //middle
                    textId = block.type * 4 + 1;
                }

                int zCoord = blockPositionChunk.z - 0.5;
                int yCoord = blockPositionChunk.y - 0.5;
                int xCoord = blockPositionChunk.x - 0.5;
                VertexDataInt data = 0;

                data |= ((VertexDataInt)(textId & 127)) << 19;   // TTTTTTT: 7 bitów
                data |= ((VertexDataInt)(face & 7)) << 16;      // NNN: 3 bity
                data |= ((VertexDataInt)(xCoord & 15)) << 12;      // XXXX: 4 bity
                data |= ((VertexDataInt)(yCoord & 255)) << 4;      // YYYYYYYY: 8 bitów
                data |= ((VertexDataInt)(zCoord & 15));            // ZZZZ: 4 bity
                        
                blockFaceVertices.push_back(data);    
                if (block.type == WATER && !face){
                    break;;
                }
            }

            

            
        }
        
        void renderOpaque(){
            vboInst.fillData< VertexDataInt >(blockFaceVertices);
            glDrawElementsInstanced(GL_TRIANGLES, BLOCK_FACE_INDICES.size(), GL_UNSIGNED_INT, 0,blockFaceVertices.size());
        };

        
        
        void addBlock(Block block){
            if (block.type == NONE_BLOCK){
                return;
            }
            if (block.position.x > this->position.x + CHUNK_WIDTH || 
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
                int platformsToAdd = platform - this->blocks.size() + 2;
                for (int i=0; i<platformsToAdd; i++){
                    this->blocks.push_back(std::vector< std::vector< Block > >(CHUNK_WIDTH, std::vector< Block >(CHUNK_WIDTH,Block(NONE_BLOCK,glm::vec3(1.0f)))));
                }
                
            } 
            this->blocks[platform][row][column] = block;
            genBlockFacesVertices(block);
        }
        void removeBlock(int platform, int row, int col){
            blocks[platform][row][col].type = NONE_BLOCK;
        }
        void fillWater(){
            int platformI = std::min(20,(int)blocks.size());
            for (int row =0; row < blocks[platformI].size();row++){
                for (int col =0; col < blocks[platformI][row].size();col++){
                    Block &block = blocks[platformI][row][col];
                    if (block.type != NONE_BLOCK){
                        continue;
                    }
                    block.type = WATER;
                    block.position.y = platformI + 0.5 + position.y;
                    block.position.x = col + 0.5 + (position.x - (CHUNK_WIDTH / 2));
                    block.position.z  = row + 0.5 + (position.z - (CHUNK_WIDTH / 2));
                }
            }
            blockFaceVertices.clear();
            for (int platform =0; platform < blocks.size(); platform++){

                for (int row =0; row < blocks[platform].size(); row++){

                    for (int col =0; col < blocks[platform][row].size();col++){
                        Block &block = blocks[platform][row][col];
                        
                        addBlock(block);
                        
                        
                        
                        
                    }
                }
            }
            
        }

};

#endif

