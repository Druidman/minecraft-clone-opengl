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
    
    if (bufferType == GL_SHADER_STORAGE_BUFFER){
        return fillData<StorageBufferType>(&this->bufferContent); // filling buffer
    }
    else if (bufferType == GL_UNIFORM_BUFFER){
        allocateBuffer(UNIFORM_BUFFER_LENGTH * sizeof(StorageBufferType));
        return updateData<StorageBufferType>(&this->bufferContent, 0, this->bufferSize); // filling buffer
    }
    
    
    

    
}


