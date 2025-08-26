#ifndef STORAGEBUFFERDYNAMIC_H
#define STORAGEBUFFERDYNAMIC_H

#include <vector>


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "betterGL.h"
#include "dynamicBuffer.h"
#include "chunk.h"
#include "buffer.h"
#include "world.h"


const int UNIFORM_BUFFER_LENGTH = 1024;
class StorageBufferDynamic : public DynamicBuffer {

    protected:
        virtual bool markData(BufferInt markStart, BufferInt markEnd) override;
        virtual BufferInt getChunkDataSize(Chunk* chunk) override {return sizeof(StorageBufferType);};
        virtual bool requiresContiguousMemoryLayout() override {return false;};
        
        virtual std::string getBufferTypeString() override {return "STORAGE_BUFFER";};
       
        
    private:
        glm::vec4 UNACTIVE_STORAGE_ELEMENT = glm::vec4(0.0, 0.0, 0.0, -1.0); 

        World *world;

        // this is relatively small data so we keep it here for optimized buffer insertions
        std::vector<StorageBufferType> bufferContent;
    public:
        StorageBufferDynamic() : DynamicBuffer(GL_UNIFORM_BUFFER, false){
            if (bufferType != GL_UNIFORM_BUFFER ){
                ExitError("STORAGE_BUFFER","Can't create storage buffer different than uniform buffer");
                return ;
            }
            BUFFER_PADDING = 0; // expressed in %
            CHUNK_PADDING = 0;
            BUFFER_EXPANSION_RATE = 0;
        };
        void init(World *world);
        void setBindingPoint(int port);

        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;

    
};
#endif