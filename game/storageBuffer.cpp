#include "storageBuffer.h"

#include "world.h"
#include "player.h"

bool StorageBuffer::markData(BufferInt markStart, BufferInt markEnd)
{
     // now lets mark elements as unactive
    // to mark data as delete we need to change it to have -1.0 on w element
 
    if (
        markEnd < markStart ||
        markStart > this->bufferTarget->bufferSize ||
        markEnd > this->bufferTarget->bufferSize
    ){
        std::cout  << "FAIL MARK\n";
        return false;
    }
    
    std::vector<StorageBufferType> data((markEnd - markStart) / sizeof(StorageBufferType),UNACTIVE_MESH_ELEMENT);
 
    return this->bufferTarget->uploadData(data.data(),data.size() * sizeof(StorageBufferType), markStart);
}

void StorageBuffer::init(World *world)
{
    this->world = world;
    this->bufferTarget->allocateBuffer(UNIFORM_BUFFER_LENGTH * sizeof(StorageBufferType));
    this->setBindingPoint(0);
}

void StorageBuffer::setBindingPoint(int port)
{

    GLCall( glBindBufferBase(GL_UNIFORM_BUFFER, port, gpuBuffer.getId()) );
}

bool StorageBuffer::updateChunkBuffer(Chunk *chunk)
{
    if (!chunk->hasBufferSpace[this->chunkBufferType]){
        ExitError("STORAGE_BUFFER","UPDATE CALLED ON UN INSERTED CHUNK");
    }
    glm::vec4 tmp = glm::vec4(chunk->position - this->world->player->camera->position, 0.0f);

    if (!this->bufferTarget->uploadData(
            &tmp,
            chunk->bufferZone[this->chunkBufferType].second - chunk->bufferZone[this->chunkBufferType].first,
            chunk->bufferZone[this->chunkBufferType].first
        )
    ){
        return false;
    };
    gpuBufferRequiresRefill = true;
    
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
    gpuBufferRequiresRefill = true;
    return true;
}

bool StorageBuffer::fillGpuBuffer()
{
    if (!gpuBufferRequiresRefill){
        return true;
    }
    
    gpuBuffer.allocateBuffer(UNIFORM_BUFFER_LENGTH * sizeof(StorageBufferType));
    
    if (!gpuBuffer.uploadData(this->bufferTarget->getBufferContent(), this->bufferTarget->bufferSize, 0)){
        std::cout << "upload fail\n";
        
        return false;
    };
    std::cout << "STORAGE_GPU_BUFFER_CALL\n";
    gpuBufferRequiresRefill = false;
    return true;

}
