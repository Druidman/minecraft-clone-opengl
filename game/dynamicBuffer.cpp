#include "dynamicBuffer.h"
#include "betterGL.h"

BufferInt DynamicBuffer::getBufferPadding(BufferInt size)
{
    BufferInt padding = static_cast<BufferInt>(size * (BUFFER_PADDING / 100.0));
    BufferInt alignment = sizeof(CHUNK_MESH_DATATYPE);
    BufferInt remainder = padding % alignment;
    if (remainder != 0){
        padding -= remainder;
    }
    return padding;
}
BufferInt DynamicBuffer::getChunkPadding(BufferInt size)
{
    BufferInt padding = static_cast<BufferInt>(size * (CHUNK_PADDING / 100.0));
    BufferInt alignment = sizeof(CHUNK_MESH_DATATYPE);
    BufferInt remainder = padding % alignment;
    if (remainder != 0){
        padding -= remainder;
    }
    return padding;
}

void DynamicBuffer::mergeFreeZones()
{
  
    if (
        this->bufferFreeZones.size() == 0 ||
        this->bufferFreeZones.size() == 1 
    ){
        return ;
    }
    for (int i=0; i < this->bufferFreeZones.size() - 1; i++){
        // is is sorted so we can use >=
        if (this->bufferFreeZones[i].second == this->bufferFreeZones[i + 1].first){
            // merge
            this->bufferFreeZones[i].second = std::max(this->bufferFreeZones[i + 1].second, this->bufferFreeZones[i].second);

            // delete 
            this->bufferFreeZones.erase(
                this->bufferFreeZones.begin() + i + 1
            );

            //handle index
            i--;
        }

    }   
}

int DynamicBuffer::getChunkBufferSpaceIndex(Chunk* chunk)
// -1 = invalid get request
// -2 = buffer requires realloc
{
    if (this->bufferFreeZones.size() == 0){
        return -2;
    }

    BufferInt dataSize = getChunkDataSize(chunk);

    if (dataSize == 0){
        return -1;
    }   
    BufferInt minSpaceSize = dataSize;
    


    bool zoneFound = false;
    int lowestDifferenceZoneIndex = 0;
    BufferInt minSpaceDifference = bufferSize; // max difference

    
    for (int i = 0; i < this->bufferFreeZones.size(); i++){
        std::pair<BufferInt, BufferInt> &freeZone = bufferFreeZones[i];
        if (freeZone.second < freeZone.first){
            ExitError("DYNAMIC_BUFFER","smth wrong with free zones second < first, GETCHUNKBUFFERINDEX()");
            return -2;
        }
        BufferInt zoneSpace = freeZone.second - freeZone.first;
        if (zoneSpace < minSpaceSize){
            continue;// too small zone
        }

        if (zoneSpace < minSpaceDifference + minSpaceSize){
            lowestDifferenceZoneIndex = i;
            if (minSpaceSize > zoneSpace){
                ExitError("DYNAMIC_BUFFER","minSpaceSize > zoneSpace, smth went wrong..., GETCHUNKBUFFERSPACEINDEX");
                return -2;
            }
            minSpaceDifference = zoneSpace - minSpaceSize;
            zoneFound = true;
        }
        
    }
    if (!zoneFound){
        return -2;
    }
    return lowestDifferenceZoneIndex;
}

GLenum DynamicBuffer::getBufferType()
{
    return bufferType;
}

bool DynamicBuffer::allocateDynamicBuffer(BufferInt size)
{
    std::cout << size << "\n";
    allocateBuffer(size + getBufferPadding(size));
    this->bufferCalls++;
    if (this->deleteData){
    
        markData(0,this->bufferSize);
        
    }
    
    

    this->bufferFreeZones.clear();
    this->bufferFreeZones.push_back(std::pair<BufferInt, BufferInt>(0, this->bufferSize));
    return true;
}

bool DynamicBuffer::insertChunkToBuffer(Chunk *chunk)
{   
    
    if (!chunk->hasBufferSpace[bufferType]){
        std::cout << "assigning space\n";
        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            expandBufferByChunk(chunk);

        }
        else if (assignRes == -1){
            std::cout << "-1\n"; 
            ExitError("DYNAMIC_BUFFER","Attempting to insert invalid chunk");
            return false;
        }

        return insertChunkToBuffer(chunk);
    }
   
    if (!updateChunkBuffer(chunk)){
        return false;
    };

    return true;

}

