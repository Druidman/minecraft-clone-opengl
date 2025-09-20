#ifndef OPAQUE_ID_BUFFER_H
#define OPAQUE_ID_BUFFER_H

#include "dynamicIdBuffer.h"

class OpaqueIdBuffer : public DynamicIdBuffer {

    public:
        OpaqueIdBuffer(Buffer* buffer, bool deleteData = false) : DynamicIdBuffer(buffer, OPAQUE_ID_BUFFER, deleteData){};
};

#endif
