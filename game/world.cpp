#include "world.h"

#include <vector>
#include <optional>
#include <list>
#include <thread>
#define FNL_IMPL
#include "vendor/fastNoise/FastNoiseLite.h"

#include "chunk.h"
#include "player.h"
#include "renderer.h"
#include "buffer.h"





float World::genTreeChance(glm::vec2 positionXZ){
    float gen = fnlGetNoise2D(&this->treeNoise, positionXZ.y, positionXZ.x);
    gen = (gen + 1) / 2;
    return gen;
}

fnl_state World::genBlockNoiseFunc()
{
    fnl_state noise = fnlCreateState();
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.frequency = 0.005;
    noise.octaves = 4;
    noise.seed = blockSeed;
    

    noise.noise_type = FNL_NOISE_PERLIN;
    return noise;
}

fnl_state World::genTreeNoiseFunc()
{
    fnl_state noise = fnlCreateState();
    noise.fractal_type = FNL_FRACTAL_NONE;
    noise.frequency = 0.005;
    noise.octaves = 4;
    noise.seed = treeSeed;

    

    noise.noise_type = FNL_NOISE_VALUE;
    return noise;
}
void World::setChunkToFlat(Chunk *chunk)
{
    chunk->terrainGenData.flat = true;
    chunk->terrainGenData.blocksType = GRASS_DIRT;
}
void World::initChunks()
{

    float startX =  this->worldMiddle.x - ((float)WIDTH / 2);
    float startZ =  this->worldMiddle.z - ((float)WIDTH / 2);

    for (int i=0; i<CHUNK_ROWS; i++){

        std::vector< Chunk > row;
        for (int j=0; j<CHUNK_COLUMNS; j++){
            glm::vec3 chunkPos = glm::vec3(startX + (CHUNK_WIDTH / 2), 0.0, startZ + (CHUNK_WIDTH / 2));

            Chunk chunk = Chunk(chunkPos, this);
            if (this->flatWorld){
                setChunkToFlat(&chunk);
                
            }
    
            row.push_back(chunk);

            startX += CHUNK_WIDTH;
            
        }
        
        this->chunks.push_back(row);
        startX = this->worldMiddle.x - ((float)WIDTH / 2);
        startZ += CHUNK_WIDTH;
        
    }

    
}

void World::generateSeeds()
{
    
    generateBlockSeed();
    generateTreeSeed();
    
}

void World::generateBlockSeed()
{
    this->blockSeed = std::rand() % 10000;
    
}

void World::generateTreeSeed()
{
    this->treeSeed = std::rand() % 10000;
    
}

void World::setSeeds(int blockSeed, int treeSeed)
{
    if (blockSeed == 0){
        generateBlockSeed();
    }
    else {
        this->blockSeed = blockSeed;
    }

    if (treeSeed == 0){
        generateTreeSeed();
    }
    else {
        this->treeSeed = treeSeed;
    }
    generateNoises();
    
    WriteToLogFile("BLOCK_SEED",std::to_string(this->blockSeed));
    WriteToLogFile("TREE_SEED",std::to_string(this->treeSeed));
}

void World::generateNoises()
{
    this->blockNoise = genBlockNoiseFunc();
    this->treeNoise = genTreeNoiseFunc();
}

void World::genRenderChunkRefs()
{

    this->chunkRenderRefs.clear();
    
    for (int row = 0; row < CHUNK_ROWS; row++){
        for (int col=0; col < CHUNK_COLUMNS; col++){
            Chunk* chunk = &this->chunks[row][col];
            if (!chunk->renderReady){
                continue;                          
            }
            
            
            // float dist = glm::distance(glm::vec2(chunk->position.x , chunk->position.z), glm::vec2(player->position.x , player->position.z));
            // if (dist / (float)CHUNK_WIDTH < RENDER_DISTANCE){
                
            this->chunkRenderRefs.push_back(chunk);
                

            // }
            
        }
    }
}

