#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "chunk.h"
#include "vendor/fastNoise/FastNoiseLite.h"
#include "buffer.h"
#include "camera.h"
#include <optional>

class Player;


class World{
    private:
        fnl_state blockNoise = genBlockNoiseFunc();
        fnl_state treeNoise = genTreeNoiseFunc();
    private:
        fnl_state genBlockNoiseFunc(){
            fnl_state noise = fnlCreateState();
            noise.fractal_type = FNL_FRACTAL_FBM;
            noise.frequency = 0.005;
            noise.octaves = 4;
            noise.seed = 1652;

            noise.noise_type = FNL_NOISE_PERLIN;
            return noise;
        }
        fnl_state genTreeNoiseFunc(){
            fnl_state noise = fnlCreateState();
            noise.fractal_type = FNL_FRACTAL_NONE;
            noise.frequency = 0.011;
            noise.octaves = 3;
            noise.seed = 1337;

            noise.noise_type = FNL_NOISE_VALUE;
            return noise;
        }
        
        int genBlockHeight(glm::vec2 positionXZ);
        float genTreeChance(glm::vec2 positionXZ);

        void initChunks();
        
    public:
        
        int WIDTH;
        int CHUNK_ROWS; // expressed by amount of chunks
        int CHUNK_COLUMNS; // expressed by amount of chunks
        

        std::vector< std::vector <Chunk> > chunks;
        
        std::vector< std::vector < Chunk*  > > chunkRefs;
        std::vector< std::vector < Chunk*  > > renderChunkRefs;

        Buffer* meshBuffer;
        Buffer* chunkDrawBuffer;
        Buffer* chunkStorageBuffer;

        Player* player;
        Camera* camera;

        glm::vec3 worldMiddle;
    public:
        World(int width, glm::vec3 worldMiddle, Buffer* meshBuffer, Buffer* chunkDrawBuffer, Buffer* chunkStorageBuffer){ 
            this->WIDTH = width;
            this->CHUNK_ROWS = this->WIDTH / CHUNK_WIDTH;
            this->CHUNK_COLUMNS = this->WIDTH / CHUNK_WIDTH;
            this->meshBuffer = meshBuffer;
            this->chunkDrawBuffer = chunkDrawBuffer;
            this->chunkStorageBuffer = chunkStorageBuffer;
            this->worldMiddle = worldMiddle;
        }
        void init(Player *player, Camera* camera);
        void addChunk(Chunk* chunk);
        void genChunkRefs();
        void genWorldBase();
        void prepareChunks(glm::vec3 playerPos){};
        void genChunk(Chunk* chunk);

        void addChunkToBuffers(Chunk* chunk);
        void fillBuffers();
        void fillChunkStorageBuffer();

        unsigned long long getWorldMeshSize();

        int getChunkRow(Chunk* chunk);
        int getChunkCol(Chunk* chunk);

        

        std::optional<Chunk*> getChunkByPos(glm::vec3 pointPositionInWorld);
        std::optional<Block*> getBlockByPos(glm::vec3 pointPositionInWorld, bool noneBlock = false);

        
};
#endif