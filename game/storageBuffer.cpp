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

    return fillData<StorageBufferType>(&this->bufferContent); // filling buffer
}