int World::genBlockHeight(glm::vec2 positionXZ){
    float gen = fnlGetNoise2D(&this->blockNoise, positionXZ.y, positionXZ.x);
    gen = pow(2,((gen + 1) / 2 * 10));
    return gen;
}

void World::genWorldBase()
{
    std::cout << "Generating world...\n";
    for (std::vector<Chunk> &chunkRow : this->chunks){
        for (Chunk &chunk : chunkRow){
            chunk.genChunk();
        }
    }
    for (std::vector<Chunk> &chunkRow : this->chunks){
        for (Chunk &chunk : chunkRow){
            chunk.genChunkMesh();
        }
    }

    
    
}

void World::updateChunks(WorldTickData *worldTickData){ 
    worldTickData->playerChangedChunk = false;
    worldTickData->playerChangedPosition = false;
    // returns true when this->chunks has been changed
    if (this->lastPlayerPos == player->position){
        return; //player didn't move
    }
    lastPlayerPos = player->position;
    worldTickData->playerChangedPosition = true;

    
    std::optional<Chunk* > chunkRes = getChunkByPos(player->position);
    if (!chunkRes.has_value()){
        return; // player is outside of the map so we do nothing
    }
    Chunk* chunk = chunkRes.value();
    if (lastPlayerChunk == chunk){
        // player changed pos but is in the same chunk
        return; // player didn't change chunks
    }
        //player changed chunks


    worldTickData->playerChangedChunk = true;
    glm::vec3 positionChange = lastPlayerChunk->position - chunk->position;

    int rowsChanged = std::abs(getChunkRow(lastPlayerChunk) - getChunkRow(chunk));
    int colsChanged = std::abs(getChunkCol(lastPlayerChunk) - getChunkCol(chunk));
    lastPlayerChunk = chunk;
    int lastPlayerChunkRow = getChunkRow(lastPlayerChunk);
    int lastPlayerChunkCol = getChunkCol(lastPlayerChunk);



    for (int changeI=0; changeI < colsChanged; changeI++){
        if (positionChange.x > 0){ // that means we moved: -x
            // we need to remove right chunks
            // we need to add left chunks

            bool merge = false;
            int i = this->chunks.size();
            for (std::vector<Chunk> &chunkRow : this->chunks){
                i--;
                if (i == 0){
                    merge = true; // we are removing last chunk row so we need to merge free zones
                }
    
                removeChunk(&chunkRow[chunkRow.size() - 1], merge);
                chunkRow.pop_back();
        
                
            }
            
            std::vector<Chunk> column;
            for (int row=0; row<CHUNK_ROWS; row++){
                
                glm::vec3 chunkPos = this->chunks[row].front().position - glm::vec3(CHUNK_WIDTH,0.0,0.0);
                column.emplace_back(chunkPos,this);
            }
            int ind = 0;

            for (std::vector<Chunk> &chunkRow : this->chunks){
                chunkRow.insert(chunkRow.begin(), column[ind]);
                ind++;
            }
            // after insertion we need to shift chunk positions for threads and lastPlayerChunk pointer
    
            lastPlayerChunk = &this->chunks[lastPlayerChunkRow][lastPlayerChunkCol + 1];
            for (ThreadWorkingData &data : threadsWorkingData){
                for (ChunkVecPos &pos : data.chunkPositions){
                    pos.col++;
                };
            };

            worldMiddle.x -= CHUNK_WIDTH;


            // not safe chunk gen due to memory shifts
            // ___
            std::vector< ChunkVecPos > chunkPositions;
            for (int row = 0; row<(int)column.size(); row++){
                chunkPositions.push_back({
                    row,0
                });
            };
            

            spawnChunkPrepareThread(column, chunkPositions);
            lastPlayerChunkCol += 1;
            
            // ___
        }
        else if (positionChange.x < 0){ // that means we moved: +x
            // we need to add right chunks
            // we need to remove left chunks

            
            bool merge = false;
            int i = this->chunks.size();
            for (std::vector<Chunk> &chunkRow : this->chunks){
                i--;
                if (i == 0){
                    merge = true; // we are removing last chunk row so we need to merge free zones
                }
    
                removeChunk(&chunkRow[0], merge);
                
    
                chunkRow.erase(chunkRow.begin());
                
                
                
            }
            std::vector<Chunk> column;
            for (int row=0; row<CHUNK_ROWS; row++){
                glm::vec3 chunkPos = this->chunks[row].back().position + glm::vec3(CHUNK_WIDTH,0.0,0.0);
                column.emplace_back(chunkPos,this);
            }
            int ind = 0;
            for (std::vector<Chunk> &chunkRow : this->chunks){
                chunkRow.push_back(column[ind]);
                ind++;
            }
            // after insertion we need to shift chunk positions for threads and lastPlayerChunk pointer
            
            lastPlayerChunk = &this->chunks[lastPlayerChunkRow][lastPlayerChunkCol - 1];
            for (ThreadWorkingData &data : threadsWorkingData){
                for (ChunkVecPos &pos : data.chunkPositions){
                    pos.col-- ;
                }
            }

            worldMiddle.x += CHUNK_WIDTH;
            
            
            // not safe chunk gen due to memory shifts
            // ___
            std::vector< ChunkVecPos > chunkPositions;
            for (int row = 0; row<(int)column.size(); row++){
                chunkPositions.push_back({
                    row,(int)chunks[row].size() - 1
                });
            };

    
            spawnChunkPrepareThread(column,  chunkPositions);
            // ___
            lastPlayerChunkCol -= 1;
            
        }
        
    }

    for (int changeI=0; changeI < rowsChanged; changeI++){
        if (positionChange.z > 0){ // that means we moved: -z
            // we need to remove bottom chunks
            // we need to add top chunks
            bool merge = false;
            int i = this->chunks.back().size();
            for (Chunk &chunk : this->chunks.back()){
                i--;
                if (i == 0){
                    merge = true; // we are removing last chunk row so we need to merge free zones
                }

                removeChunk(&chunk, merge);

                
            }

            this->chunks.pop_back();

            
            std::vector<Chunk> row;
            for (int col=0; col<CHUNK_COLUMNS; col++){
                glm::vec3 chunkPos = this->chunks.front()[col].position - glm::vec3(0.0,0.0,CHUNK_WIDTH);
                row.emplace_back(chunkPos,this);
            }
            this->chunks.insert(this->chunks.begin(),row);
            worldMiddle.z -= CHUNK_WIDTH;
            

            for (ThreadWorkingData &data : threadsWorkingData){
                for (ChunkVecPos &pos : data.chunkPositions){
                    pos.row++;
                }
            }
            // not safe chunk gen due to memory shifts
            // ___


            std::vector< ChunkVecPos > chunkPositions;
            for (int col = 0; col<CHUNK_COLUMNS; col++){
                chunkPositions.push_back({
                    0, col
                });
            };
 
            
        
            spawnChunkPrepareThread(row, chunkPositions);
            // ___


            
        }
        else if (positionChange.z < 0){ // that means we moved: +z
            // we need to remove top chunks
            // we need to add bottom chunks
            bool merge = false;
            int i = this->chunks.front().size();
            for (Chunk &chunk : this->chunks.front()){
                i--;
                if (i == 0){
                    merge = true; // we are removing last chunk row so we need to merge free zones
                }
                
                removeChunk(&chunk, merge);
                
            
            }

            this->chunks.erase(this->chunks.begin());
        
            std::vector<Chunk> row;
            for (int col=0; col<CHUNK_COLUMNS; col++){
                glm::vec3 chunkPos = this->chunks.back()[col].position + glm::vec3(0.0,0.0,CHUNK_WIDTH);
                row.emplace_back(chunkPos,this);
            }
            this->chunks.push_back(row);
            worldMiddle.z += CHUNK_WIDTH;
            

            for (ThreadWorkingData &data : threadsWorkingData){
                for (ChunkVecPos &pos : data.chunkPositions){
                    pos.row--;
                }
            }
        
            // not safe chunk gen due to memory shifts
            // ___
            std::vector< ChunkVecPos > chunkPositions;
            for (int col = 0; col<(int)row.size(); col++){
                chunkPositions.push_back({
                    (int)chunks.size() - 1, col
                });
            };
    
            spawnChunkPrepareThread(row,  chunkPositions);
            // ___
        }
    }
    
    worldTickData->requiresRefsUpdate = true; // just needed 50%
}

