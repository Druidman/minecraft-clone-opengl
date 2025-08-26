#include "storageBufferDynamic.h"

#include "world.h"
#include "player.h"

void StorageBufferDynamic::init(World* world){
    this->world = world;
}

void StorageBufferDynamic::setBindingPoint(int port)
{

    GLCall( glBindBufferBase(bufferType, port, m_bo) );
}

bool StorageBufferDynamic::updateChunkBuffer(Chunk* chunk){
    return false;
}

bool StorageBufferDynamic::insertChunksToBuffer(std::vector<Chunk*> *chunks){
    
    std::cout << "\nSTORAGE_BUFFER_UPDATE\n\n";
    if (chunks->size() == 0){
        std::cout << "size = 0\n";
        return false;
    }
    
    
    
    this->bufferContent.clear();
    this->bufferContent.resize(chunks->size(), glm::vec4(0.0f,0.0f,0.0f, 0.0f));
    
    
    
    for (Chunk* chunk : *chunks){
        if (!chunk->hasBufferSpace[GL_ARRAY_BUFFER]){
            continue;
        }
        if (!chunk->hasBufferSpace[bufferType]){
            std::cout << "storageBuffer assigning\n"; 
            int zone = assignChunkBufferZone(chunk);
            
            if (zone == -2 || zone == -1){
                ExitError("STORAGE_BUFFER", "error assigning chunk buffer zone");
            }   
            std::cout << "ASSIGNED: " << chunk->bufferZone[bufferType].first << " " << chunk->bufferZone[bufferType].second << "\n";
        }
        
        this->bufferContent[chunk->bufferZone[bufferType].first / sizeof(StorageBufferType)]= glm::vec4(chunk->position - this->world->player->camera->position,1.0);
    }

    for (glm::vec4 element : this->bufferContent){
        std::cout << "STORAGE_BUFFER_ELEMENT: " << \
        element.x << " " << \
        element.y << " " << \
        element.z << " ";
        if (element.w == 0.0){
            std::cout << "PLACEHOLDER\n";
        }
        else {
            std::cout << "\n";
        }
        
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


