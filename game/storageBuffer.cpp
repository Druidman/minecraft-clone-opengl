#include "storageBuffer.h"

#include "world.h"
#include "player.h"

void StorageBuffer::init(World* world){
    this->world = world;
}

void StorageBuffer::setBindingPoint(int port)
{

    GLCall( glBindBufferBase(bufferType, port, m_bo) );
}

bool StorageBuffer::updateChunkBuffer(Chunk* chunk){
    return false;
}

bool StorageBuffer::insertChunksToBuffer(std::vector<Chunk*> *chunks){
    std::cout << "\nSTORAGE_BUFFER_UPDATE\n\n";
    if (chunks->size() == 0){
        std::cout << "size = 0\n";
        return false;
    }
    
    
    if (chunks->size() != this->bufferContent.size()){
        // ExitError("STORAGE_BUFFER","RESIZE??");
        this->bufferContent.clear();
        this->bufferContent.resize(chunks->size(), glm::vec4(0.0f,0.0f,0.0f, 0.0f));
    }
    else {
        this->bufferContent.clear();
    }
    
    
    for (Chunk* chunk : *chunks){
        if (!chunk->hasBufferSpace[bufferType]){
            std::cout << "storageBuffer assigning\n"; 
            int zone = assignChunkBufferZone(chunk);
            
            if (zone == -2 || zone == -1){
                ExitError("STORAGE_BUFFER", "error assigning chunk buffer zone");
            }   
            std::cout << "ASSIGNED: " << chunk->bufferZone[bufferType].first << " " << chunk->bufferZone[bufferType].second << "\n";
        }
        std::cout << "Update buffer content: " << chunk->bufferZone[bufferType].first << " first " << chunk->bufferZone[GL_ARRAY_BUFFER].first << "\n";
        this->bufferContent[chunk->bufferZone[bufferType].first / sizeof(StorageBufferType)]= glm::vec4(chunk->position - this->world->player->camera->position,0.0);
    }

  
    std::cout << "insert to buffers\n";
    if (bufferType == GL_SHADER_STORAGE_BUFFER){
        return updateData<StorageBufferType>(&this->bufferContent, 0, this->bufferSize); // filling buffer
    }
    else if (bufferType == GL_UNIFORM_BUFFER){
        
        return updateData<StorageBufferType>(&this->bufferContent, 0, this->bufferSize); // filling buffer
    }
    else{
        return false;
    }
}


