#ifndef BLOCK_H
#define BLOCK_H
#include "vendor/glm/glm.hpp"
#include <vector>
#include <cstdint>


typedef float VertexDataInt;
struct Vertex{
    glm::vec3 position;
    glm::vec2 uvCoord;
    glm::vec3 normalVector;
};

enum BlockType{
    GRASS_DIRT, SAND, STONE, WATER, WOOD, LEAF, NONE_BLOCK
};
enum BlockFace {
    TOP_FACE, BOTTOM_FACE, FRONT_FACE, BACK_FACE, LEFT_FACE, RIGHT_FACE
};
struct FaceData{
    glm::vec3 pos;
    glm::vec3 normal;
    
};
const int BLOCK_WIDTH = 1;
const int BLOCK_VERTICES_COUNT = 192;
const int BLOCK_INDICIES_COUNT = 36;

const std::vector< FaceData > FACES_DATA = {
    {glm::vec3(0.0,0.5,0.0) , glm::vec3(0.0f, 1.0f, 0.0f)},//top
    {glm::vec3(0.0,-0.5,0.0), glm::vec3(0.0f, -1.0f, 0.0f)},//bottom
    {glm::vec3(0.0,0.0,0.5) , glm::vec3(0.0f, 0.0f, 1.0f)},//front
    {glm::vec3(0.0,0.0,-0.5), glm::vec3(0.0f, 0.0f, -1.0f)},//back
    {glm::vec3(-0.5,0.0,0.0), glm::vec3(-1.0f, 0.0f, 0.0f)},//left
    {glm::vec3(0.5,0.0,0.0) , glm::vec3(1.0f, 0.0f, 0.0f)},//right
};

const glm::vec3 TOP_FACE_POS = glm::vec3(0.0,0.5,0.0);
const glm::vec3 BOTTOM_FACE_POS = glm::vec3(0.0,-0.5,0.0);

const glm::vec3 FRONT_FACE_POS = glm::vec3(0.0,0.0,0.5);
const glm::vec3 BACK_FACE_POS = glm::vec3(0.0,0.0,-0.5);

const glm::vec3 LEFT_FACE_POS = glm::vec3(-0.5,0.0,0.0);
const glm::vec3 RIGHT_FACE_POS = glm::vec3(0.5,0.0,0.0);

const std::vector<float> BLOCK_FACE_VERTEX_POS = {
    -0.5f,  0.0f, -0.5f, 1,               // 0.0, 1.0,  top left
    -0.5f,  0.0f,  0.5f, 0,               // 0.0, 0.75, bottom left
     0.5f,  0.0f,  0.5f, 3,               // 0.25, 0.75 bottom right
    -0.5f,  0.0f, -0.5f, 1,               // 0.0, 1.0,  top left
     0.5f,  0.0f,  0.5f, 3,               // 0.25, 0.75 bottom right
     0.5f,  0.0f, -0.5f, 2                // 0.25, 1.0, top right

};
const int BLOCK_FACE_VERTICES_COUNT = 6;

const std::vector<float> TOPFACEVERTICES = {
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 0.75f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 0.75f, 0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.75f,  0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f
};

const std::vector<float> BOTTOMFACEVERTICES = {
    -0.5f, -0.5f, -0.5f,  0.5f, 1.0f,   0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  0.75f, 1.0f,  0.0f, -1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.75f, 0.75f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.75f, 0.75f, 0.0f, -1.0f, 0.0f,
   -0.5f, -0.5f,  0.5f,  0.5f, 0.75f,  0.0f, -1.0f, 0.0f,
   -0.5f, -0.5f, -0.5f,  0.5f, 1.0f,   0.0f, -1.0f, 0.0f
};
const std::vector<float> FRONTFACEVERTICES = {
    -0.5f, -0.5f,  0.5f,  0.25f, 0.75f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.5f, 0.75f,  0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.25f, 1.0f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.25f, 0.75f, 0.0f, 0.0f, 1.0f
};
const std::vector<float> BACKFACEVERTICES = {
    -0.5f, -0.5f, -0.5f,  0.5f, 0.75f,  0.0f, 0.0f, -1.0f,
    0.5f, -0.5f, -0.5f,  0.25f, 0.75f, 0.0f, 0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  0.0f, 0.0f, -1.0f,
    0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  0.0f, 0.0f, -1.0f,
   -0.5f,  0.5f, -0.5f,  0.5f, 1.0f,   0.0f, 0.0f, -1.0f,
   -0.5f, -0.5f, -0.5f,  0.5f, 0.75f,  0.0f, 0.0f, -1.0f
};
const std::vector<float> LEFTFACEVERTICES = {
    -0.5f, -0.5f, -0.5f,  0.25f, 0.75f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.5f, 0.75f,  -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.5f, 1.0f,   -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.25f, 0.75f, -1.0f, 0.0f, 0.0f

};
const std::vector<float> RIGHTFACEVERTICES = {
    0.5f, -0.5f, -0.5f,  0.5f, 0.75f,  1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.25f, 0.75f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 1.0f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 1.0f,  1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.5f, 1.0f,   1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.5f, 0.75f,  1.0f, 0.0f, 0.0f

};
const std::vector<std::vector<float>> FACEVERTICES = {
    TOPFACEVERTICES, BOTTOMFACEVERTICES, FRONTFACEVERTICES,BACKFACEVERTICES, LEFTFACEVERTICES, RIGHTFACEVERTICES  
};



const std::vector<float> blockVertices = {
    // Top face (Y+)
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.25f, 0.75f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  0.0f, 1.0f, 0.0f,
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
     0.5f,  0.5f, -0.5f,  0.25f, 1.0f,  0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.25f, 0.75f, 0.0f, 0.0f, -1.0f,
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

const unsigned int blockIndicies[BLOCK_INDICIES_COUNT] = {
    0, 1, 2, 1, 0, 3,        // Top
    4, 5, 6, 6, 7, 4,        // Bottom
    8, 9,10,10,11, 8,        // Front
   12,13,14,13,12,15,        // Back
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