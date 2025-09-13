#ifndef WORLD_H
#define WORLD_H



#include <vector>
#include <optional>
#include <list>


#include "vendor/fastNoise/FastNoiseLite.h"
#include "vendor/glm/glm.hpp"

#include "block.h"
#include "buffer.h"
#include <thread>

class Player;
class Renderer;
class Chunk;


struct ChunkVecPos{
    int row,col;
};

struct ThreadWorkingData {
    std::vector< Chunk > chunksToPrepare;
    std::vector< bool > chunksDone;
    std::vector< bool > chunksInserted;
    std::vector< ChunkVecPos > chunkPositions;
    bool ready = false;
};

struct WorldTickData {
    bool playerChangedChunk = false;
    bool playerChangedPosition = false;
    bool requiresRefsUpdate = false;
};

class World{ //world class
    private:
        fnl_state blockNoise = genBlockNoiseFunc();
        fnl_state treeNoise = genTreeNoiseFunc();
    private:
        fnl_state genBlockNoiseFunc();
        fnl_state genTreeNoiseFunc();
        
        

        void initChunks();

        void setChunkToFlat(Chunk* chunk);
        
    public:
        bool flatWorld = false;

        
        int WIDTH;
        int CHUNK_ROWS; // expressed by amount of chunks
        int CHUNK_COLUMNS; // expressed by amount of chunks
        int RENDER_DISTANCE;
        

        std::vector< std::vector <Chunk> > chunks;
        

        std::vector < Chunk*  > chunkRenderRefs;

        Player* player;
    
        Renderer *renderer;
        glm::vec3 worldMiddle;
        glm::vec3 sunPosition;

        glm::vec3 lastPlayerPos;
        Chunk* lastPlayerChunk;

        std::list< std::thread > threads;
        std::list< ThreadWorkingData > threadsWorkingData;

        float sunAngle = 0.0;
    public:
        World(int width, glm::vec3 worldMiddle, Renderer* renderer);
        ~World();
        
        void init(Player *player);
    public:
        int genBlockHeight(glm::vec2 positionXZ);
        float genTreeChance(glm::vec2 positionXZ);

        
        
        void addChunk(Chunk* chunk);
        void genRenderChunkRefs();
        void genWorldBase();


        void updateWorld(double delta);

        void updateSun(double delta, WorldTickData *worldTickData);
        void updateChunkRender(WorldTickData *worldTickData);
        void updateChunks(WorldTickData *worldTickData);
        void updateThreads(WorldTickData *worldTickData);


        
        unsigned long long getWorldMeshSize();

        int getChunkRow(Chunk* chunk);
        int getChunkCol(Chunk* chunk);

        

        std::optional<Chunk*> getChunkByPos(glm::vec3 pointPositionInWorld);
        std::optional<Block*> getBlockByPos(glm::vec3 pointPositionInWorld, bool noneBlock = false);

        void removeChunk(Chunk* chunk, bool merge = false);
    public:
        void prepareChunks(ThreadWorkingData &data);

        void spawnChunkPrepareThread(std::vector< Chunk > chunksToPrepare, std::vector<ChunkVecPos> chunkPositions);
};
#endif