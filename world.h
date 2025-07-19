#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "chunk.h"
#include <optional>



class World{
    private:
        std::vector<float> trees_area_gen(int size);
        std::vector<int> blocks_area_gen(int size);
    public:
        int WIDTH;
        std::vector< std::vector <Chunk> > chunks;
        std::vector<Chunk* > chunkRefs;
    public:
        World(int width){ this->WIDTH = width; }
        void genWorld();

        std::optional<Chunk*> getChunkByPos(glm::vec3 pointPositionInWorld);

        
};
#endif