void World::updateThreads(WorldTickData *worldTickData){
    std::list<ThreadWorkingData>::iterator dataIterator = threadsWorkingData.begin();
    std::list<std::thread>::iterator threadIterator = threads.begin();

  
    while (dataIterator != threadsWorkingData.end() && threadIterator != threads.end()){
        
        
        for (int chunkInd =0; chunkInd < CHUNK_COLUMNS; chunkInd++){
            if (glfwGetTime() - worldTickData->updateStartTime > MAX_FRAME_TIME_MS){
                break;
            }
            if (!dataIterator->chunksDone[chunkInd] || dataIterator->chunksInserted[chunkInd]){
        
                continue;
            }
            
            
            int row = dataIterator->chunkPositions[chunkInd].row;
            int col = dataIterator->chunkPositions[chunkInd].col;
            if (row < 0 || col < 0){
                continue;
            }
            if (row >= CHUNK_ROWS || col >= CHUNK_COLUMNS){
                continue;
            }
            
            
            this->removeChunk(&this->chunks[row][col], true);

            this->chunks[row][col] = dataIterator->chunksToPrepare[chunkInd];
            std::cout << "adding chunk from thread...: " << row << " " << col << "\n";
            this->renderer->addChunk(&this->chunks[row][col]);
            std::cout << "End\n";
          
       
            dataIterator->chunksInserted[chunkInd] = true; // so that we won't insert it again
            worldTickData->requiresRefsUpdate = true;
            
            

        }

        bool isReady = true;
        for (int chunkInd =0; chunkInd < CHUNK_COLUMNS; chunkInd++){
            
            if (
                !dataIterator->chunksInserted[chunkInd] && 
                
                dataIterator->chunkPositions[chunkInd].row < CHUNK_ROWS &&
                dataIterator->chunkPositions[chunkInd].col < CHUNK_COLUMNS &&
                dataIterator->chunkPositions[chunkInd].row >= 0 &&
                dataIterator->chunkPositions[chunkInd].col >= 0
                
                
            ){
                isReady = false;
                break;
            }
        }


        if (isReady){
            
            
            
            threadIterator->join();
            threadIterator = threads.erase(threadIterator);
            dataIterator = threadsWorkingData.erase(dataIterator);
            
        }
        else {
            dataIterator++;
            threadIterator++;
        }

        
        
       
        


      
    }
  
}