bool DynamicBuffer::deleteChunkFromBuffer(Chunk *chunk, bool merge)
{
    if (!chunk->hasBufferSpace[bufferType]){
        return true;
    }

    std::cout << "SIZE: " << this->bufferFreeZones.size() << "\n";

    if (chunk->bufferZone[bufferType].second < chunk->bufferZone[bufferType].first){
        ExitError("DYNAMIC_BUFFER","smth wrong with chunk free zones second < first, DELETECHUNK()");
        return false;
    }
    if (this->bufferFreeZones.size() == 0){
        this->bufferFreeZones.emplace_back(
            chunk->bufferZone[bufferType].first,
            chunk->bufferZone[bufferType].second
        );
    }
    else {
        bool inserted = false;
        for (int i =0; i<this->bufferFreeZones.size(); i++){
            std::pair<BufferInt, BufferInt> &freeZone = this->bufferFreeZones[i];
            if (freeZone.first == chunk->bufferZone[bufferType].first){
                ExitError("DYNAMIC_BUFFER","free zones starts at currently not deleted chunk");
                return false;
            }
            if (freeZone.second == chunk->bufferZone[bufferType].second){
                ExitError("DYNAMIC_BUFFER","free zones ends at currently not deleted chunk");
                return false;
            }


            if (
                ( freeZone.first >= chunk->bufferZone[bufferType].first && 
                  freeZone.first < chunk->bufferZone[bufferType].second) ||
                ( freeZone.second > chunk->bufferZone[bufferType].first && 
                  freeZone.second <= chunk->bufferZone[bufferType].second)
            ){
                ExitError("DYNAMIC_BUFFER","overlapping freeZone with currently occupied buffer space");
                return false;
            }

            if (freeZone.first < chunk->bufferZone[bufferType].first){
                continue;
            }

            // this free zone is further in memory than chunk space so 
            // WE INSERT NEW FREE ZONE RIGHT BEFORE IT

            this->bufferFreeZones.insert(
                this->bufferFreeZones.begin() + i,
                chunk->bufferZone[bufferType]
            );
            inserted = true;
            break;
        }
        if (!inserted){ // all free zones were before this chunk zone
            this->bufferFreeZones.emplace_back(
                chunk->bufferZone[bufferType].first,
                chunk->bufferZone[bufferType].second
            );
        }
    }

    if (this->deleteData){
        std::cout << "DATA: " << chunk->bufferZone[bufferType].first << " " << chunk->bufferZone[bufferType].second << " " << this->bufferSize << "\n";
        if (!markData(chunk->bufferZone[bufferType].first, chunk->bufferZone[bufferType].second)){
            ExitError("DYNAMIC_BUFFER","Marking data failed");
        };
        
    }
   
    chunk->bufferZone[bufferType].first = 0;
    chunk->bufferZone[bufferType].second = 0;
    chunk->hasBufferSpace[bufferType] = false;
    
    if (merge){
        mergeFreeZones();
    }
    
    

    return true;
}

int DynamicBuffer::assignChunkBufferZone(Chunk* chunk){
    int zoneIndex = getChunkBufferSpaceIndex(chunk);

    if (zoneIndex == -2){
        return -2; // buffer requires realloc
    }
    if (zoneIndex == -1){
        return -1; // no chunk data
    }

    BufferInt dataSize = getChunkDataSize(chunk);
    
    // now we have zone but what if it is some very huge zone then we would take it 
    // and loose a lot of usable space so we would do this
    // shrink our zone if it is too big and add newly created zone

    BufferInt maxSpaceSize = dataSize + getChunkPadding(dataSize);

    if (this->bufferFreeZones[zoneIndex].second < this->bufferFreeZones[zoneIndex].first){
        ExitError("DYNAMIC_BUFFER","smth wrong with chunk free zones second < first, ASSIGNCHUNKZONE()");
        return false;
    }

    BufferInt zoneSpace = this->bufferFreeZones[zoneIndex].second - this->bufferFreeZones[zoneIndex].first;
    if (zoneSpace > maxSpaceSize + (maxSpaceSize / 2)){
        // zone is too big so we resize it and add new free zone

        // new zone
        if (this->bufferFreeZones[zoneIndex].first + maxSpaceSize > this->bufferFreeZones[zoneIndex].second){
            ExitError("DYNAMIC_BUFFER","smth wrong with chunk free zones second < first, ASSIGNCHUNKZONE(), INSERTING FREE ZONE");
            return false;
        }
        this->bufferFreeZones.insert(
            this->bufferFreeZones.begin() + zoneIndex + 1,
            std::pair<BufferInt, BufferInt>(
                this->bufferFreeZones[zoneIndex].first + maxSpaceSize, 
                this->bufferFreeZones[zoneIndex].second
            )
        );
        // resize
        this->bufferFreeZones[zoneIndex].second = this->bufferFreeZones[zoneIndex].first + maxSpaceSize;
    }
    // with this approach we might have caused memory gap
    // what I mean is freeZone that is very small so no chunk will fit so It will never be used 
    // ! UNLESS THERE IS DYNAMIC CHUNK BUFFER SPACE LOGIC APPLIED !

    chunk->bufferZone[bufferType].first = this->bufferFreeZones[zoneIndex].first;
    chunk->bufferZone[bufferType].second = this->bufferFreeZones[zoneIndex].second;
    chunk->hasBufferSpace[bufferType] = true;

    this->bufferFreeZones.erase(this->bufferFreeZones.begin() + zoneIndex);
    mergeFreeZones();
    return 0;
}

