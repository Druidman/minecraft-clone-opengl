#ifndef MATH_H
#define MATH_H

#include <optional>
#include <vector>

#include "vendor/glm/glm.hpp"

#include "block.h"
#include "chunk.h"
#include "world.h"



struct BlockIntersection{
    Block *block;
    Chunk *chunk;
    glm::vec3 hitPos;
};


inline std::optional<BlockIntersection> shootRay(
    glm::vec3 startPos, 
    int range, 
    std::vector<BlockType> ignoredBlocks,
    glm::vec3 direction, 
    World *world
){
    direction = glm::normalize(direction);

    const float stepSize = 1 / 50.0f;
    glm::vec3 target = startPos;
    bool targetFound = false;

    for (int i=0; i< range / stepSize; i++){
    
        target += direction * stepSize;

        std::optional<Block*> blockRes = world->getBlockByPos(target);
        if (!blockRes.has_value()){
            continue ;
        }
        Block *block = blockRes.value();

        bool foundBlock = true;
        for (BlockType type : ignoredBlocks){
            if (block->type == type){
                foundBlock = false;
                break;
            }
        }
        if (!foundBlock){
            continue;
        }
        std::optional<Chunk*> chunkRes = world->getChunkByPos(target);
        if (!chunkRes.has_value()){
            ExitError("MATH","ARE YOU MESSING WITH MEMORY????");
        }
        

        BlockIntersection intersection;
        intersection.block = block;
        intersection.chunk = chunkRes.value();
        intersection.hitPos = target;
        
        return intersection;
        
    }
    
    return std::nullopt;
}
#endif