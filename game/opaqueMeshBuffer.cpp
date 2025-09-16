#include "opaqueMeshBuffer.h"

BufferInt OpaqueMeshBuffer::getChunkDataSize(Chunk *chunk)
{
    return chunk->getOpaqueMeshSize();
}