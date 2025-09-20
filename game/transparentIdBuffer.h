#ifndef TRANSPARENT_ID_BUFFER_H
#define TRANSPARENT_ID_BUFFER_H

#include "dynamicIdBuffer.h"

class TransparentIdBuffer : public DynamicIdBuffer {

    public:
    TransparentIdBuffer(Buffer* buffer, bool deleteData = false) : DynamicIdBuffer(buffer, TRANSPARENT_ID_BUFFER, deleteData){};
};

#endif