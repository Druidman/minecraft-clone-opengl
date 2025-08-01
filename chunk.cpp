#include "chunk.h"
#include "world.h"
#include <cstring>
#include <optional>

Chunk::Chunk(glm::vec3 chunkPosition, World *world)
{
    this->position = chunkPosition;
    this->world = world;
    addBlockPlatform();
}


std::optional<Block *> Chunk::getBlock(int plat, int row, int col, bool noneBlock)
{
    if (
        plat >= blocks.size() ||
        plat < 0 ||
        row >= CHUNK_WIDTH ||
        row < 0 ||
        col >= CHUNK_WIDTH ||
        col < 0 
    ) return std::nullopt; 
    //those were boundaries checks

    if (row >= blocks[plat].size()){
        return std::nullopt;
    } // no blocks from here on
    if (col >= blocks[plat][row].size()){
        return std::nullopt;
    }// no blocks from here on too

    
    Block *block = &blocks[plat][row][col];
    if (block->type == NONE_BLOCK && !noneBlock)
    {
        return std::nullopt;
    }
    return block;
}

std::optional<Block *> Chunk::getBlock(glm::vec3 positionInWorld, bool noneBlock)
{
    
    int platform = floor(positionInWorld.y) - position.y;
    int col = floor(positionInWorld.x) - (position.x - (CHUNK_WIDTH / 2));
    int row = floor(positionInWorld.z) - (position.z - (CHUNK_WIDTH / 2));
    
    return getBlock(platform, row, col, noneBlock);
}

void Chunk::addBlockFace(Face face, Block *block, std::vector<CHUNK_MESH_DATATYPE> *buffer)
{
    glm::vec3 blockPositionChunk = block->position - position + glm::vec3(CHUNK_WIDTH / 2, 0.0, CHUNK_WIDTH / 2);

    int textId;
    if (face == 0)
    { // top
        textId = block->type * 4;
    }
    else if (face == 1)
    { // bottom
        textId = block->type * 4 + 2;
    }
    else
    { // middle
        textId = block->type * 4 + 1;
    }

    int zCoord = blockPositionChunk.z - 0.5;
    int yCoord = blockPositionChunk.y - 0.5;
    int xCoord = blockPositionChunk.x - 0.5;
    int32_t data = 0;

    data |= ((int32_t)(textId & 127)) << 19; // TTTTTTT: 7 bitów
    data |= ((int32_t)(face & 7)) << 16;     // NNN: 3 bity
    data |= ((int32_t)(xCoord & 15)) << 12;  // XXXX: 4 bity
    data |= ((int32_t)(yCoord & 255)) << 4;  // YYYYYYYY: 8 bitów
    data |= ((int32_t)(zCoord & 15));        // ZZZZ: 4 bity

    CHUNK_MESH_DATATYPE packedFloat;
    std::memcpy(&packedFloat,&data,sizeof(CHUNK_MESH_DATATYPE));
    buffer->push_back(packedFloat);
}

bool Chunk::canAddBlockFace(Face face, Block *currentBlock)
{
    glm::vec3 checkBlockPos = currentBlock->position;
    glm::vec3 checkBlockFacePos = checkBlockPos;
    switch (face)
    {
        case TOP_FACE:
            checkBlockPos.y += 1;
            checkBlockFacePos += TOP_FACE_POS;
            break;
        case BOTTOM_FACE:
            checkBlockPos.y -= 1;
            checkBlockFacePos += BOTTOM_FACE_POS;
            break;
        case FRONT_FACE:
            checkBlockPos.z += 1;
            checkBlockFacePos += FRONT_FACE_POS;
            break;
        case BACK_FACE:
            checkBlockPos.z -= 1;
            checkBlockFacePos += BACK_FACE_POS;
            break;
        case LEFT_FACE:
            checkBlockPos.x -= 1;
            checkBlockFacePos += LEFT_FACE_POS;
            break;
        case RIGHT_FACE:
            checkBlockPos.x += 1;
            checkBlockFacePos += RIGHT_FACE_POS;
            break;
    }
    std::optional<Block *> res;
    // lets check if face is on border of world
    if (checkBlockFacePos.y == 0 || 
        checkBlockFacePos.x == world->worldMiddle.x - (world->WIDTH / 2) || 
        checkBlockFacePos.z == world->worldMiddle.z - (world->WIDTH / 2) || 
        checkBlockFacePos.x == world->worldMiddle.x + (world->WIDTH / 2) || 
        checkBlockFacePos.z == world->worldMiddle.z + (world->WIDTH / 2))
    {
        return true;
    }
    else {
        res = getBlock(checkBlockPos);
    }
    
    
    if (!res.has_value())
    {
        return true;
    }
    
    Block *checkBlock = res.value();

    if (!checkBlock->isTransparent())
    {
        return false;
        
    }
    if (currentBlock->isTransparent()){
        return false;
    }

    return true;
    
    
}

