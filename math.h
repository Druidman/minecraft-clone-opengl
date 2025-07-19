#ifndef MATH_H
#define MATH_H

#include <optional>
#include <vector>

#include "vendor/glm/glm.hpp"

#include "chunk.h"
#include "world.h"
#include "block.h"

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

        std::optional<Chunk*> result = world->getChunkByPos(target);
        if (!result.has_value()){
            continue ;
        }

        Chunk *currentChunk = result.value();

        std::optional<Block*> blockRes = currentChunk->getBlock(target);
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

        BlockIntersection intersection;
        intersection.block = block;
        intersection.chunk = currentChunk;
        intersection.hitPos = target;
        
        return intersection;
        
    }
    
    return std::nullopt;
}
#endif