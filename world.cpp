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
    std::cout << "generating Chunk Refs\n";
    genChunkRefs();
}

void World::genChunkRefs()
{
    
    this->chunkRefs.clear();
    for (int row = 0; row < CHUNK_ROWS; row++){
        this->chunkRefs.push_back(std::vector<Chunk* >(CHUNK_COLUMNS, nullptr));
        for (int col=0; col < CHUNK_COLUMNS; col++){
            this->chunkRefs[row][col] = &this->chunks[row][col];
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

    for (std::vector<Chunk* > &chunkRow : this->chunkRefs){
        for (Chunk* chunk : chunkRow){
            if (chunk == nullptr){
                ExitError("WORLD","NULLPTR IN CHUNKS");
                continue;
            }
            genChunk(chunk);
        }
    }
    for (std::vector<Chunk* > &chunkRow : this->chunkRefs){
        for (Chunk* chunk : chunkRow){
            if (chunk == nullptr){
                ExitError("WORLD","NULLPTR IN CHUNKS");
                continue;
            }
            chunk->genChunkMesh();
        }
    }
    
}

void World::genChunk(Chunk *chunk)
{
    for (int i =0; i< CHUNK_WIDTH ; i++){
        for (int j =0; j< CHUNK_WIDTH; j++){
            float x = j + 0.5 + (chunk->position.x - (CHUNK_WIDTH / 2)); // j = column
            float z = i + 0.5 + (chunk->position.z - (CHUNK_WIDTH / 2)); // i = row
            float yCoord = genBlockHeight(glm::vec2(x,z)) + 0.5;
            BlockType blockType = GRASS_DIRT;
            if (yCoord < 20){
                blockType = SAND;
            }
            
            else if (yCoord > 60){
                blockType = STONE;
            }
            
            Block block(blockType,glm::vec3(x, yCoord ,z));
            
            chunk->addBlock(block);
            
            chunk->fillUnderBlock(block);
            
        }
    }
    
    chunk->fillWater();

    for (int i =3; i< CHUNK_WIDTH - 3; i+= 6){
        for (int j =3; j< CHUNK_WIDTH - 3; j+=6){
            float x = j + 0.5 + (chunk->position.x - (CHUNK_WIDTH / 2)); // j = column
            float z = i + 0.5 + (chunk->position.z - (CHUNK_WIDTH / 2)); // i = row
            float treeChance = genTreeChance(glm::vec2(x,z));
            if (treeChance < 0.5){
                continue ;
            }
            float yCoord = genBlockHeight(glm::vec2(x,z)) + 0.5;
            
            glm::vec3 treePos = glm::vec3(i + 0.5 + (rand() % 6), yCoord ,j + 0.5 + (rand() % 6));
            auto blockRes = chunk->getBlock(treePos);
            if (!blockRes.has_value()){ // cause that means smth went really wrong 
                continue ;
            }
            
            Block *block = blockRes.value();
            if (block->type != GRASS_DIRT){
                continue ;
            }
            chunk->addTree(block->position + glm::vec3(0.0,1.0,0.0));
        }
    }
}

void World::init(Player *player, Camera *camera)
{
    this->player = player;
    this->camera = camera;
    initChunks();
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
    this->chunkStorageBuffer->addData<glm::vec4>(glm::vec4(chunk->position - worldMiddle,0.0)); 

    this->meshBuffer->addData< CHUNK_MESH_DATATYPE >(chunk->getOpaqueMesh());
    this->meshBuffer->addData< CHUNK_MESH_DATATYPE >(chunk->getTransparentMesh());
}

void World::fillBuffers()
{
    unsigned long long sizeToAlloc = getWorldMeshSize();

    this->meshBuffer->allocateBuffer(sizeToAlloc);
    this->chunkDrawBuffer->allocateBuffer(sizeof(DrawArraysIndirectCommand) * CHUNK_ROWS * CHUNK_COLUMNS);
    this->chunkStorageBuffer->allocateBuffer(sizeof(glm::vec4) * CHUNK_ROWS * CHUNK_COLUMNS); // vec4 due to std430 in shader

    std::cout << "Filling buffers\n";
    for (std::vector< Chunk* > &chunkRow : chunkRefs){
        for (Chunk* chunk : chunkRow){
            addChunkToBuffers(chunk);
            std::cout << "CHUNK: " << chunk->position.x << " " << chunk->position.y << " " << chunk->position.z << " " << '\n';
        }
        
    }
    GLCall( glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->chunkStorageBuffer->getId()) );
}

void World::fillChunkStorageBuffer()
{   
    std::vector<glm::vec4> chunkPositions;
    for (std::vector< Chunk* > &chunkRow : chunkRefs){
        for (Chunk* chunk : chunkRow){
            chunkPositions.push_back(glm::vec4(chunk->position - camera->position,0.0));
        }
    }       
    
    this->chunkStorageBuffer->updateData<glm::vec4>(&chunkPositions, 0);
}

unsigned long long World::getWorldMeshSize()
{
    unsigned long long sizeToAlloc = 0;
    for (std::vector< Chunk* > &chunkRow : chunkRefs){
        for (Chunk* chunk : chunkRow){
            sizeToAlloc += chunk->getMeshSize();
        }
        
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

    return chunkRefs[chunkRow][chunkCol];
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
