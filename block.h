#ifndef BLOCK_H
#define BLOCK_H
#include "vendor/glm/glm.hpp"
enum BlockType{
    DIRT, GRASS, STONE, COBBLESTONE, WOOD
};
float blockVertices[120] = {
    // Top face (Y+)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.25f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,

    // Bottom face (Y-)
    -0.5f, -0.5f, -0.5f,  0.5f, 1.0f,  // 4
     0.5f, -0.5f, -0.5f,  0.75f, 1.0f,  // 5
     0.5f, -0.5f,  0.5f,  0.75f, 0.0f,  // 6
    -0.5f, -0.5f,  0.5f,  0.5f, 0.0f,  // 7

    // Front face (Z+)
    -0.5f, -0.5f,  0.5f,  0.25f, 0.0f,  // 8
     0.5f, -0.5f,  0.5f,  0.5f, 0.0f,   // 9
     0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   // 10
    -0.5f,  0.5f,  0.5f,  0.25f, 1.0f,  // 11

    // Back face (Z-)
    -0.5f, -0.5f, -0.5f,  0.5f, 0.0f,  // 12
     0.5f, -0.5f, -0.5f,  0.25f, 0.0f,   // 13
     0.5f,  0.5f, -0.5f,  0.25f, 1.0f,   // 14
    -0.5f,  0.5f, -0.5f,  0.5f, 1.0f,  // 15

    // Left face (X-)
    -0.5f, -0.5f, -0.5f,  0.25f, 0.0f,  // 16
    -0.5f, -0.5f,  0.5f,  0.5f, 0.0f,   // 17
    -0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   // 18
    -0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  // 19

    // Right face (X+)
     0.5f, -0.5f, -0.5f,  0.5f, 0.0f,  // 20
     0.5f, -0.5f,  0.5f,  0.25f, 0.0f,   // 21
     0.5f,  0.5f,  0.5f,  0.25f, 1.0f,   // 22
     0.5f,  0.5f, -0.5f,  0.5f, 1.0f   // 23
};

int blockIndicies[36] = {
    0, 1, 2, 2, 3, 0,        // Top
    4, 5, 6, 6, 7, 4,        // Bottom
    8, 9,10,10,11, 8,        // Front
   12,13,14,14,15,12,        // Back
   16,17,18,18,19,16,        // Left
   20,21,22,22,23,20         // Right
};

class Block{

    public:
        inline Block(BlockType blockType, glm::vec3 blockPosition){this->type = blockType; this->position = position;};
        BlockType type;
        glm::vec3 position;
};

#endif