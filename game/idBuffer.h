#ifndef ID_BUFFER_H
#define ID_BUFFER_H
#include <vector>

#include "dynamicBuffer.h"
#include "gpuBuffer.h"
#include "chunk.h"
#include "buffer.h"

#include "opaqueIdBuffer.h"
#include "transparentIdBuffer.h"
#include "multiDynamicBuffer.h"

class IdBuffer : public MultiDynamicBuffer<CpuBuffer<int>, OpaqueIdBuffer, TransparentIdBuffer>{
    protected:
        bool gpuBufferRequiresRefill = false;
    protected:
        virtual void postChunkUpdateFunction() override;

        
    public:
        GpuBuffer gpuBuffer = GpuBuffer(GL_ARRAY_BUFFER);
        CpuBuffer<int> cpuBuffer = CpuBuffer<int>();
        
    public:
        IdBuffer() : MultiDynamicBuffer(&cpuBuffer){};

        bool fillGpuBuffer();
};

#endif



