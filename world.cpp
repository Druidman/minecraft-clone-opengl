#include "world.h"
#include <optional>

#define FNL_IMPL
#include "vendor/fastNoise/FastNoiseLite.h"

std::vector<float> World::trees_area_gen(int size)
{
    
    std::vector<float> trees(size * size, 0.0f);

        // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.fractal_type = FNL_FRACTAL_NONE;
    noise.frequency = 0.011;
    noise.octaves = 3;
    noise.seed = 1337;

    noise.noise_type = FNL_NOISE_VALUE;

    // Gather noise data
    
    unsigned long int index = 0;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++) 
        {
            float gen = fnlGetNoise2D(&noise, y, x);
            gen = (gen + 1) / 2;

            trees[index++] = gen;
            
        }
    }
    
    return trees;
    
}

std::vector<int> World::blocks_area_gen(int size)
{
    std::vector<int> world(size * size, 0.0f);

        // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.frequency = 0.005;
    noise.octaves = 4;
    noise.seed = 1652;

    noise.noise_type = FNL_NOISE_PERLIN;

    // Gather noise data
    
    unsigned long int index = 0;
    int min = 1000;
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++) 
        {
            float gen = fnlGetNoise2D(&noise, y, x);
            gen = pow(2,((gen + 1) / 2 * 10));

            world[index++] = (int)gen;
            if ((int)gen < min){
                min = (int)gen;
            }
        }
    }
    // lets make world's wfirst block be settled on 0.0
    for (int i=0; i<world.size();i++){
        world[i] -= min;
    } 


    
        

    return world;

}

void World::genWorld()
{
    const unsigned long long WORLD_BLOCKS_COUNT = WIDTH * WIDTH;
    const unsigned long long WORLD_CHUNKS_COUNT = WORLD_BLOCKS_COUNT / ((unsigned long long)CHUNK_WIDTH * CHUNK_WIDTH);

    std::vector<int> blocks = blocks_area_gen(WIDTH);
    std::vector<float> trees = trees_area_gen(WIDTH);


    int startX = 0;
    int startY = 0;

    for (int row = 0; row < WIDTH / CHUNK_WIDTH; row++){
        for (int column = 0; column < WIDTH / CHUNK_WIDTH; column++){
            if (startX >= WIDTH){
                startX = 0;
                startY += CHUNK_WIDTH;
            }

            glm::vec3 chunkPos = glm::vec3(startX + (CHUNK_WIDTH / 2), 0.0, startY + (CHUNK_WIDTH / 2));
            Chunk chunk = Chunk(chunkPos);
            
            
            for (int i =startX; i< startX + CHUNK_WIDTH ; i++){
                for (int j =startY; j< startY + CHUNK_WIDTH; j++){

                    float zCoord = blocks[j * WIDTH + i] + 0.5;
                    BlockType blockType = GRASS_DIRT;
                    if (zCoord < 20){
                     
                        blockType = SAND;
                    }
                    
                    else if (zCoord > 60){
                        blockType = STONE;
                    }
                    
                    Block block(blockType,glm::vec3(i + 0.5, zCoord ,j + 0.5));
                    
                    chunk.addBlock(block);
                    
                    chunk.fillUnderBlock(block);
                    
                }
            }
            
            chunk.fillWater();

            for (int i =startX + 3; i< startX + CHUNK_WIDTH - 3; i+= 6){
                for (int j =startY + 3; j< startY + CHUNK_WIDTH - 3; j+=6){

                    float treeChance = trees[j * WIDTH + i];
                    if (treeChance < 0.5){
                        continue ;
                    }
                    float zCoord = blocks[j * WIDTH + i] + 0.5;
                    
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
            
            if (column == 0){
                chunks.push_back(std::vector<Chunk>(1,chunk));
                
            }
            else{
                chunks[row].push_back(chunk);
            }
            
            startX += CHUNK_WIDTH;
            
        }
    }
    for (int row =0; row < chunks.size(); row++){
        for (int col =0; col < chunks[row].size(); col++){
            this->chunkRefs.push_back(&chunks[row][col]);
        }   
    }
    for (Chunk* chunk : this->chunkRefs){
        chunk->genChunkMesh();
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
