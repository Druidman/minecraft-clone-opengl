#ifndef MESHBUFFER_H
#define MESHBUFFER_H

#include <vector>
#include <utility>
#include <string>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/type_ptr.hpp"

#include "dynamicBuffer.h"
#include "betterGL.h"
#include "chunk.h"
#include "gpuBuffer.h"
#include "subBuffer.h"
#include "buffer.h"

#include "opaqueMeshBuffer.h"
#include "transparentMeshBuffer.h"

#include "multiDynamicBuffer.h"

class MeshBuffer : public MultiDynamicBuffer<GpuBuffer, OpaqueMeshBuffer, TransparentMeshBuffer>{
    
        
    public:
        GpuBuffer buffer = GpuBuffer(GL_ARRAY_BUFFER);
    public:
        MeshBuffer() : MultiDynamicBuffer(&buffer){

        };
        
};
#endif