void World::updateSun(double delta, WorldTickData *worldTickData){
    sunPosition = glm::vec3(0.0,WIDTH,0.0);
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f),glm::radians(sunAngle),glm::vec3(1.0,0.0,1.0));
    sunPosition = glm::vec4(sunPosition,0.0) * rotate;
    sunPosition += worldMiddle;

    sunAngle += 6 * delta;
}

void World::updateChunkRender(WorldTickData *worldTickData){
    if (!worldTickData->playerChangedChunk && !worldTickData->requiresRefsUpdate){
        return ;
    }
    double refsStart = glfwGetTime();
    genRenderChunkRefs();
    double refsEnd = glfwGetTime();


 
    std::cout << "Storage Buffer update...\n";
    double storageStart = glfwGetTime();
    this->renderer->fillBuffer(STORAGE_BUFFER);  
    double storageEnd = glfwGetTime();
    std::cout << "End\n";

    
    std::cout << "Indirect Buffer update...\n";
    double indirectStart = glfwGetTime();
    this->renderer->fillBuffer(INDIRECT_BUFFER);
    double indirectEnd = glfwGetTime();
    std::cout << "End\n";
    

    

    
    std::cout \
     << "STORAGE_BUFFER: " << (storageEnd - storageStart) * 1000 << "\n" 
     << "REFS: " << (refsEnd - refsStart) * 1000 << "\n"
     << "INDIRECT: " << (indirectEnd - indirectStart) * 1000 << "\n";
}