void Chunk::meshBlock(int platform, int row, int col){
    Block *block = &this->blocks[platform][row][col];
    if (block->type == NONE_BLOCK){
        return;
    }

    std::vector<CHUNK_MESH_DATATYPE> *buffer;
    if (block->type == WATER || block->type == LEAF)
    {
        buffer = &transparentMesh;
    }
    else
    {
        buffer = &opaqueMesh;
    }

    // top
    if (canAddBlockFace(TOP_FACE, block))
    {
        addBlockFace(TOP_FACE, block, buffer);
    }
    // bottom
    if (canAddBlockFace(BOTTOM_FACE, block))
    {
        addBlockFace(BOTTOM_FACE, block, buffer);
    }
    // front
    if (canAddBlockFace(FRONT_FACE, block))
    {
        addBlockFace(FRONT_FACE, block, buffer);
    }
    // back
    if (canAddBlockFace(BACK_FACE, block))
    {
        addBlockFace(BACK_FACE, block, buffer);
    }
    // left
    if (canAddBlockFace(LEFT_FACE, block))
    {
        addBlockFace(LEFT_FACE, block, buffer);
    }
    // right
    if (canAddBlockFace(RIGHT_FACE, block))
    {
        addBlockFace(RIGHT_FACE, block, buffer);
    }
}

void Chunk::genChunk()
{
    for (int i =0; i< CHUNK_WIDTH ; i++){
        for (int j =0; j< CHUNK_WIDTH; j++){
            float x = j + 0.5 + (position.x - (CHUNK_WIDTH / 2)); // j = column
            float z = i + 0.5 + (position.z - (CHUNK_WIDTH / 2)); // i = row
            float yCoord = world->genBlockHeight(glm::vec2(x,z)) + 0.5;
            BlockType blockType = GRASS_DIRT;
            if (yCoord < 20){
                blockType = SAND;
            }
            
            else if (yCoord > 60){
                blockType = STONE;
            }
            
            Block block(blockType,glm::vec3(x, yCoord ,z));
            
            addBlock(block);
            
            fillUnderBlock(block);
            
        }
    }
    
    fillWater();

    for (int i =3; i< CHUNK_WIDTH - 3; i+= 6){
        for (int j =3; j< CHUNK_WIDTH - 3; j+=6){
            float x = j + 0.5 + (position.x - (CHUNK_WIDTH / 2)); // j = column
            float z = i + 0.5 + (position.z - (CHUNK_WIDTH / 2)); // i = row
            float treeChance = world->genTreeChance(glm::vec2(x,z));
            if (treeChance < 0.5){
                continue ;
            }
            float yCoord = world->genBlockHeight(glm::vec2(x,z)) + 0.5;
            
            glm::vec3 treePos = glm::vec3(x + (rand() % 6), yCoord ,z + (rand() % 6));
            auto blockRes = getBlock(treePos);
            if (!blockRes.has_value()){ // cause that means smth went really wrong 
                continue ;
            }
            
            Block *block = blockRes.value();
            if (block->type != GRASS_DIRT){
                continue ;
            }
            addTree(block->position + glm::vec3(0.0,1.0,0.0));
        }
    }

    // important
    this->chunkReady = true;
}

void Chunk::genChunkMesh()
{

    opaqueMesh.clear();
    transparentMesh.clear();
    
    for (int platform = 0; platform < blocks.size(); platform++)
    {
        for (int row = 0; row < blocks[platform].size(); row++)
        {

            for (int col = 0; col < blocks[platform][row].size(); col++)
            {
                
                meshBlock(platform,row,col);
            }
        }
    }
    this->renderReady = true;
}


unsigned long long Chunk::getMeshSize()
{
    unsigned long long size = sizeof(CHUNK_MESH_DATATYPE) * (transparentMesh.size() + opaqueMesh.size());
    return size;
}


glm::vec3 Chunk::getPositionInWorld(int platform, int row, int column){
    float y = platform + 0.5 + this->position.y;
    float x = column + 0.5 + (this->position.x - (CHUNK_WIDTH / 2));
    float z = row + 0.5 + (this->position.z - (CHUNK_WIDTH / 2));

    return glm::vec3(x,y,z);

}

