#ifndef CHUNK_H
#define CHUNK_H


#include <vector>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <optional>


#include "betterGL.h"
#include "vendor/glm/glm.hpp"

#include "block.h"
#include "models.h"




class World;
typedef FaceData CHUNK_MESH_DATATYPE;
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

class Chunk
{

public:
    

    glm::vec3 position;
    World *world;

    std::vector<std::vector<std::vector<Block>>> blocks;

    std::vector<CHUNK_MESH_DATATYPE> opaqueMesh;
    std::vector<CHUNK_MESH_DATATYPE> transparentMesh;

    bool chunkReady = false;
    bool renderReady = false;

public:
    Chunk(glm::vec3 chunkPosition, World* world);
    

    glm::vec3 getPositionInWorld(int platform, int row, int column);
    std::optional<Block *> getBlock(int plat, int row, int col, bool noneBlock = false);
    std::optional<Block *> getBlock(glm::vec3 positionInWorld, bool noneBlock = false);

    void addBlockFace(Face face, Block *block, std::vector<CHUNK_MESH_DATATYPE> *buffer);
    bool canAddBlockFace(Face face, Block *currentBlock);
    bool isInChunkBorder(Block &block);
    

    void genChunk();
    void genChunkMesh();
    void meshBlock(int platform, int row, int col);
    unsigned long long getMeshSize();
    std::vector<CHUNK_MESH_DATATYPE>* getOpaqueMesh(){return &opaqueMesh;};
    std::vector<CHUNK_MESH_DATATYPE>* getTransparentMesh(){return &transparentMesh;};


    bool addBlock(Block &block);
    bool updateBlock(Block &block);

    void addBlockPlatform();
    void addBlockRow(int platform);
    
    bool removeBlock(int platform, int row, int col);
    bool removeBlock(glm::vec3 blockPositionInWorld);


    void fillWater();
    void fillUnderBlock(Block &block);


    void addTree(glm::vec3 positionInWorld);

};


#endif