void World::updateWorld(double delta)
{
    WorldTickData worldTickData = {
        false, // playerChangedChunk
        false,  // playerChangedPosition
        false, // requires refs update
        glfwGetTime() * 1000, // update start ms
        delta * 1000, // last frame time ms


    };
    
    std::cout << "End\n";
    
    // make sun move
    updateSun(delta, &worldTickData);

    std::cout << "Chunk Update...\n";
    double chunkStart = glfwGetTime();
    // handles chunk generation management
    updateChunks(&worldTickData);
    double chunkEnd = glfwGetTime();
    std::cout << "End\n";

    std::cout << "Thread update...\n";
    double threadsStart = glfwGetTime();
    // handles chunk generation threads
    updateThreads(&worldTickData);
    double threadsEnd = glfwGetTime();
    std::cout << "End\n";

    std::cout << "Chunk render update...\n";
    double chunkRenderStart = glfwGetTime();
    // generates render command
    updateChunkRender(&worldTickData);
    double chunkRenderEnd = glfwGetTime();

    

    std::cout << "CHUNK UPDATE: " << (chunkEnd - chunkStart) * 1000 << "\n"
     << "THREADS: " << (threadsEnd - threadsStart) * 1000 << "\n" 
     << "CHUNK RENDER UPDATE: " << (chunkRenderEnd - chunkRenderStart) * 1000 << "\n";

    this->lastFrameWorldTickData = worldTickData;
  

    
    
}

World::World(int width, glm::vec3 worldMiddle, Renderer *renderer, int blockSeed, int treeSeed)
{ 
    this->WIDTH = width;
    this->CHUNK_ROWS = this->WIDTH / CHUNK_WIDTH;
    this->CHUNK_COLUMNS = this->WIDTH / CHUNK_WIDTH;
    this->RENDER_DISTANCE = CHUNK_ROWS * 2; //(CHUN`K_ROWS / 4) + 1; // render distance is much smaller than stored chunks 
    this->renderer = renderer;
    this->worldMiddle = worldMiddle;

    setSeeds(blockSeed,treeSeed);

}



