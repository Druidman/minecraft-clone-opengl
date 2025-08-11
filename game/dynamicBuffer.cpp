#include "dynamicBuffer.h"

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
        if (this->bufferFreeZones[i].second == this->bufferFreeZones[i + 1].first){
            // merge
            this->bufferFreeZones[i].second = this->bufferFreeZones[i + 1].second;

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
        BufferInt zoneSpace = freeZone.second - freeZone.first;
        if (zoneSpace - minSpaceSize < 0){
            continue;// too small zone
        }

        if (zoneSpace - minSpaceSize < minSpaceDifference){
            lowestDifferenceZoneIndex = i;
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
    std::cout << size << "\n"; 
    allocateBuffer(size + getBufferPadding(size));


    this->bufferFreeZones.clear();
    this->bufferFreeZones.push_back(std::pair<BufferInt, BufferInt>(0, this->bufferSize));
    return true;
}

bool DynamicBuffer::insertChunkToBuffer(Chunk *chunk)
{   
    if (!chunk->hasBufferSpace[bufferType]){
        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            expandBufferByChunk(chunk);
        }
        else if (assignRes == -1){
            std::cout << "-1\n"; 
            return false;
        }

        
        
    }
    return updateChunkBuffer(chunk);
}

bool DynamicBuffer::deleteChunkFromBuffer(Chunk *chunk)
{
    bool insertedZone = false;
    for (int i = 0; i<this->bufferFreeZones.size(); i++){
        if (this->bufferFreeZones[i].first == chunk->bufferZone[bufferType].second){

            this->bufferFreeZones.insert(
                this->bufferFreeZones.begin() + i, // cause i can't be 0
                std::pair<BufferInt, BufferInt>(
                    chunk->bufferZone[bufferType].first,
                    chunk->bufferZone[bufferType].second
                )
            );
            insertedZone = true;
            break;
        }
        else if (this->bufferFreeZones[i].second == chunk->bufferZone[bufferType].first){
            this->bufferFreeZones.insert(
                this->bufferFreeZones.begin() + i + 1, 
                std::pair<BufferInt, BufferInt>(
                    chunk->bufferZone[bufferType].first,
                    chunk->bufferZone[bufferType].second
                )
            );
            insertedZone = true;
            break;
        }
        else if (this->bufferFreeZones[i].first == chunk->bufferZone[bufferType].first && 
                 this->bufferFreeZones[i].second == chunk->bufferZone[bufferType].second) {

            insertedZone = true;
            break;
        }
    }
    if (!insertedZone){
        return false;
    }
    mergeFreeZones();
    
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

    BufferInt zoneSpace = this->bufferFreeZones[zoneIndex].second - this->bufferFreeZones[zoneIndex].first;
    if (zoneSpace - maxSpaceSize > 0){
        // zone is too big so we resize it and add new free zone

        // new zone
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
    return 0;
}

void DynamicBuffer::expandBufferByChunk(Chunk* chunk){
    ExitError("DYNBAMIC BUFFER","can;t expand buffer");
    return ;
    BufferInt dataSize = getChunkDataSize(chunk);
    BufferInt oldBufferSize = this->bufferSize;

    expandBuffer(dataSize + getBufferPadding(dataSize));

    this->bufferFreeZones.push_back(std::pair<BufferInt, BufferInt>(oldBufferSize, this->bufferSize));
}


