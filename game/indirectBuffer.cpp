#include "indirectBuffer.h"
#include <vector>

bool IndirectBuffer::fillBufferWithChunks(std::vector<Chunk *> *chunks)
{
    if (chunks->size() == 0){
        return false;
    }
    this->bufferContent.clear();
    this->bufferContent.resize(chunks->size() * 2);
    int i = 0;
    for (Chunk* chunk : *chunks){
        this->bufferContent[i] = {
            (unsigned int)BLOCK_FACE_VERTICES_COUNT,
            (unsigned int)chunk->opaqueMesh.size(),
            0,
            (unsigned int)(chunk->bufferZone[MESH_BUFFER].first / sizeof(CHUNK_MESH_DATATYPE))
        };
        i++;
    }

    for (Chunk* chunk : *chunks){
        this->bufferContent[i] = {
            (unsigned int)BLOCK_FACE_VERTICES_COUNT,
            (unsigned int)chunk->transparentMesh.size(),
            0,
            (unsigned int)((chunk->bufferZone[MESH_BUFFER].first / sizeof(CHUNK_MESH_DATATYPE) + chunk->opaqueMesh.size()))
        };
        i++;
    }
    

    return fillData<DrawArraysIndirectCommand>(&this->bufferContent); // filling buffer
}