World::~World()
{
    for (std::thread &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}


void World::init(Player *player)
{
    this->player = player;

  
    lastPlayerPos = player->position;
    initChunks();
    std::optional< Chunk* > chunkRes = getChunkByPos(lastPlayerPos);
    if (!chunkRes.has_value()){
        ExitError("WORLD","assigning last chunk in init went wrong");
        return ;
    }
    lastPlayerChunk = chunkRes.value();
}

void World::addChunk(Chunk *chunk)
{
    if (chunk == nullptr){
        return; 
    }
    int row = getChunkRow(chunk);
    int col = getChunkCol(chunk);

    
    if (row >= CHUNK_ROWS){
        return ; 
    }
    if (col >= CHUNK_COLUMNS){
        return;
    }
    if (row < 0 || col < 0){
        return;
    }

    this->chunks[row][col] = *chunk;
}

unsigned long long World::getWorldMeshSize()
{
    unsigned long long sizeToAlloc = 0;
    for (std::vector< Chunk > &chunkRow : this->chunks){
        for (Chunk &chunk : chunkRow){
            sizeToAlloc += chunk.getMeshSize();
        }
       
    };
    
    return sizeToAlloc;
}

unsigned long long World::getWorldOpaqueMeshSize()
{
    unsigned long long sizeToAlloc = 0;
    for (std::vector< Chunk > &chunkRow : this->chunks){
        for (Chunk &chunk : chunkRow){
            sizeToAlloc += chunk.getOpaqueMeshSize();
        }
       
    };
    
    return sizeToAlloc;
}

unsigned long long World::getWorldTransparentMeshSize()
{
    unsigned long long sizeToAlloc = 0;
    for (std::vector< Chunk > &chunkRow : this->chunks){
        for (Chunk &chunk : chunkRow){
            sizeToAlloc += chunk.getTransparentMeshSize();
        }
       
    };
    
    return sizeToAlloc;
}

int World::getChunkCol(Chunk *chunk)
{
    int chunkCol = (int)(std::floor((chunk->position.x - this->worldMiddle.x + (float)(this->WIDTH / 2)) / (float)CHUNK_WIDTH));
    return chunkCol;
    
}

int World::getChunkRow(Chunk *chunk)
{
    int chunkRow = (int)(std::floor((chunk->position.z - this->worldMiddle.z + (float)(this->WIDTH / 2)) / (float)CHUNK_WIDTH));
    return chunkRow;
}

std::optional<Chunk *> World::getChunkByPos(glm::vec3 pointPositionInWorld)
{
    int chunkRow = (int)(std::floor((pointPositionInWorld.z - this->worldMiddle.z + (float)(this->WIDTH / 2)) / (float)CHUNK_WIDTH));
    int chunkCol = (int)(std::floor((pointPositionInWorld.x - this->worldMiddle.x + (float)(this->WIDTH / 2)) / (float)CHUNK_WIDTH));
    if (chunkRow >= CHUNK_ROWS ||
        chunkCol >= CHUNK_COLUMNS ||
        chunkRow < 0 ||
        chunkCol < 0 
    ){
        
        return std::nullopt;
    }

    

    if (pointPositionInWorld.y < this->chunks[chunkRow][chunkCol].position.y){
    
        return std::nullopt;
    }

    return &this->chunks[chunkRow][chunkCol];
}

std::optional<Block *> World::getBlockByPos(glm::vec3 pointPositionInWorld, bool noneBlock)
{
    std::optional<Chunk *> chunkRes = getChunkByPos(pointPositionInWorld);
    if (!chunkRes.has_value()){
        return std::nullopt; 
    }
    
    std::optional<Block *> blockRes = (chunkRes.value())->getBlock(pointPositionInWorld, noneBlock);
    if (!blockRes.has_value()){

        return std::nullopt; 
    }
    return blockRes.value();
}

void World::removeChunk(Chunk *chunk, bool merge)
{
    this->renderer->deleteChunk(chunk,merge);
    
   
}


void World::prepareChunks(ThreadWorkingData &data)
{
    
    for (Chunk &chunk : data.chunksToPrepare){
        if (this->flatWorld){
            World::setChunkToFlat(&chunk);
        }
        chunk.genChunk();

       
    }
    int ind=0;
    for (Chunk &chunk : data.chunksToPrepare){
 
        chunk.genChunkMesh();


        data.chunksDone[ind] = true;
        ind++;
        
    }
    data.ready = true;

}
void World::spawnChunkPrepareThread(std::vector<Chunk> chunksToPrepare, std::vector<ChunkVecPos> chunkPositions) {
    std::vector<bool> chunksDone(chunksToPrepare.size(), false);
    std::vector<bool> chunksInserted(chunksToPrepare.size(), false);
    threadsWorkingData.push_back({
        chunksToPrepare,
        chunksDone,
        chunksInserted,
        chunkPositions
    });
    threads.emplace_back(
        &World::prepareChunks, 
        this,
        std::ref(threadsWorkingData.back())
    );
  
    
};
