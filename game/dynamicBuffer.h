#ifndef DYNAMICBUFFER_H
#define DYNAMICBUFFER_H

#include <vector>
#include <utility>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "buffer.h"
#include "betterGL.h"
#include "chunk.h"

/* FROM DOCS
`insertChunkToBuffer(Chunk* chunk)`   
   
1. give chunk start/end position in buffer (end would have extra safe zone)  
1.1. if end will exceed `MESHBUFFER` size then allocate buffer with newlny calculated size and fill all chunks again  
1.1. ! due to us having `SAFEZONE` in buffer and chunks there should not be really need to reallocation but we need to be ready  
 if it will happen !  
2. insert data to this memory   
  
`deleteChunkFromBuffer(Chunk* chunk)`:  
  
be called at chunk destructor  
1. append `FREEZONE(chunk->bufferStart, chunk->bufferEnd)` to `bufferFreeZones`
  
`updateChunkBuffer(Chunk* chunk)`:  
  
1. glSubData at chunk->start - chunk->end (replaces whole mesh)  
*/

// we inherit as private to district acces to the Buffer methods on MeshBuffer object
class DynamicBuffer : protected Buffer {
    private:
        
        BufferInt BUFFER_PADDING = 20; // expressed in %
        BufferInt CHUNK_PADDING = 10;

        // keeps free zones in bytes as pairs: (B_start, B_end)
        // if empty then entire buffer taken
        std::vector< std::pair<BufferInt, BufferInt> > bufferFreeZones;
    protected:
        void mergeFreeZones();
        void expandBufferByChunk(Chunk* chunk);
        
        int assignChunkBufferZone(Chunk* chunk);
        int getChunkBufferSpaceIndex(Chunk* chunk);
        
        GLenum getBufferType();

    protected:
        DynamicBuffer(GLenum bufferT) : Buffer(bufferT){
            if (
                bufferT != GL_ARRAY_BUFFER && 
                bufferT != GL_DRAW_INDIRECT_BUFFER &&
                bufferT != GL_SHADER_STORAGE_BUFFER
            ){
                ExitError("DYNAMIC_BUFFER", "Invalid buffer type for DynamicBuffer ONLY MESH_BUFFER, INDIRECT_BUFFER and STORAGE_BUFFER are allowed");
                return;
            }
        };
    protected:
        virtual BufferInt getChunkDataSize(Chunk* chunk) = 0;
    public:
        void bind() { Buffer::bind(); };
        void getId() { Buffer::getId(); };
        bool allocateDynamicBuffer(BufferInt meshSize);

        bool insertChunkToBuffer(Chunk* chunk);
        bool deleteChunkFromBuffer(Chunk* chunk);

    public:
        virtual bool updateChunkBuffer(Chunk* chunk) = 0;
};
#endif