bool Chunk::isInChunkBorder(Block &block){
    if (block.position.x > this->position.x + CHUNK_WIDTH ||
        block.position.x < this->position.x - CHUNK_WIDTH ||
        block.position.z > this->position.z + CHUNK_WIDTH ||
        block.position.z < this->position.z - CHUNK_WIDTH ||
        block.position.y < this->position.y)
    {
        return false;// adding to chunk outside its border
    }
    return true;
}
bool Chunk::addBlock(Block &block)
{
    if (!isInChunkBorder(block)){
        return false;
    }
    
    int platform = block.position.y - 0.5 - position.y;
    int column = block.position.x - 0.5 - (position.x - (CHUNK_WIDTH / 2));
    int row = block.position.z - 0.5 - (position.z - (CHUNK_WIDTH / 2));

    if (getBlock(platform,row,column).has_value()){
        return false; //block already exists /or invalid row,col,platform
    }

    if (platform > this->blocks.size() - 1)
    {
        int platformsToAdd = platform - this->blocks.size() + 1;
        for (int i = 0; i < platformsToAdd; i++)
        {
            addBlockPlatform();
        }
    }
    if (row > this->blocks[platform].size()-1){
        int rowsToAdd = row - this->blocks[platform].size() + 1;
        for (int i = 0; i < rowsToAdd; i++)
        {
            addBlockRow(platform);
        }
    }
    if (column > this->blocks[platform][row].size()-1){
        int blocksToAdd = column - this->blocks[platform][row].size() + 1;
        for (int i = 0; i < blocksToAdd; i++)
        {
            Block fillBlock = Block(NONE_BLOCK,getPositionInWorld(platform,row,column));
            this->blocks[platform][row].push_back(fillBlock);
        }
    }
    
    
    this->blocks[platform][row][column] = block;
    return true;
}

bool Chunk::updateBlock(Block &block)
{   
    std::optional<Block*> targetBlockRes = getBlock(block.position);
    if (!targetBlockRes.has_value()){
        return false; // no block here or block outside border or none block here
    }
    Block *targetBlock = targetBlockRes.value();
    targetBlock->position = block.position;
    targetBlock->type = block.type;
    return true;
}

void Chunk::addBlockPlatform()
{
    std::vector<std::vector<Block>> platform = std::vector<std::vector<Block>>(1, std::vector<Block>(1,Block(NONE_BLOCK,getPositionInWorld(this->blocks.size(),0,0))));
    this->blocks.push_back(platform);
}

void Chunk::addBlockRow(int platform)
{
    std::vector<Block> row = std::vector<Block>(1,Block(NONE_BLOCK,getPositionInWorld(platform,this->blocks[platform].size(),0)));
    this->blocks[platform].push_back(row);
}   

bool Chunk::removeBlock(int platform, int row, int col)
{
    if (
        platform >= this->blocks.size() ||
        row >= CHUNK_WIDTH ||
        col >= CHUNK_WIDTH ||
        platform < 0 ||
        row < 0 ||
        col < 0
    ){
        return false;
    }
    blocks[platform][row][col].type = NONE_BLOCK;
    return true;
}

bool Chunk::removeBlock(glm::vec3 blockPositionInWorld)
{
    // removes block at pos
    int platform = floor(blockPositionInWorld.y) - this->position.y;
    int col = floor(blockPositionInWorld.x) - (this->position.x - (CHUNK_WIDTH / 2));
    int row = floor(blockPositionInWorld.z) - (this->position.z - (CHUNK_WIDTH / 2));

    return removeBlock(platform,row,col);

}

void Chunk::fillWater()
{
    for (int platform = 0; platform < 20; platform++)
    {
        if (platform >= this->blocks.size())
        {
            addBlockPlatform();
        }
        for (int row = 0; row < CHUNK_WIDTH; row++)
        {
            if (row >= this->blocks[platform].size())
            {
                addBlockRow(platform);
            }
            for (int col = 0; col < CHUNK_WIDTH; col++)
            {
                
                auto blockRes = getBlock(platform,row,col);
                if (blockRes.has_value())
                {
                    continue;
                }
                Block waterBlock = Block(WATER,getPositionInWorld(platform,row,col));
                addBlock(waterBlock);
                
            }
        }
    }
}

void Chunk::fillUnderBlock(Block &block)
{
    glm::vec3 underBlockPos = block.position;
    underBlockPos.y -= 1;
    int col = floor(underBlockPos.x) - (position.x - (CHUNK_WIDTH / 2));
    int row = floor(underBlockPos.z) - (position.z - (CHUNK_WIDTH / 2));
    while (underBlockPos.y >= 0.5){
        int platform = floor(underBlockPos.y) - position.y;
        
        

        Block underBlock = Block(STONE, underBlockPos);
        addBlock(underBlock);
        
        
        underBlockPos.y -= 1;
        
    }
}

void Chunk::addTree(glm::vec3 positionInWorld)
{
    for (glm::vec3 blockPos : TREE::woodPositions){
        Block woodBlock = Block(WOOD,blockPos + positionInWorld);
        addBlock(woodBlock);
    }
    for (glm::vec3 blockPos : TREE::leafPositions){
        Block leafBlock = Block(LEAF,blockPos + positionInWorld);
        addBlock(leafBlock);
    }
}
