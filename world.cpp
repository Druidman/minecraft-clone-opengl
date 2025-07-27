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

void World::genChunkRefs()
{
    this->chunkRefs.clear();
    for (int row =0; row < chunks.size(); row++){
        for (int col =0; col < chunks[row].size(); col++){
            this->chunkRefs.push_back(&(chunks[row][col]));
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
    const unsigned long long WORLD_BLOCKS_COUNT = WIDTH * WIDTH;
    const unsigned long long WORLD_CHUNKS_COUNT = WORLD_BLOCKS_COUNT / ((unsigned long long)CHUNK_WIDTH * CHUNK_WIDTH);

    float startX =  this->player->position.x - ((float)WIDTH / 2);
    float startZ =  this->player->position.z - ((float)WIDTH / 2);

    for (int row = 0; row < WIDTH / CHUNK_WIDTH; row++){
        for (int column = 0; column < WIDTH / CHUNK_WIDTH; column++){
            if (startX >= (WIDTH / 2) + this->player->position.x){
                startX = this->player->position.x - (WIDTH / 2);
                startZ += CHUNK_WIDTH;
            }
            glm::vec3 chunkPos = glm::vec3(startX + (CHUNK_WIDTH / 2), 0.0, startZ + (CHUNK_WIDTH / 2));
            genChunk(chunkPos);
            startX += CHUNK_WIDTH;
            
        }
    }
    genChunkRefs();
    
    std::cout << "Generating world meshes...\n";
    for (Chunk* chunk : this->chunkRefs){
        chunk->genChunkMesh();
        
    }
    
}

void World::genChunk(glm::vec3 position)
{
    
    Chunk chunk = Chunk(position, this);
    std::cout << "ChunkPOS: " << position.x <<  " " << position.y <<  " " << position.z <<  " " << "\n";
    for (int i =0; i< CHUNK_WIDTH ; i++){
        for (int j =0; j< CHUNK_WIDTH; j++){
            float x = j + 0.5 + (position.x - (CHUNK_WIDTH / 2)); // j = column
            float z = i + 0.5 + (position.z - (CHUNK_WIDTH / 2)); // i = row
            float yCoord = genBlockHeight(glm::vec2(x,z)) + 0.5;
            BlockType blockType = GRASS_DIRT;
            if (yCoord < 20){
                blockType = SAND;
            }
            
            else if (yCoord > 60){
                blockType = STONE;
            }
            
            Block block(blockType,glm::vec3(x, yCoord ,z));
            
            chunk.addBlock(block);
            
            chunk.fillUnderBlock(block);
            
        }
    }
    
    chunk.fillWater();

    for (int i =3; i< CHUNK_WIDTH - 3; i+= 6){
        for (int j =3; j< CHUNK_WIDTH - 3; j+=6){
            float x = j + 0.5 + (position.x - (CHUNK_WIDTH / 2)); // j = column
            float z = i + 0.5 + (position.z - (CHUNK_WIDTH / 2)); // i = row
            float treeChance = genTreeChance(glm::vec2(x,z));
            if (treeChance < 0.5){
                continue ;
            }
            float yCoord = genBlockHeight(glm::vec2(x,z)) + 0.5;
            
            glm::vec3 treePos = glm::vec3(i + 0.5 + (rand() % 6), yCoord ,j + 0.5 + (rand() % 6));
            auto blockRes = chunk.getBlock(treePos);
            if (!blockRes.has_value()){ // cause that means smth went really wrong 
                continue ;
            }
            
            Block *block = blockRes.value();
            if (block->type != GRASS_DIRT){
                continue ;
            }
            chunk.addTree(block->position + glm::vec3(0.0,1.0,0.0));

            
        }
    }
    int chunkRow = getChunkRow(&chunk);
    std::cout << chunkRow << "\n";

    if (chunkRow >= chunks.size()){
        std::cout << "row add\n";
        chunks.push_back(std::vector<Chunk>(1,chunk));
    }
    else{
        std::cout << "regular add\n";
        chunks[chunkRow].push_back(chunk);
    }
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
    this->chunkStorageBuffer->addData<glm::vec4>(glm::vec4(chunk->position - camera->position,0.0)); 

    this->meshBuffer->addData< CHUNK_MESH_DATATYPE >(chunk->getOpaqueMesh());
    this->meshBuffer->addData< CHUNK_MESH_DATATYPE >(chunk->getTransparentMesh());
}

void World::fillBuffers()
{
    unsigned long long sizeToAlloc = getWorldMeshSize();

    this->meshBuffer->allocateBuffer(sizeToAlloc);
    this->chunkDrawBuffer->allocateBuffer(sizeof(DrawArraysIndirectCommand) * chunkRefs.size());
    this->chunkStorageBuffer->allocateBuffer(sizeof(glm::vec4) * chunkRefs.size()); // vec4 due to std430 in shader

    std::cout << "Filling buffers\n";
    for (Chunk* chunk: chunkRefs){
        addChunkToBuffers(chunk);
        
    }
    GLCall( glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, this->chunkStorageBuffer->getId()) );
}

void World::fillChunkStorageBuffer()
{   
    std::vector<glm::vec4> chunkPositions;
    for (Chunk* chunk : chunkRefs){
        chunkPositions.push_back(glm::vec4(chunk->position - camera->position,0.0));
    }       
    
    this->chunkStorageBuffer->updateData<glm::vec4>(&chunkPositions,0);
        
    
    
    
    
}

unsigned long long World::getWorldMeshSize()
{
    unsigned long long sizeToAlloc = 0;
    for (Chunk* chunk: chunkRefs){
        sizeToAlloc += chunk->getMeshSize();
    }
    return sizeToAlloc;
}

int World::getChunkCol(Chunk *chunk)
{
    int chunkCol = (int)(std::floor((chunk->position.x - this->player->position.x + (float)(this->WIDTH / 2)) / (float)CHUNK_WIDTH));
    return chunkCol;
    
}

int World::getChunkRow(Chunk *chunk)
{
    int chunkRow = (int)(std::floor((chunk->position.z - this->player->position.z + (float)(this->WIDTH / 2)) / (float)CHUNK_WIDTH));
    return chunkRow;
}

std::optional<Chunk *> World::getChunkByPos(glm::vec3 pointPositionInWorld)
{
    int chunkRow = (int)(std::floor((pointPositionInWorld.z - this->player->position.z + (float)(this->WIDTH / 2)) / (float)CHUNK_WIDTH));
    int chunkCol = (int)(std::floor((pointPositionInWorld.x - this->player->position.x + (float)(this->WIDTH / 2)) / (float)CHUNK_WIDTH));
    if (chunkRow >= this->chunks.size() ||
        chunkCol >= this->chunks[chunkRow].size() ||
        chunkRow < 0 ||
        chunkCol < 0 
    ){
        
        return std::nullopt;
    }

    

    if (pointPositionInWorld.y < this->chunks[chunkRow][chunkCol].position.y){
    
        return std::nullopt;
    }

    return &chunks[chunkRow][chunkCol];
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
