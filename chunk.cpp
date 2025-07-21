#include "chunk.h"
#include "world.h"

Chunk::Chunk(glm::vec3 chunkPosition, World *world)
{
    this->position = chunkPosition;
    this->world = world;
    addBlockPlatform();
}

bool Chunk::isBlockTransparent(Block *block)
{
    if (block->type == WATER || block->type == LEAF)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::optional<Block *> Chunk::getBlock(int plat, int row, int col, bool noneBlock)
{
    // if (plat >= blocks.size()){
    //     int platformsToAdd = plat - this->blocks.size() + 1;
    //     for (int i = 0; i < platformsToAdd; i++)
    //     {
    //         createBlockPlatform();
    //     }
    // } //just so chunks feel infinite
    
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
    // std::cout << "p: " << platform << " c: " << this->position.x << " r: " << row << "\n";
    return getBlock(platform, row, col, noneBlock);
}

void Chunk::addBlockFace(BlockFace face, Block *block, std::vector<VertexDataInt> *buffer)
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

    VertexDataInt packedFloat;
    std::memcpy(&packedFloat,&data,sizeof(float));
    buffer->push_back(packedFloat);
}

bool Chunk::canAddFace(BlockFace face, Block *currentBlock)
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
    // lets check if face is on border of world
    if (checkBlockFacePos.y == 0 || checkBlockFacePos.x == 0 || checkBlockFacePos.z == 0 || checkBlockFacePos.x == world->WIDTH || checkBlockFacePos.z == world->WIDTH){
        return false; //face is on world border
    }
    
    std::optional<Block *> res = this->world->getBlockByPos(checkBlockPos);
    if (!res.has_value())
    {
        return true;
    }
    
    Block *checkBlock = res.value();

    if (!isBlockTransparent(checkBlock))
    {
        return false;
        
    }
    if (isBlockTransparent(currentBlock)){
        return false;
    }

    return true;
    
    
}

void Chunk::genChunkMesh()
{
    opaqueFacesData.clear();

    for (int platform = 0; platform < blocks.size(); platform++)
    {

        for (int row = 0; row < blocks[platform].size(); row++)
        {

            for (int col = 0; col < blocks[platform][row].size(); col++)
            {
                std::optional<Block *> res = getBlock(platform, row, col);
                if (!res.has_value())
                {
                    continue;
                }
                Block *block = res.value();

                std::vector<VertexDataInt> *buffer;
                if (block->type == WATER || block->type == LEAF)
                {
                    buffer = &transparentFacesData;
                }
                else
                {
                    buffer = &opaqueFacesData;
                }

                // top
                if (canAddFace(TOP_FACE, block))
                {
                    addBlockFace(TOP_FACE, block, buffer);
                }
                // bottom
                if (canAddFace(BOTTOM_FACE, block))
                {
                    addBlockFace(BOTTOM_FACE, block, buffer);
                }
                // front
                if (canAddFace(FRONT_FACE, block))
                {
                    addBlockFace(FRONT_FACE, block, buffer);
                }
                // back
                if (canAddFace(BACK_FACE, block))
                {
                    addBlockFace(BACK_FACE, block, buffer);
                }
                // left
                if (canAddFace(LEFT_FACE, block))
                {
                    addBlockFace(LEFT_FACE, block, buffer);
                }
                // right
                if (canAddFace(RIGHT_FACE, block))
                {
                    addBlockFace(RIGHT_FACE, block, buffer);
                }
            }
        }
    }
}

void Chunk::renderOpaque(VertexBuffer *vboInst)
{
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    vboInst->fillData<VertexDataInt>(opaqueFacesData);
    glDrawElementsInstanced(GL_TRIANGLES, BLOCK_FACE_INDICES.size(), GL_UNSIGNED_INT, 0, opaqueFacesData.size());
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
};
void Chunk::renderTransparent(VertexBuffer *vboInst)
{
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    vboInst->fillData<VertexDataInt>(transparentFacesData);
    glDrawElementsInstanced(GL_TRIANGLES, BLOCK_FACE_INDICES.size(), GL_UNSIGNED_INT, 0, transparentFacesData.size());
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
glm::vec3 Chunk::getPos(int platform, int row, int column){
    float y = platform + 0.5 + this->position.y;
    float x = column + 0.5 + (this->position.x - (CHUNK_WIDTH / 2));
    float z = row + 0.5 + (this->position.z - (CHUNK_WIDTH / 2));

    return glm::vec3(x,y,z);

}

void Chunk::addBlock(Block block)
{
    if (block.type == NONE_BLOCK)
    {
        return;
    }
    if (block.position.x > this->position.x + CHUNK_WIDTH ||
        block.position.x < this->position.x - CHUNK_WIDTH ||
        block.position.z > this->position.z + CHUNK_WIDTH ||
        block.position.z < this->position.z - CHUNK_WIDTH ||
        block.position.y < this->position.y)
    {
        // ExitError("CHUNK", "adding block to chunk outside its borders");
        return;
    }
    int platform = block.position.y - 0.5 - position.y;
    int column = block.position.x - 0.5 - (position.x - (CHUNK_WIDTH / 2));
    int row = block.position.z - 0.5 - (position.z - (CHUNK_WIDTH / 2));

    if (row > CHUNK_WIDTH - 1 ||
        column > CHUNK_WIDTH - 1 ||
        row < 0 ||
        column < 0 ||
        platform < 0)
    {
        // ExitError("CHUNK", "invalid row or column");
        return;
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
            Block block = Block(NONE_BLOCK,getPos(platform,row,column));
            this->blocks[platform][row].push_back(block);
        }
    }
    
    this->blocks[platform][row][column] = block;
}

void Chunk::addBlockPlatform()
{
    std::vector<std::vector<Block>> platform = std::vector<std::vector<Block>>(1, std::vector<Block>(1,Block(NONE_BLOCK,getPos(this->blocks.size(),0,0))));
    this->blocks.push_back(platform);
}

void Chunk::addBlockRow(int platform)
{
    std::vector<Block> row = std::vector<Block>(1,Block(NONE_BLOCK,getPos(platform,this->blocks[platform].size(),0)));
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
                addBlock(Block(WATER,getPos(platform,row,col)));
                
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
        addBlock(Block(WOOD,blockPos + positionInWorld));
    }
    for (glm::vec3 blockPos : TREE::leafPositions){
        addBlock(Block(LEAF,blockPos + positionInWorld));
    }
}
