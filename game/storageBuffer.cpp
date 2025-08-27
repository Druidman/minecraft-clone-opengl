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


bool StorageBuffer::insertChunksToBuffer(std::vector<Chunk*> *chunks){
    

    
    std::cout << "\nSTORAGE_BUFFER_UPDATE\n\n";
    if (chunks->size() == 0){
        std::cout << "size = 0\n";
        return false;
    }
    
    
    if (chunks->size() != this->bufferContent.size()){
        this->bufferContent.clear();
        this->bufferContent.resize(chunks->size(), glm::vec4(0.0f,0.0f,0.0f, 0.0f));
    }
    else {
        this->bufferContent.clear();
    }
    
    int ind = 0;
    for (Chunk* chunk : *chunks){
        if (!chunk->hasBufferSpace[GL_ARRAY_BUFFER]){
            continue;
        }
        
        this->bufferContent[ind]= glm::vec4(chunk->position - this->world->player->camera->position,0.0);
        ind++;
    }

    return fillData<StorageBufferType>(&this->bufferContent);
}


