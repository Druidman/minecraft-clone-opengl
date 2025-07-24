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
        int worldOriginx = 1073741823; // float max / 2
        int worldOriginz = 1073741823; // float max / 2
        int WIDTH;

        std::vector< std::vector <Chunk> > chunks;
        std::vector<Chunk* > chunkRefs;
    public:
        World(int width){ 
            this->WIDTH = width;
            std::cout << "Generating world...\n";
            genWorldBase(); 
        }
        void genWorldBase();

        std::optional<Chunk*> getChunkByPos(glm::vec3 pointPositionInWorld);
        std::optional<Block*> getBlockByPos(glm::vec3 pointPositionInWorld, bool noneBlock = false);

        
};
#endif