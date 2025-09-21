#ifndef BLOCK_H
#define BLOCK_H
#include "vendor/glm/glm.hpp"
#include <vector>


typedef float FaceData;


enum BlockType{
    GRASS_DIRT, SAND, STONE, WATER, WOOD, LEAF, EMPTY_LEAF, SNOW, NONE_BLOCK
};
enum Face {
    TOP_FACE, BOTTOM_FACE, FRONT_FACE, BACK_FACE, LEFT_FACE, RIGHT_FACE
};
const std::vector<bool> transparentBlocks = {0,0,0,1,0,0,0,0,1};

const int SNOW_LEVEL = 100;
const int STONE_LEVEL = 60;
const int SAND_LEVEL = 20;

const int BLOCK_WIDTH = 1;

const glm::vec3 TOP_FACE_POS = glm::vec3(0.0,0.5,0.0);
const glm::vec3 BOTTOM_FACE_POS = glm::vec3(0.0,-0.5,0.0);

const glm::vec3 FRONT_FACE_POS = glm::vec3(0.0,0.0,0.5);
const glm::vec3 BACK_FACE_POS = glm::vec3(0.0,0.0,-0.5);

const glm::vec3 LEFT_FACE_POS = glm::vec3(-0.5,0.0,0.0);
const glm::vec3 RIGHT_FACE_POS = glm::vec3(0.5,0.0,0.0);

const std::vector<float> BLOCK_FACE_VERTICES = {
    -0.5f,  0.0f, -0.5f, 1,               // 0.0, 1.0,  top left
    -0.5f,  0.0f,  0.5f, 0,               // 0.0, 0.75, bottom left
     0.5f,  0.0f,  0.5f, 3,               // 0.25, 0.75 bottom right
    -0.5f,  0.0f, -0.5f, 1,               // 0.0, 1.0,  top left
     0.5f,  0.0f,  0.5f, 3,               // 0.25, 0.75 bottom right
     0.5f,  0.0f, -0.5f, 2                // 0.25, 1.0, top right

};
const int BLOCK_FACE_VERTICES_COUNT = 6;

class Block{

    public:
        Block(BlockType blockType, glm::vec3 blockPosition){
            this->type = blockType; 
            this->position = blockPosition;
        };
        BlockType type;
        glm::vec3 position;

        bool isTransparent(){
        
            return transparentBlocks[type];
        }
};

#endif
