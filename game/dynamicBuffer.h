#ifndef DYNAMICBUFFER_H
#define DYNAMICBUFFER_H

#include <vector>
#include <utility>
#include <string>

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
        
        CHUNK_MESH_DATATYPE UNACTIVE_MESH_ELEMENT = 393216; // masks face bits to be 6
        bool deleteData = false;

    protected:
        std::vector< std::pair<BufferInt, BufferInt> > bufferFreeZones;
        BufferInt BUFFER_PADDING = 0; // expressed in %
        BufferInt CHUNK_PADDING = 0;
        BufferInt BUFFER_EXPANSION_RATE = 0;
        
        bool bufferRequiresMovingElements = false;

        int bufferCalls = 0;
        
    protected:
        BufferInt getBufferPadding(BufferInt size);
        BufferInt getChunkPadding(BufferInt size);
        void mergeFreeZones();
        void expandBufferByChunk(Chunk* chunk);
        
        int assignChunkBufferZone(Chunk* chunk);
        int getChunkBufferSpaceIndex(Chunk* chunk);
        
        GLenum getBufferType();
        void expandBuffer(BufferInt by);

        bool markData(BufferInt markStart, BufferInt markEnd);

        bool moveBufferPart(BufferInt from, BufferInt to); // moves all data from -> to 

    protected:
        DynamicBuffer(GLenum bufferT, bool deleteData = false) : Buffer(bufferT){
            if (
                bufferT != GL_ARRAY_BUFFER && 
                bufferT != GL_DRAW_INDIRECT_BUFFER &&
                bufferT != GL_SHADER_STORAGE_BUFFER &&
                bufferT != GL_UNIFORM_BUFFER
            ){
                ExitError("DYNAMIC_BUFFER", "Invalid buffer type for DynamicBuffer ONLY MESH_BUFFER, INDIRECT_BUFFER and STORAGE_BUFFER are allowed");
                return;
            }
            this->deleteData = deleteData;
        };
    protected:
        virtual bool requiresContiguousMemoryLayout() = 0;
        virtual BufferInt getChunkDataSize(Chunk* chunk) = 0;

        virtual std::string getBufferTypeString() = 0;

        
        
    public:
        int getBufferCallsNum();
        void bind() { Buffer::bind(); };
        void getId() { Buffer::getId(); };
        BufferInt getBufferSize(){return Buffer::getBufferSize();};
        bool allocateDynamicBuffer(BufferInt meshSize);

        bool insertChunkToBuffer(Chunk* chunk);
        bool deleteChunkFromBuffer(Chunk* chunk, bool merge = false);
    public:
        virtual bool updateChunkBuffer(Chunk* chunk) = 0;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) = 0;
};
#endif