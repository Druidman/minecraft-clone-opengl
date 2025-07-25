#include "world.h"
#include <optional>

#define FNL_IMPL
#include "vendor/fastNoise/FastNoiseLite.h"

float World::genTreeChance(glm::vec2 positionXZ){
    float gen = fnlGetNoise2D(&this->treeNoise, positionXZ.y, positionXZ.x);
    gen = (gen + 1) / 2;
    return gen;
}


int World::genBlockHeight(glm::vec2 positionXZ){
    float gen = fnlGetNoise2D(&this->blockNoise, positionXZ.y, positionXZ.x);
    gen = pow(2,((gen + 1) / 2 * 10));
    return gen;
}

void World::genWorldBase()
{
    const unsigned long long WORLD_BLOCKS_COUNT = WIDTH * WIDTH;
    const unsigned long long WORLD_CHUNKS_COUNT = WORLD_BLOCKS_COUNT / ((unsigned long long)CHUNK_WIDTH * CHUNK_WIDTH);

    int startX = 0;
    int startY = 0;

    for (int row = 0; row < WIDTH / CHUNK_WIDTH; row++){
        for (int column = 0; column < WIDTH / CHUNK_WIDTH; column++){
            if (startX >= WIDTH){
                startX = 0;
                startY += CHUNK_WIDTH;
            }
            glm::vec3 chunkPos = glm::vec3(startX + (CHUNK_WIDTH / 2), 0.0, startY + (CHUNK_WIDTH / 2));
            genChunk(chunkPos);
            startX += CHUNK_WIDTH;
            
        }
    }
    for (int row =0; row < chunks.size(); row++){
        for (int col =0; col < chunks[row].size(); col++){
            this->chunkRefs.push_back(&chunks[row][col]);
        }   
    }
    std::cout << "Generating world meshes...\n";
    for (Chunk* chunk : this->chunkRefs){
        chunk->genChunkMesh();
        
    }
    
}

void World::genChunk(glm::vec3 position)
{
    
    Chunk chunk = Chunk(position, this);
    for (int i =0; i< CHUNK_WIDTH ; i++){
        for (int j =0; j< CHUNK_WIDTH; j++){
            float x = j + 0.5 + (position.x - (CHUNK_WIDTH / 2)); // j = column
            float z = i + 0.5 + (position.z - (CHUNK_WIDTH / 2)); // i = row

            float zCoord = genBlockHeight(glm::vec2(x,z)) + 0.5;
            BlockType blockType = GRASS_DIRT;
            if (zCoord < 20){
                blockType = SAND;
            }
            
            else if (zCoord > 60){
                blockType = STONE;
            }
            
            Block block(blockType,glm::vec3(x, zCoord ,z));
            
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
            float zCoord = genBlockHeight(glm::vec2(x,z)) + 0.5;
            
            glm::vec3 treePos = glm::vec3(i + 0.5 + (rand() % 6), zCoord ,j + 0.5 + (rand() % 6));
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
    int chunkRow = (int)(std::floor(position.z / (float)CHUNK_WIDTH));
    int chunkCol = (int)(std::floor(position.x / (float)CHUNK_WIDTH));

    if (chunkRow >= chunks.size()){
        chunks.push_back(std::vector<Chunk>(1,chunk));
    }
    else{
        chunks[chunkRow].push_back(chunk);
    }
}

std::optional<Chunk *> World::getChunkByPos(glm::vec3 pointPositionInWorld)
{
    int chunkRow = (int)(std::floor(pointPositionInWorld.z / (float)CHUNK_WIDTH));
    int chunkCol = (int)(std::floor(pointPositionInWorld.x / (float)CHUNK_WIDTH));
    if (chunkRow >= this->chunks.size() ||
        chunkCol >= this->chunks[chunkRow].size()
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
