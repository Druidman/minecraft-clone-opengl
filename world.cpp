#include "world.h"
#include "renderer.h"
#include <optional>

#define FNL_IMPL
#include "vendor/fastNoise/FastNoiseLite.h"

float World::genTreeChance(glm::vec2 positionXZ){
    float gen = fnlGetNoise2D(&this->treeNoise, positionXZ.y, positionXZ.x);
    gen = (gen + 1) / 2;
    return gen;
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
    
            row.push_back(chunk);

            startX += CHUNK_WIDTH;
            
        }
    
        this->chunks.push_back(row);
        startX = this->worldMiddle.x - ((float)WIDTH / 2);
        startZ += CHUNK_WIDTH;
        
    }
    
}


void World::genRenderChunkRefs(){
    this->chunkRenderRefs.clear();
    for (int row = 0; row < CHUNK_ROWS; row++){
        for (int col=0; col < CHUNK_COLUMNS; col++){
            Chunk* chunk = &this->chunks[row][col];
            if (!chunk->renderReady){
                continue;
            }
            float dist = glm::distance(glm::vec2(chunk->position.x , chunk->position.z), glm::vec2(player->position.x , player->position.z));
            if (dist / (float)CHUNK_WIDTH < RENDER_DISTANCE){
                this->chunkRenderRefs.push_back(chunk);
            }
            
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
            if (!chunk.chunkReady){
                continue;
            }
            chunk.genChunkMesh();
        }
    }
    
}

void World::updateChunks()
{
    std::list<ThreadWorkingData>::iterator dataIterator = threadsWorkingData.begin();
    std::list<std::thread>::iterator threadIterator = threads.begin();
    bool anything = false;
    while (dataIterator != threadsWorkingData.end() && threadIterator != threads.end()){
        bool isReady = dataIterator->ready;
        for (int chunkInd =0; chunkInd < CHUNK_COLUMNS; chunkInd++){
            if (!dataIterator->chunksDone[chunkInd]){
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
            anything = true;
            this->chunks[row][col] = dataIterator->chunksToPrepare[chunkInd];
       
            dataIterator->chunksDone[chunkInd] = false;
            
        }

        if (isReady){
            dataIterator = threadsWorkingData.erase(dataIterator);
            threadIterator->join();
            threadIterator = threads.erase(threadIterator);
        }
        else {
            dataIterator++;
            threadIterator++;
        }
        
   
        
        
    }
    genRenderChunkRefs();
    
    fillBuffers();
    
    

    if (this->lastPlayerPos == player->position){
        return ;
    }
    lastPlayerPos = player->position;
    
    std::optional<Chunk* > chunkRes = getChunkByPos(player->position);
    if (!chunkRes.has_value()){
        return ; // idk what to do

    }
    Chunk* chunk = chunkRes.value();
    if (lastPlayerChunk == chunk){
        fillChunkStorageBuffer();
        return ; // the same chunk 
    }

    glm::vec3 positionChange = lastPlayerChunk->position - chunk->position;

    lastPlayerChunk = chunk;
    int lastPlayerChunkRow = getChunkRow(lastPlayerChunk);
    int lastPlayerChunkCol = getChunkCol(lastPlayerChunk);
    if (positionChange.x > 0){ // that means we moved: -x
        // we need to remove right chunks
        // we need to add left chunks

        for (std::vector<Chunk> &chunkRow : this->chunks){
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
        for (int row = 0; row<column.size(); row++){
            chunkPositions.push_back({
                row,0
            });
        };
        std::vector< bool > chunksDone(column.size(),false);

        spawnChunkPrepareThread(column, chunksDone, chunkPositions);
        
        // ___
    }
    else if (positionChange.x < 0){ // that means we moved: +x
        // we need to add right chunks
        // we need to remove left chunks

        

        for (std::vector<Chunk> &chunkRow : this->chunks){
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
        for (int row = 0; row<column.size(); row++){
            chunkPositions.push_back({
                row,(int)chunks[row].size() - 1
            });
        };
        std::vector< bool > chunksDone(column.size(),false);
 
        spawnChunkPrepareThread(column, chunksDone, chunkPositions);
        // ___
        
    }
    else if (positionChange.z > 0){ // that means we moved: -z
        // we need to remove bottom chunks
        // we need to add top chunks

        
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
        std::vector< bool > chunksDone(CHUNK_COLUMNS,false);
        
      
        spawnChunkPrepareThread(row, chunksDone, chunkPositions);
        // ___


        
    }
    else if (positionChange.z < 0){ // that means we moved: +z
        // we need to remove top chunks
        // we need to add bottom chunks

    
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
        for (int col = 0; col<row.size(); col++){
            chunkPositions.push_back({
                (int)chunks.size() - 1, col
            });
        };
        std::vector< bool > chunksDone(row.size(),false);
 
        spawnChunkPrepareThread(row, chunksDone, chunkPositions);
        // ___
    }

    

}

void World::init(Player *player, Camera *camera)
{
    this->player = player;
    this->camera = camera;
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

void World::addChunkToBuffers(Chunk *chunk)
{
    DrawArraysIndirectCommand data = {
        BLOCK_FACE_VERTICES_COUNT,
        (uint)chunk->transparentMesh.size() + (uint)chunk->opaqueMesh.size(),
        0,
        (uint)(this->meshBuffer->getFilledDataSize() / sizeof(CHUNK_MESH_DATATYPE))
    };
    this->chunkDrawBuffer->addData<DrawArraysIndirectCommand>(data);

    // vec4 due to std430 in shader
    // this approach passes regular chunk coord to buffer but if we place cam always at 0,0,0 then it won't work
    // this->chunkStorageBuffer->addData<glm::vec4>(glm::vec4(chunk->position,0.0)); 
    // SO we shift chunks pos by camera position
    this->chunkStorageBuffer->addData<glm::vec4>(glm::vec4(chunk->position - player->position,0.0)); 

    this->meshBuffer->addData< CHUNK_MESH_DATATYPE >(chunk->getOpaqueMesh());
    this->meshBuffer->addData< CHUNK_MESH_DATATYPE >(chunk->getTransparentMesh());
}

void World::fillBuffers()
{
    unsigned long long sizeToAlloc = getWorldMeshSize();

    this->meshBuffer->allocateBuffer(sizeToAlloc);
    this->chunkDrawBuffer->allocateBuffer(sizeof(DrawArraysIndirectCommand) * chunkRenderRefs.size());
    this->chunkStorageBuffer->allocateBuffer(sizeof(glm::vec4) * chunkRenderRefs.size()); // vec4 due to std430 in shader
    for (Chunk* chunk : chunkRenderRefs){
        
        addChunkToBuffers(chunk);
    }
    GLCall( glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->chunkStorageBuffer->getId()) );
}

void World::fillChunkStorageBuffer()
{   
    std::vector<glm::vec4> chunkPositions;
    for (Chunk* chunk : chunkRenderRefs){
        
        chunkPositions.push_back(glm::vec4(chunk->position - player->position,0.0));
        
    }       
    
    this->chunkStorageBuffer->updateData<glm::vec4>(&chunkPositions, 0);
}

unsigned long long World::getWorldMeshSize()
{
    unsigned long long sizeToAlloc = 0;
    for (Chunk* chunk : chunkRenderRefs){
        sizeToAlloc += chunk->getMeshSize();
    
    }
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
