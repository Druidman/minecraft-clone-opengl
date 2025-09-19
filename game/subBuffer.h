#ifndef SUB_BUFFFER_H
#define SUB_BUFFFER_H

#include "buffer.h"
#include "betterGL.h"


// subBuffer is a buffer sub-class that takes buffer: start-end and fullbuffer pointer
// it performs operations on given full buffer zone not in any other
// ! buffer zone can't grow !
// ! subBuffer isn't dynamic in any way !


class SubBuffer : public Buffer{
    private:
        Buffer *buffer;
        BufferInt bufferStart;
        BufferInt bufferEnd;
    public:
        SubBuffer(BufferInt bufferStart, BufferInt bufferEnd, Buffer *buffer) : Buffer(){
            if (
                bufferEnd < bufferStart ||
                bufferStart > buffer->bufferSize ||
                bufferEnd > buffer->bufferSize                
            ){
                ExitError("SUB_BUFFER", "wrong buffer zone");
            }

            this->bufferSize = bufferEnd - bufferStart;
            this->bufferStart = bufferStart;
            this->bufferEnd = bufferEnd;
            this->buffer = buffer;
        }
        virtual bool uploadData(const void* data, BufferInt size, BufferInt start){
            if (
                start + bufferStart > bufferEnd ||
                start + bufferStart + size > bufferEnd 
            ){
                std::cout << "Forbidden subBuffer action in uploading data: " \
                << start << " " << size << " " << bufferSize << \
                " " << bufferStart << " " << bufferEnd << "\n";
                return false;
            }
            return buffer->uploadData(data, size, start + this->bufferStart);
        };

        virtual bool allocateBuffer(BufferInt size){ 
            ExitError("SUB_BUFFER", "Buffer allocation in subBuffer. This action is forbidden");
            return false;
        };
        virtual bool expandBuffer(BufferInt by){
            ExitError("SUB_BUFFER", "Buffer expansion in subBuffer. This action is forbidden");
            return false;
        };
        virtual bool moveBufferPart(){
            ExitError("SUB_BUFFER", "Buffer movement in subBuffer. This action is forbidden");
            return false;
        };

};
#endif