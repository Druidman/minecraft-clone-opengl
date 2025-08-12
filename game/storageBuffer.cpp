#include "storageBuffer.h"
#include "world.h"
#include "player.h"

void StorageBuffer::init(World* world){
    this->world = world;
}

void StorageBuffer::setBindingPoint(int port)
{
    GLCall( glBindBufferBase(GL_SHADER_STORAGE_BUFFER, port, m_bo) );
}

BufferInt StorageBuffer::getChunkDataSize(Chunk *chunk)
{
    return sizeof(StorageBufferType);
}

bool StorageBuffer::updateChunkBuffer(Chunk *chunk)
{
    std::cout << "Storage Buffer insertion: \n";
    std::cout << "Buffer Zone\n";
    std::cout << "First: " << chunk->bufferZone[bufferType].first;
    std::cout << "\nSecond: " << chunk->bufferZone[bufferType].second;
    std::cout << "\n";
    BufferInt dataSize = getChunkDataSize(chunk);
    if (dataSize > chunk->bufferZone[bufferType].second - chunk->bufferZone[bufferType].first){
        // chunk is to big so we either find new free zone OR reallocate buffer
        ExitError("BUFFER " + bufferType,"can't delete");
        return false;

        // ! in all of these scenarios we need to remove chunk from its current location !
        deleteChunkFromBuffer(chunk);

        // lets try to find new freeZone

        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            expandBufferByChunk(chunk);
        }
        else if (assignRes == -1){
            std::cout << "update -1\n";
            return false;
        }

        insertChunkToBuffer(chunk);
    }\
    
    std::cout << world->player->camera->position.x << " " << world->player->camera->position.y << "\n";
    if (!updateData<StorageBufferType>(
        glm::vec4(chunk->position - world->player->camera->position,0.0), //  
        chunk->bufferZone[bufferType].first,
        chunk->bufferZone[bufferType].second
    )){
        return false;
    }
    
    return true;
}

