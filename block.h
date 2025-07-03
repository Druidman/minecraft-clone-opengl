#ifndef BLOCK_H
#define BLOCK_H
#include "vendor/glm/glm.hpp"
enum BlockType{
    GRASS_DIRT, SAND, STONE, WATER
};

const int BLOCK_VERTICES_COUNT = 192;
float blockVertices[BLOCK_VERTICES_COUNT] = {
    // Top face (Y+)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 0.75f, 0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.75f,  0.0f, 1.0f, 0.0f,

    // Bottom face (Y-)
    -0.5f, -0.5f, -0.5f,  0.5f, 1.0f,   0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.75f, 1.0f,  0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.75f, 0.75f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.5f, 0.75f,  0.0f, -1.0f, 0.0f,

    // Front face (Z+)
    -0.5f, -0.5f,  0.5f,  0.25f, 0.75f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.5f, 0.75f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.25f, 1.0f,  0.0f, 0.0f, 1.0f,

    // Back face (Z-)
    -0.5f, -0.5f, -0.5f,  0.5f, 0.75f,  0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.25f, 0.75f, 0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.5f, 1.0f,   0.0f, 0.0f, -1.0f,

    // Left face (X-)
    -0.5f, -0.5f, -0.5f,  0.25f, 0.75f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.5f, 0.75f,  -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  -1.0f, 0.0f, 0.0f,

    // Right face (X+)
     0.5f, -0.5f, -0.5f,  0.5f, 0.75f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.25f, 0.75f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 1.0f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.5f, 1.0f,   1.0f, 0.0f, 0.0f
};

const int BLOCK_INDICIES_COUNT = 36;
unsigned int blockIndicies[BLOCK_INDICIES_COUNT] = {
    0, 1, 2, 2, 3, 0,        // Top
    4, 5, 6, 6, 7, 4,        // Bottom
    8, 9,10,10,11, 8,        // Front
   12,13,14,14,15,12,        // Back
   16,17,18,18,19,16,        // Left
   20,21,22,22,23,20         // Right
};

class Block{

    public:
        Block(BlockType blockType, glm::vec3 blockPosition){this->type = blockType; this->position = blockPosition;};
        BlockType type;
        glm::vec3 position;
};

#endif