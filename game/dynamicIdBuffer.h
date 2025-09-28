#ifndef DYNAMIC_ID_BUFFER_H
#define DYNAMIC_ID_BUFFER_H

#include "dynamicBuffer.h"


class DynamicIdBuffer : public DynamicBuffer{
    private:
        int UNACTIVE_ELEMENT = -1; // masks face bits to be 6
    protected:
        
        virtual bool markData(BufferInt markStart, BufferInt markEnd) override;
        virtual bool requiresContiguousMemoryLayout() override;
        

        virtual std::string getBufferTypeString() override;

        virtual void postChunkUpdateFunction(){};
    public:
        DynamicIdBuffer(Buffer* buffer, ChunkBufferType idBufferType, bool deleteData = false) : DynamicBuffer(buffer, idBufferType, deleteData){
            if (idBufferType != OPAQUE_ID_BUFFER && idBufferType != TRANSPARENT_ID_BUFFER){
                ExitError(getBufferTypeString(), "WRONG CONSTRUCTOR ID BUFFER TYPE PARAM");
            }
            BUFFER_PADDING = 20; // expressed in %
            CHUNK_PADDING = 10;
            BUFFER_EXPANSION_RATE = 20;

            
        }
    
    public:
        
        virtual bool updateChunkBuffer(Chunk* chunk) override;
        virtual bool insertChunksToBuffer(std::vector<Chunk*> *chunks) override;
        
};
#endif