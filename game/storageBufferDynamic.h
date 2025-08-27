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
        
        virtual BufferInt getChunkDataSize(Chunk* chunk) override{return sizeof(StorageBufferType);};
        virtual bool requiresContiguousMemoryLayout() override {return false;};
        
        virtual std::string getBufferTypeString() override {return "STORAGE_BUFFER";};

    private:

        World *world;
        bool bufferRequiresRefill = false;
        // this is relatively small data so we keep it here for optimized buffer insertions
        std::vector<StorageBufferType> bufferContent;
    public:
        StorageBufferDynamic(GLenum bufferType) : DynamicBuffer(bufferType,false){
            if (bufferType != GL_SHADER_STORAGE_BUFFER && bufferType != GL_UNIFORM_BUFFER ){
                ExitError("STORAGE_BUFFER","Can't create storage buffer different than uniform/shaderStorage buffer");
                return ;
            }
        };
        void init(World *world);
        void setBindingPoint(int port);

        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;

    
};
#endif