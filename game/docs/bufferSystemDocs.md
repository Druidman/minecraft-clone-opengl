# EFFICIENT BUFFER HANDLING SYSTEM

# idea
we will have: `MESHBUFFER, INDIRECTBUFFER, CHUNKSTORAGEBUFFER`   
new variables:   
     - vec `bufferFreeZones` - will keep all currently free zones in meshBuffer   
   
we would allocate on `MESHBUFFER`  as much space as base generated world mesh takes + something (safe zone)  

# Functions MESHBUFFER
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


# Functions RENDERER
`addChunkToBuffers(Chunk* chunk)`

1. MESHBUFFER -> insertChunkToBuffer()
2. INDIRECTBUFFER( Buffer ):
     DrawArraysIndirectCommand data = {
          BLOCK_FACE_VERTICES_COUNT,
          (uint)chunk->transparentMesh.size() + (uint)chunk->opaqueMesh.size(),
          0,
          (uint)(this->chunk.buffer_begin / sizeof(CHUNK_MESH_DATATYPE))
     };
3. CHUNKSTORAGEBUFFER( Buffer ):
     this->chunkStorageBuffer.addData<glm::vec4>(glm::vec4(chunk->position - this->world->player->camera->position,0.0)); 