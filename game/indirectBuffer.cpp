#include "indirectBuffer.h"

BufferInt IndirectBuffer::getChunkDataSize(Chunk *chunk)
{
    return sizeof(DrawArraysIndirectCommand);
}

bool IndirectBuffer::requiresContiguousMemoryLayout()
{
    return true;
}

bool IndirectBuffer::updateChunkBuffer(Chunk *chunk)
{
    BufferInt dataSize = getChunkDataSize(chunk);
    if (dataSize > chunk->bufferZone[bufferType].second - chunk->bufferZone[bufferType].first){
        // chunk is to big so we either find new free zone OR reallocate buffer
        ExitError("BUffer","can't delete");
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
    }
    DrawArraysIndirectCommand data = {
        BLOCK_FACE_VERTICES_COUNT,
        (uint)chunk->transparentMesh.size() + (uint)chunk->opaqueMesh.size(),
        0,
        (uint)(chunk->bufferZone[GL_ARRAY_BUFFER].first / sizeof(CHUNK_MESH_DATATYPE))
    };
    std::cout << "Indirect Buffer insertion: \n";
    std::cout << "Buffer Zone\n";
    std::cout << "First: " << chunk->bufferZone[bufferType].first;
    std::cout << "\nSecond: " << chunk->bufferZone[bufferType].second;
    std::cout << "\nInstances: " << (uint)chunk->transparentMesh.size() + (uint)chunk->opaqueMesh.size();
    std::cout << "\nBaseInstance: " << (uint)(chunk->bufferZone[GL_ARRAY_BUFFER].first / sizeof(CHUNK_MESH_DATATYPE));
    std::cout << "\nBaseInstanceBytes: " << (uint)(chunk->bufferZone[GL_ARRAY_BUFFER].first);
    std::cout << "\n";

    if (!updateData<DrawArraysIndirectCommand>(
        data,
        chunk->bufferZone[bufferType].first, 
        chunk->bufferZone[bufferType].second
    )){
        return false;
    }
    
    return true;
}

bool IndirectBuffer::insertChunksToBuffer(std::vector<Chunk *> *chunks)
{
    return false;
}
