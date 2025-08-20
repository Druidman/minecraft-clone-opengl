#include "storageBuffer.h"

#include "world.h"
#include "player.h"

void StorageBuffer::init(World* world){
    this->world = world;
}

void StorageBuffer::setBindingPoint(int port)
{

    GLCall( glBindBufferBase(bufferType, port, m_bo) );
}

bool StorageBuffer::insertChunksToBuffer(std::vector<Chunk*> *chunks){
    if (chunks->size() == 0){
        return false;
    }
    this->bufferContent.clear();
    BufferInt dataFilled = 0;
    for (Chunk* chunk : *chunks){
        
        this->bufferContent.emplace_back(
            glm::vec4(chunk->position - this->world->player->camera->position,0.0)
        );
        chunk->bufferZone[bufferType].first = dataFilled;
        chunk->bufferZone[bufferType].second = chunk->bufferZone[bufferType].first + sizeof(StorageBufferType);

        chunk->hasBufferSpace[bufferType] = true;

        dataFilled += sizeof(StorageBufferType);
    }
    
   
    return fillData<StorageBufferType>(&this->bufferContent); // filling buffer
    
    

    
}


