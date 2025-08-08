#include "meshBuffer.h"

bool MeshBuffer::allocateMeshBuffer(BufferInt meshSize)
{
    allocateBuffer(meshSize + (meshSize * (BUFFER_PADDING / 100))); // allocate with padding
    return true;
}