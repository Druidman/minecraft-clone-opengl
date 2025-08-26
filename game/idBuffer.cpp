#include "idBuffer.h"

bool IdBuffer::fillBufferWithChunks(std::vector<Chunk*> *chunks, size_t elements){
    // if (elements != this->bufferContent.size()){
    this->bufferContent.clear();
    this->bufferContent.resize(elements, -1);
    // } 
    for (Chunk* chunk : *chunks){
        
        if (!chunk->hasBufferSpace[GL_UNIFORM_BUFFER]){
            ExitError("ID_BUFFER", "CHUNK NOT IN STORAGE BUFFER ADDED TO ID BUFFER");
        }
        BufferInt startInstance = chunk->bufferZone[GL_ARRAY_BUFFER].first / sizeof(CHUNK_MESH_DATATYPE);
        BufferInt instanceCount = (chunk->bufferZone[GL_ARRAY_BUFFER].second - chunk->bufferZone[GL_ARRAY_BUFFER].first) / sizeof(CHUNK_MESH_DATATYPE);
        int id = chunk->bufferZone[GL_UNIFORM_BUFFER].first / sizeof(StorageBufferType);
        
        // if (id != bufferContent[startInstance]){
        if (bufferContent[startInstance] != -1){
            ExitError("ID_BUFFER","filling already filled buffer zone");
        }
        std::cout << "NEW ID: " << id << "\n";
        for (int i = startInstance; i<startInstance + instanceCount; i++){
            
            bufferContent[i] = id;
        };
      
        
            
        // }
        
        
    }
    std::cout << "fill\n";
    allocateBuffer(elements * sizeof(int));
    return fillData<int>(&bufferContent);


    
}   