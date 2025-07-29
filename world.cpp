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
    genRenderChunkRefs();
}


void World::genRenderChunkRefs(){
    this->chunkRenderRefs.clear();
    for (int row = 0; row < CHUNK_ROWS; row++){
        for (int col=0; col < CHUNK_COLUMNS; col++){
            Chunk* chunk = &this->chunks[row][col];
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
            
            genChunk(&chunk);
            
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
    std::cout << "lastChunkPos: " << lastPlayerChunk->position.x << " " << lastPlayerChunk->position.z << "\n";
    std::cout << "ChunkPos: " << chunk->position.x << " " << chunk->position.z << "\n";
    glm::vec3 positionChange = lastPlayerChunk->position - chunk->position;
    std::cout << "POSC: " << positionChange.x << "\n";
    lastPlayerChunk = chunk;
    int lastPlayerChunkRow = getChunkRow(lastPlayerChunk);
    int lastPlayerChunkCol = getChunkCol(lastPlayerChunk);
    if (positionChange.x > 0){ // that means we moved: -x
        // we need to remove right chunks
        // we need to add left chunks
        std::cout << "add left\n";
        
 
        for (std::vector<Chunk> &chunkRow : this->chunks){
   
            chunkRow.pop_back();
   
            
        }
        std::vector<Chunk> column;
        for (int row=0; row<CHUNK_ROWS; row++){
            
            glm::vec3 chunkPos = this->chunks[row].front().position - glm::vec3(CHUNK_WIDTH,0.0,0.0);
            column.emplace_back(chunkPos,this);
        }
        int ind = 0;
        std::cout << "insert sizes: \n";
        for (std::vector<Chunk> &chunkRow : this->chunks){
            std::cout << chunkRow.size() << " ";
            chunkRow.insert(chunkRow.begin(), column[ind]);
            std::cout << chunkRow.size() << "\n";
            ind++;
        }
        // after insertion we need to change lastPlayerChunk pointer due to shifting all elements
        lastPlayerChunk = &this->chunks[lastPlayerChunkRow][lastPlayerChunkCol + 1];
        worldMiddle.x -= CHUNK_WIDTH;
        std::cout << "worldMiddle: " << worldMiddle.x << " " << worldMiddle.z << "\n";
        std::cout << "lastChunkPos: " << lastPlayerChunk->position.x << " " << lastPlayerChunk->position.z << "\n";
        std::cout << "playerPos" << player->position.x << " " << player->position.z << "\n";
        for (std::vector<Chunk> &chunkRow : this->chunks){
            genChunk(&chunkRow.front());
            chunkRow.front().genChunkMesh();
        }
    }
    if (positionChange.x < 0){ // that means we moved: +x
        // we need to add right chunks
        // we need to remove left chunks
        std::cout << "add right\n";
        

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

        worldMiddle.x += CHUNK_WIDTH;
        lastPlayerChunk = &this->chunks[lastPlayerChunkRow][lastPlayerChunkCol - 1];
        std::cout << "worldMiddle: " << worldMiddle.x << " " << worldMiddle.z << "\n";
        std::cout << "lastChunkPos: " << lastPlayerChunk->position.x << " " << lastPlayerChunk->position.z << "\n";
        std::cout << "playerPos" << player->position.x << " " << player->position.z << "\n";
        
        for (std::vector<Chunk> &chunkRow : this->chunks){
            genChunk(&chunkRow.back());
            chunkRow.back().genChunkMesh();
        }
        
    }
    if (positionChange.z > 0){ // that means we moved: -z
        // we need to remove bottom chunks
        // we need to add top chunks
        std::cout << "add top\n";
        
        this->chunks.pop_back();
        std::vector<Chunk> row;
        for (int col=0; col<CHUNK_COLUMNS; col++){
            glm::vec3 chunkPos = this->chunks.front()[col].position - glm::vec3(0.0,0.0,CHUNK_WIDTH);
            row.emplace_back(chunkPos,this);
        }
        this->chunks.insert(this->chunks.begin(),row);
        worldMiddle.z -= CHUNK_WIDTH;
        
        for (int col=0; col<CHUNK_COLUMNS; col++){
            genChunk(&this->chunks.front()[col]);
            this->chunks.front()[col].genChunkMesh();
        }


        
    }
    if (positionChange.z < 0){ // that means we moved: +z
        // we need to remove top chunks
        // we need to add bottom chunks
        std::cout << "add bottom\n";
    
        this->chunks.erase(this->chunks.begin());
        std::vector<Chunk> row;
        for (int col=0; col<CHUNK_COLUMNS; col++){
            glm::vec3 chunkPos = this->chunks.back()[col].position + glm::vec3(0.0,0.0,CHUNK_WIDTH);
            row.emplace_back(chunkPos,this);
        }
        this->chunks.push_back(std::move(row));
        worldMiddle.z += CHUNK_WIDTH;
        
        for (int col=0; col<CHUNK_COLUMNS; col++){
            genChunk(&this->chunks.back()[col]);
            this->chunks.back()[col].genChunkMesh();
        }


        
    }
   




    genRenderChunkRefs();
    fillBuffers();
    
    



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
    chunk->chunkReady = true;
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
