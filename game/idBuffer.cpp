#include "idBuffer.h"

bool IdBuffer::fillBufferWithChunks(std::vector<Chunk*> *chunks, size_t elements){

    this->bufferContent.resize(elements, 0);
                    
    for (Chunk* chunk : *chunks){
        BufferInt startInstance = chunk->bufferZone[GL_ARRAY_BUFFER].first / sizeof(CHUNK_MESH_DATATYPE);
        BufferInt instanceCount = (chunk->bufferZone[GL_ARRAY_BUFFER].second - chunk->bufferZone[GL_ARRAY_BUFFER].first) / sizeof(CHUNK_MESH_DATATYPE);
        if (!chunk->hasBufferSpace[GL_UNIFORM_BUFFER]){
            ExitError("ID_BUFFER", "CHUNK NOT IN STORAGE BUFFER ADDED TO ID BUFFER");
        }
        int id = chunk->bufferZone[GL_UNIFORM_BUFFER].first / sizeof(StorageBufferType);
        if (id != bufferContent[startInstance]){
            for (int i = startInstance; i<startInstance + instanceCount; i++){
                bufferContent[i] = id;
            };
            
        }
        
        
    }
    return fillData<int>(&bufferContent);
}   