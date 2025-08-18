#include "indirectBuffer.h"

bool IndirectBuffer::fillBufferWithChunks(std::vector<Chunk *> *chunks)
{
    if (chunks->size() == 0){
        return false;
    }
    this->bufferContent.clear();
    for (Chunk* chunk : *chunks){
        this->bufferContent.emplace_back(
            BLOCK_FACE_VERTICES_COUNT,
            (uint)chunk->transparentMesh.size() + (uint)chunk->opaqueMesh.size(),
            0,
            (uint)(chunk->bufferZone[GL_ARRAY_BUFFER].first / sizeof(CHUNK_MESH_DATATYPE))
        );
    }

    return fillData<DrawArraysIndirectCommand>(&this->bufferContent); // filling buffer
}
