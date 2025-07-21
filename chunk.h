#ifndef CHUNK_H
#define CHUNK_H

#include "betterGL.h"
#include "vendor/glm/glm.hpp"
#include <vector>
#include <algorithm>
#include <cmath>

#include "block.h"
#include "vertexBuffer.h"
#include "models.h"
#include "vertexArray.h"
#include "elementBuffer.h"
#include "vertexBuffer.h"



#include <optional>


class World;
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

// FORMULAS TO COPY:
// y = platform + 0.5 + chunkPosition.y
// x = column + 0.5 + (chunkPosition.x - (CHUNK_WIDTH / 2))
// z = row + 0.5 + (chunkPosition.z - (CHUNK_WIDTH / 2))

// NOW we are gonna do a lot we will have buffer of faces

struct BlockSortData
{
    float distance;
    Block block;
};
class Chunk
{

public:
    glm::vec3 position;
    World *world;
  
    VertexBuffer *vbo;

    std::vector<std::vector<std::vector<Block>>> blocks;

    std::vector<VertexDataInt> opaqueFacesData;
    std::vector<VertexDataInt> transparentFacesData;

    Chunk(glm::vec3 chunkPosition, World* world);
    
    bool isBlockTransparent(Block *block);
    
    glm::vec3 getPos(int platform, int row, int column);

    std::optional<Block *> getBlock(int plat, int row, int col, bool noneBlock = false);
    

    std::optional<Block *> getBlock(glm::vec3 positionInWorld, bool noneBlock = false);
    
    
    void addBlockFace(BlockFace face, Block *block, std::vector<VertexDataInt> *buffer);
    

    bool canAddFace(BlockFace face, Block *currentBlock);
    

    void genChunkMesh();
    
    void createBuffer(VertexBuffer *dataVbo);
    unsigned long long getMeshSize();
    void fillBuffer();
    void render();
    

    void addBlock(Block block);
    
    void addBlockPlatform();
    void addBlockRow(int platform);

    bool removeBlock(int platform, int row, int col);
    

    bool removeBlock(glm::vec3 blockPositionInWorld);

    void fillWater();
    

    void fillUnderBlock(Block &block);
    

    void addTree(glm::vec3 positionInWorld);

};


#endif
