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

bool StorageBuffer::fillBufferWithChunks(std::vector<Chunk*> *chunks){
    if (chunks->size() == 0){
        return false;
    }
    this->bufferContent.clear();
    for (Chunk* chunk : *chunks){
        
        this->bufferContent.emplace_back(
            glm::vec4(chunk->position - this->world->player->camera->position,0.0)
        );
    }
    
    if (this->bufferType == GL_SHADER_STORAGE_BUFFER){
        return fillData<StorageBufferType>(&this->bufferContent); // filling buffer
    }
    else if (this->bufferType == GL_UNIFORM_BUFFER){
        return fillData<StorageBufferType>(&this->bufferContent); // filling buffer
        // allocateBuffer(UNIFORM_BUFFER_LENGTH * sizeof(StorageBufferType));
        // return updateData<StorageBufferType>(&this->bufferContent,0,this->bufferContent.size() * sizeof(StorageBufferType));
    }
    return false;
    

    
}


