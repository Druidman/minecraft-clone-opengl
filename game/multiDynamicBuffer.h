#ifndef MULTI_DYNAMIC_BUFFER_H
#define MULTI_DYNAMIC_BUFFER_H


#include <type_traits>

#include "dynamicBuffer.h"
#include "subBuffer.h"

#define MDM_TEMPLATE \
            template < \
                FullBufferConcept FullBufferType, \
                DynamicBufferConcept FirstDynamicBufferType, \
                DynamicBufferConcept SecondDynamicBufferType \
            > 
#define MDM_CLASS MultiDynamicBuffer<FullBufferType, FirstDynamicBufferType, SecondDynamicBufferType>


template<typename T>
concept FullBufferConcept = std::is_same_v<T, GpuBuffer> || std::is_same_v<T, CpuBuffer<float>> || std::is_same_v<T, CpuBuffer<int>>;

template<typename T>
concept DynamicBufferConcept = std::is_base_of_v<DynamicBuffer, T>;



MDM_TEMPLATE
class MultiDynamicBuffer{
    protected:
        virtual void postChunkUpdateFunction(){};
    protected:
        FullBufferType* fullBuffer;

        SubBuffer firstSubBuffer = SubBuffer(0,0,fullBuffer); // placeHolder
        SubBuffer secondSubBuffer = SubBuffer(0,0,fullBuffer);  // placeHolder

        FirstDynamicBufferType firstDynamicSubBuffer = FirstDynamicBufferType(&firstSubBuffer,true); // placeHolder
        SecondDynamicBufferType secondDynamicSubBuffer = SecondDynamicBufferType(&secondSubBuffer, true); // placeHolder

    public:
        MultiDynamicBuffer(
            FullBufferType* fullBuffer
        ) : fullBuffer(fullBuffer){};


        BufferInt getBufferSize(){ return fullBuffer->bufferSize; };


        bool allocateBuffer(BufferInt firstBufferSize, BufferInt secondBufferSize);

        bool insertChunkToBuffer(Chunk* chunk);
        bool deleteChunkFromBuffer(Chunk* chunk, bool merge = false);
           
        bool updateChunkBuffer(Chunk* chunk);
        bool insertChunksToBuffer(std::vector<Chunk*> *chunks);
};




MDM_TEMPLATE
inline bool MDM_CLASS::allocateBuffer(BufferInt firstBufferSize, BufferInt secondBufferSize)
{
    BufferInt realfirstSize = (firstBufferSize * 1.5);
    BufferInt realSecondSize = (secondBufferSize * 1.5);

    realfirstSize -= realfirstSize % 2;
    realSecondSize -= realSecondSize % 2;
    
    if (!this->fullBuffer->allocateBuffer(realfirstSize + realSecondSize)){
        ExitError("MESH_BUFFER", "SMTH WRONG WITH alloc");
    };
    

    this->firstSubBuffer = SubBuffer(
        0,
        realfirstSize, 
        this->fullBuffer
    );
    this->secondSubBuffer = SubBuffer(
        realfirstSize, 
        this->fullBuffer->bufferSize, 
        this->fullBuffer
    );

    this->firstDynamicSubBuffer = FirstDynamicBufferType(&firstSubBuffer, true); 
    this->secondDynamicSubBuffer = SecondDynamicBufferType(&secondSubBuffer, true);

    
    this->firstDynamicSubBuffer.clearBuffer();
    this->secondDynamicSubBuffer.clearBuffer();
    return true;
}

MDM_TEMPLATE
inline bool MDM_CLASS::insertChunkToBuffer(Chunk *chunk)
{
    bool res = true;
    res &= this->firstDynamicSubBuffer.insertChunkToBuffer(chunk);
    res &= this->secondDynamicSubBuffer.insertChunkToBuffer(chunk);
    postChunkUpdateFunction();
    return res;
}

MDM_TEMPLATE
inline bool MDM_CLASS::deleteChunkFromBuffer(Chunk *chunk, bool merge)
{
    bool res = true;
    if (chunk->hasBufferSpace[this->firstDynamicSubBuffer.chunkBufferType]){
        res &= this->firstDynamicSubBuffer.deleteChunkFromBuffer(chunk, merge);
    }
    if (chunk->hasBufferSpace[this->secondDynamicSubBuffer.chunkBufferType]){
        res &= this->secondDynamicSubBuffer.deleteChunkFromBuffer(chunk, merge);
    }
    postChunkUpdateFunction();
    return res;
}

MDM_TEMPLATE
inline bool MDM_CLASS::updateChunkBuffer(Chunk *chunk)
{
    bool res = true;
    if (chunk->hasBufferSpace[this->firstDynamicSubBuffer.chunkBufferType]){
        res &= this->firstDynamicSubBuffer.updateChunkBuffer(chunk);
    }
    if (chunk->hasBufferSpace[this->secondDynamicSubBuffer.chunkBufferType]){
        res &= this->secondDynamicSubBuffer.updateChunkBuffer(chunk);
    }
    postChunkUpdateFunction();
    return res;
}

MDM_TEMPLATE
inline bool MDM_CLASS::insertChunksToBuffer(std::vector<Chunk *> *chunks)
{
    bool res = true;
    res &= this->firstDynamicSubBuffer.insertChunksToBuffer(chunks);
    res &= this->secondDynamicSubBuffer.insertChunksToBuffer(chunks);
    postChunkUpdateFunction();

    return res;
}
#endif