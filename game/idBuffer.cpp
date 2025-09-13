#include "idBuffer.h"
#include <GLFW/glfw3.h>

bool IdBuffer::markData(BufferInt markStart, BufferInt markEnd)
{
     // now lets mark elements as unactive
    // to mark data as delete we need to change it to have -1.0 on w element
 
    if (
        markEnd < markStart ||
        markStart > this->bufferTarget->bufferSize ||
        markEnd > this->bufferTarget->bufferSize ||
        markEnd == 0 ||
        markEnd - markStart == 0
    ){
        std::cout  << "FAIL MARK\n" << markStart << " " << markEnd << "\n";
        
        return false;
    }
    
    std::vector<int> data((markEnd - markStart) / sizeof(int),UNACTIVE_MESH_ELEMENT);
 
    return this->bufferTarget->uploadData(data.data(),data.size() * sizeof(int), markStart);
}

bool IdBuffer::updateChunkBuffer(Chunk *chunk)
{

    if (!chunk->hasBufferSpace[this->chunkBufferType]){
        ExitError("ID_BUFFER","UPDATE CALLED ON NOT INSERTED CHUNK");
    }
    BufferInt dataSize = (chunk->getMeshSize() / sizeof(CHUNK_MESH_DATATYPE)) * sizeof(int);
    if (chunk->bufferZone[this->chunkBufferType].first + dataSize > chunk->bufferZone[this->chunkBufferType].second){
        // chunk is to big so we either find new free zone OR reallocate buffer

        // ! in all of these scenarios we need to remove chunk from its current location !
        if (!deleteChunkFromBuffer(chunk)){
            ExitError("ID_BUFFER","Error deleting chunk in update func");
        };

        // lets try to find new freeZone
        std::cout << "assigning new zone for a chunk in IDBUFFER\n";
        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            
            if (!expandBufferByChunk(chunk)){
                ExitError("ID_BUFFER","Error expanding buffer in update func");
            };
        }
        else if (assignRes == -1){
            std::cout << "update -1\n";
            return false;
        }

        
        return updateChunkBuffer(chunk);
    }
    
    std::vector<int> data(
        (chunk->bufferZone[this->chunkBufferType].second - chunk->bufferZone[this->chunkBufferType].first) / sizeof(CHUNK_MESH_DATATYPE),
        chunk->bufferZone[STORAGE_BUFFER].first / sizeof(StorageBufferType)
    );
    

    if (!this->cpuBuffer.uploadData(
            data.data(),
            chunk->bufferZone[this->chunkBufferType].second - chunk->bufferZone[this->chunkBufferType].first,
            chunk->bufferZone[this->chunkBufferType].first
        )
    ){
        return false;
    };
    gpuBufferRequiresRefill = true;
    return true;
    
}

bool IdBuffer::insertChunksToBuffer(std::vector<Chunk *> *chunks)
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

bool IdBuffer::fillGpuBuffer()
{
    if (!gpuBufferRequiresRefill){
        return true;
    }
    if (gpuBuffer.bufferSize != this->cpuBuffer.bufferSize){
        if (!gpuBuffer.allocateBuffer(this->cpuBuffer.bufferSize)){
            return false;
        };
    }

    if (!gpuBuffer.uploadData(this->cpuBuffer.getBufferContent(), this->cpuBuffer.bufferSize, 0)){
        return false;
    };
 
    std::cout << "ID_GPU_BUFFER_CALL\n";
    gpuBufferRequiresRefill = false;
    return true;

}
