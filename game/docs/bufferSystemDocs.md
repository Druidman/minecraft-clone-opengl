# EFFICIENT BUFFER HANDLING SYSTEM

# idea
we will have: `MESHBUFFER, INDIRECTBUFFER, CHUNKSTORAGEBUFFER`   
new variables:   
     - vec `bufferFreeZones` - will keep all currently free zones in meshBuffer   
   
we would allocate on `MESHBUFFER`  as much space as base generated world mesh takes + something (safe zone)  
we would have function: insertChunkToBuffer(Chunk* chunk) which would:  
   
1. give chunk start/end position in buffer (end would have extra safe zone)  
1.1. if end will exceed `MESHBUFFER` size then allocate buffer with newlny calculated size and fill all chunks again  
1.1. ! due to us having `SAFEZONE` in buffer and chunks there should not be really need to reallocation but we need to be ready  
 if it will happen !  
2. insert data to this memory   
  
Another function: deleteChunkFromBuffer(Chunk* chunk) which would:  
  
be called at chunk destructor  
1. append `FREEZONE(chunk->bufferStart, chunk->bufferEnd)` to `bufferFreeZones`
  
Another function: updateChunkBuffer(Chunk* chunk) which would:  
  
1. glSubData at chunk->start - chunk->end (replaces whole mesh)  