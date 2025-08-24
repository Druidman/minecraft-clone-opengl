#include "idBuffer.h"

bool IdBuffer::fillBufferWithChunks(std::vector<Chunk*> *chunks, size_t elements){

    if (elements != this->bufferContent.size()){
        this->bufferContent.resize(elements, 0);
    }
    for (Chunk* chunk : *chunks){
        if (!chunk->hasBufferSpace[GL_ARRAY_BUFFER]){
            continue;
        }
        if (!chunk->hasBufferSpace[GL_UNIFORM_BUFFER]){
            ExitError("ID_BUFFER", "CHUNK NOT IN STORAGE BUFFER ADDED TO ID BUFFER");
        }
        std::cout << "IDBUFFERUPDATE: " <<  chunk->bufferZone[GL_ARRAY_BUFFER].first << "\n";
        BufferInt startInstance = chunk->bufferZone[GL_ARRAY_BUFFER].first / sizeof(CHUNK_MESH_DATATYPE);
        BufferInt instanceCount = (chunk->bufferZone[GL_ARRAY_BUFFER].second - chunk->bufferZone[GL_ARRAY_BUFFER].first) / sizeof(CHUNK_MESH_DATATYPE);
        int id = chunk->bufferZone[GL_UNIFORM_BUFFER].first / sizeof(StorageBufferType);
        // if (id != bufferContent[startInstance] || id != bufferContent[startInstance + instanceCount]){
        std::cout << "NEW ID: " << id << "\n";
        for (int i = startInstance; i<startInstance + instanceCount; i++){
            bufferContent[i] = id;
        };
        // }
        
            
        
        
        
    }

    return fillData<int>(&bufferContent);

    
}   