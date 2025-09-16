#include "transparentMeshBuffer.h"

BufferInt TransparentMeshBuffer::getChunkDataSize(Chunk *chunk)
{
    return chunk->getTransparentMeshSize();
}