void DynamicBuffer::expandBufferByChunk(Chunk* chunk){
    BufferInt dataSize = getChunkDataSize(chunk);
    BufferInt oldBufferSize = this->bufferSize;

    BufferInt minBufferSize = oldBufferSize + static_cast<BufferInt>(oldBufferSize * (BUFFER_EXPANSION_RATE / 100));

    // always expand by bigger one
    if (dataSize + getBufferPadding(dataSize) < minBufferSize){
        expandBuffer(minBufferSize);
    }
    else {
        expandBuffer(dataSize + getBufferPadding(dataSize));
    }
   

    if (oldBufferSize > this->bufferSize){
        ExitError("DYNAMIC_BUFFER","smth wrong with buffer expansion first > second, EXPANDBUFFERBYCHUNK");
        return ;
    }
    if (this->deleteData){
        if (!markData(oldBufferSize, this->bufferSize)){
            ExitError("DYNAMIC_BUFFER","marking failed");
        };
    }

    this->bufferFreeZones.push_back(std::pair<BufferInt, BufferInt>(oldBufferSize, this->bufferSize));
}

void DynamicBuffer::expandBuffer(BufferInt by){
    std::cout << "\n\nBUFFER EXPANSION..\n\n";
    BufferInt newBufferSize = this->bufferSize + by;
    newBufferSize -= newBufferSize % sizeof(CHUNK_MESH_DATATYPE); // for fitting

    // create temp buffer
    Buffer tempBuffer = Buffer(bufferType);
    tempBuffer.allocateBuffer(this->bufferSize);

    // setup for copying
    tempBuffer.bindAsWrite();
    this->bindAsRead();

    // copy data to temp buffer
    GLCall( glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, this->bufferSize) ); 
    this->bufferCalls++;

    //expand buffer
    this->allocateBuffer(newBufferSize); 

    // setup for copying
    tempBuffer.bindAsRead();
    this->bindAsWrite();

    GLCall( glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, tempBuffer.getBufferSize()) ); 
    this->bufferCalls++;

    tempBuffer.unBind();
    this->unBind();
}

bool DynamicBuffer::markData(BufferInt markStart, BufferInt markEnd)
{
     // now lets mark elements as unactive
    // to mark data as delete we need to change it to have face == 6
    // 393216 - masks face bits to be 6
    if (
        markEnd < markStart ||
        markStart > this->bufferSize ||
        markEnd > this->bufferSize
    ){
        std::cout  << "FAIL MARK\n";
        return false;
    }
    std::vector<CHUNK_MESH_DATATYPE> data((markEnd - markStart) / sizeof(CHUNK_MESH_DATATYPE),UNACTIVE_MESH_ELEMENT);
 
    return updateData<CHUNK_MESH_DATATYPE>(&data,markStart, markEnd); 
}

bool DynamicBuffer::moveBufferPart(BufferInt from, BufferInt to)
{

    ExitError("DYNAMIC_BUFFER","NO SUPPORT OVER MOVING");
    return false;



    if (to > from){
        ExitError("DYNAMIC_BUFFER","Currently no support for moving only aligning");
        return false;
    }

    if (from == to){
        return false; // no copy
    }
    BufferInt moveSize = this->bufferSize - from;
    if (
        moveSize + to > this->bufferSize ||
        moveSize + from > this->bufferSize
    ){
        return false; // moving or copying outside buffer
    }
    Buffer tempBuffer = Buffer(GL_ARRAY_BUFFER); // array just because
    tempBuffer.allocateBuffer(moveSize);


    tempBuffer.bindAsWrite();
    this->bindAsRead();

    GLCall( glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, from, 0, moveSize) ); 
    this->bufferCalls++;

    tempBuffer.bindAsRead();
    this->bindAsWrite();


    GLCall( glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, to, moveSize) ); 
    this->bufferCalls++;

    tempBuffer.unBind();
    this->unBind();

    // now we need to shift free zones
    for (std::pair<BufferInt, BufferInt> &pair : this->bufferFreeZones){
        // TODO POSSIBLE MEMORY CORRUPTION DUE TO NO ZONE CHECKING
        // VERY FRAGILE WATCH OUT
        if (pair.first >= from){
            // we know that `to` is smaller than `from` so we shift by this change:
            // from - to
            pair.first -= from - to;
            pair.second -= from - to;
        }
    }

    // and here surprise comes in shifting chunk zones



    return true;

}

int DynamicBuffer::getBufferCallsNum()
{   
    int calls = this->bufferCalls;
    this->bufferCalls = 0;
    return calls;
}