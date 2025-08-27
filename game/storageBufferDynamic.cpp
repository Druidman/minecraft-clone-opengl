#include "storageBufferDynamic.h"

#include "world.h"
#include "player.h"

bool StorageBufferDynamic::markData(BufferInt markStart, BufferInt markEnd)
{
     // now lets mark elements as unactive
    // to mark data as delete we need to change it to have -1.0 on w element
 
    if (
        markEnd < markStart ||
        markStart > this->bufferSize ||
        markEnd > this->bufferSize
    ){
        std::cout  << "FAIL MARK\n";
        return false;
    }
    
    std::vector<StorageBufferType> data((markEnd - markStart) / sizeof(StorageBufferType),UNACTIVE_MESH_ELEMENT);
 
    return updateData<StorageBufferType>(&data,markStart, markEnd); 
}

void StorageBufferDynamic::init(World *world)
{
    this->world = world;
    this->bufferContent.resize(UNIFORM_BUFFER_LENGTH);
}

void StorageBufferDynamic::setBindingPoint(int port)
{

    GLCall( glBindBufferBase(bufferType, port, m_bo) );
}

bool StorageBufferDynamic::updateChunkBuffer(Chunk *chunk)
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

bool StorageBufferDynamic::insertChunksToBuffer(std::vector<Chunk *> *chunks)
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
