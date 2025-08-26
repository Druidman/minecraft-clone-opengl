#include "storageBuffer.h"

#include "world.h"
#include "player.h"

void StorageBuffer::init(World* world){
    this->world = world;
    this->bufferContent.resize(UNIFORM_BUFFER_LENGTH);
}

void StorageBuffer::setBindingPoint(int port)
{

    GLCall( glBindBufferBase(bufferType, port, m_bo) );
}

bool StorageBuffer::updateChunkBuffer(Chunk *chunk)
{
    if (!chunk->hasBufferSpace[bufferType]){
        ExitError("STORAGE_BUFFER","UPDATE CALLED ON UN INSERTED CHUNK");
    }
    if (!updateData<StorageBufferType>(
        glm::vec4(chunk->position - this->world->player->camera->position, 0.0),
        chunk->bufferZone[bufferType].first,
        chunk->bufferZone[bufferType].second
        )
    ){
        return false;
    };
    
    return true;
}

bool StorageBuffer::insertChunksToBuffer(std::vector<Chunk *> *chunks)
{
    if (chunks->size() == 0){
        return false;
    }

    for (Chunk* chunk : *chunks){
        if (!insertChunkToBuffer(chunk)){
            return false;
        }
        
        
    }
    

    return true;
}
