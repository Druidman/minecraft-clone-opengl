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
    for (int i=0; i < (int)this->bufferFreeZones.size() - 1; i++){
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
        std::cout << "no free zones...\n";
        return -2;
    }

    BufferInt dataSize = getChunkDataSize(chunk);

    if (dataSize == 0){
        return -1;
    }   
    BufferInt minSpaceSize = dataSize;
    


    bool zoneFound = false;
    int lowestDifferenceZoneIndex = 0;
    BufferInt minSpaceDifference = bufferTarget->bufferSize; // max difference

    
    for (int i = 0; i < (int)this->bufferFreeZones.size(); i++){
        std::pair<BufferInt, BufferInt> &freeZone = bufferFreeZones[i];
        if (freeZone.second < freeZone.first){
            ExitError("DYNAMIC_BUFFER" + getBufferTypeString(),"smth wrong with free zones second < first, GETCHUNKBUFFERINDEX()");
            return -2;
        }
        BufferInt zoneSpace = freeZone.second - freeZone.first;
        if (zoneSpace < minSpaceSize){
            continue;// too small zone
        }

        if (zoneSpace < minSpaceDifference + minSpaceSize){
            lowestDifferenceZoneIndex = i;
            if (minSpaceSize > zoneSpace){
                ExitError("DYNAMIC_BUFFER" + getBufferTypeString(),"minSpaceSize > zoneSpace, smth went wrong..., GETCHUNKBUFFERSPACEINDEX");
                return -2;
            }
            minSpaceDifference = zoneSpace - minSpaceSize;
            zoneFound = true;
        }
        
    }
    if (!zoneFound){
        std::cout << "no zone found...\n";
        return -2;
    }
    return lowestDifferenceZoneIndex;
}


bool DynamicBuffer::allocateDynamicBuffer(BufferInt size)
{
    std::cout << "dynamicAlloc: " << size << "\n";
    if (!this->bufferTarget->allocateBuffer(size + getBufferPadding(size))){
        return false;
    };
    this->bufferCalls++;
    if (this->deleteData){
    
        if (!markData(0,this->bufferTarget->bufferSize)){
            ExitError("DYNAMIC_BUFFER" + getBufferTypeString(),"Error marking data");
        };
        
    }
    
    

    this->bufferFreeZones.clear();
    this->bufferFreeZones.push_back(std::pair<BufferInt, BufferInt>(0, this->bufferTarget->bufferSize));
    return true;
}

bool DynamicBuffer::insertChunkToBuffer(Chunk *chunk)
{   
    
    if (!chunk->hasBufferSpace[chunkBufferType]){
        std::cout << "assigning space\n";
        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            if (allowsExpansion){
                if (!expandBufferByChunk(chunk)){
                    ExitError("DYNAMIC_BUFFER" + getBufferTypeString(),"buffer expansion went wrong");
                };
            }
            else {
                ExitError("DYNAMIC_BUFFER" + getBufferTypeString(),"Buffer expansion in not expandable buffer type");
            }
            
            

        }
        else if (assignRes == -1){
            std::cout << "-1\n"; 
            ExitError("DYNAMIC_BUFFER" + getBufferTypeString(),"Attempting to insert invalid chunk");
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
    if (!chunk->hasBufferSpace[chunkBufferType]){
        return true;
    }

    std::cout << "SIZE: " << this->bufferFreeZones.size() << "\n";

    if (chunk->bufferZone[chunkBufferType].second < chunk->bufferZone[chunkBufferType].first){
        ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"smth wrong with chunk free zones second < first, DELETECHUNK()");
        return false;
    }
    if (this->bufferFreeZones.size() == 0){
        this->bufferFreeZones.emplace_back(
            chunk->bufferZone[chunkBufferType].first,
            chunk->bufferZone[chunkBufferType].second
        );
    }
    else {
        bool inserted = false;
        for (int i =0; i<(int)this->bufferFreeZones.size(); i++){
            std::pair<BufferInt, BufferInt> &freeZone = this->bufferFreeZones[i];
            if (freeZone.first == chunk->bufferZone[chunkBufferType].first){
                ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"free zones starts at currently not deleted chunk");
                return false;
            }
            if (freeZone.second == chunk->bufferZone[chunkBufferType].second){
                ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"free zones ends at currently not deleted chunk");
                return false;
            }


            if (
                ( freeZone.first >= chunk->bufferZone[chunkBufferType].first && 
                  freeZone.first < chunk->bufferZone[chunkBufferType].second) ||
                ( freeZone.second > chunk->bufferZone[chunkBufferType].first && 
                  freeZone.second <= chunk->bufferZone[chunkBufferType].second)
            ){
                ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"overlapping freeZone with currently occupied buffer space");
                return false;
            }

            if (freeZone.first < chunk->bufferZone[chunkBufferType].first){
                continue;
            }

            // this free zone is further in memory than chunk space so 
            // WE INSERT NEW FREE ZONE RIGHT BEFORE IT

            this->bufferFreeZones.insert(
                this->bufferFreeZones.begin() + i,
                chunk->bufferZone[chunkBufferType]
            );
            inserted = true;
            break;
        }
        if (!inserted){ // all free zones were before this chunk zone
            this->bufferFreeZones.emplace_back(
                chunk->bufferZone[chunkBufferType].first,
                chunk->bufferZone[chunkBufferType].second
            );
        }
    }

    if (this->deleteData){
        std::cout << "DATA: " << chunk->bufferZone[chunkBufferType].first << " " << chunk->bufferZone[chunkBufferType].second << " " << this->bufferTarget->bufferSize << "\n";
        if (!markData(chunk->bufferZone[chunkBufferType].first, chunk->bufferZone[chunkBufferType].second)){
            ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"Marking data failed");
        };
        
    }
   
    chunk->bufferZone[chunkBufferType].first = 0;
    chunk->bufferZone[chunkBufferType].second = 0;
    chunk->hasBufferSpace[chunkBufferType] = false;
    
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
        ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"smth wrong with chunk free zones second < first, ASSIGNCHUNKZONE()");
     
    }

    BufferInt zoneSpace = this->bufferFreeZones[zoneIndex].second - this->bufferFreeZones[zoneIndex].first;
    if (zoneSpace > maxSpaceSize + (maxSpaceSize / 2)){
        // zone is too big so we resize it and add new free zone

        // new zone
        if (this->bufferFreeZones[zoneIndex].first + maxSpaceSize > this->bufferFreeZones[zoneIndex].second){
            ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"smth wrong with chunk free zones second < first, ASSIGNCHUNKZONE(), INSERTING FREE ZONE");
      
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

    chunk->bufferZone[chunkBufferType].first = this->bufferFreeZones[zoneIndex].first;
    chunk->bufferZone[chunkBufferType].second = this->bufferFreeZones[zoneIndex].second;
    chunk->hasBufferSpace[chunkBufferType] = true;

    

    this->bufferFreeZones.erase(this->bufferFreeZones.begin() + zoneIndex);
    mergeFreeZones();
    return 0;
}

bool DynamicBuffer::expandBufferByChunk(Chunk* chunk){
    
    
    BufferInt dataSize = getChunkDataSize(chunk);
    BufferInt oldBufferSize = this->bufferTarget->bufferSize;

    BufferInt minBufferSize = oldBufferSize + static_cast<BufferInt>(oldBufferSize * (BUFFER_EXPANSION_RATE / 100.0));

    // always expand by bigger one
    if (dataSize + getBufferPadding(dataSize) + oldBufferSize < minBufferSize){
        if (!this->bufferTarget->expandBuffer(minBufferSize - oldBufferSize)){
            return false;
        };
    }
    else {
        if (!this->bufferTarget->expandBuffer(dataSize + getBufferPadding(dataSize))){
            return false;
        };
        
    }
   

    if (oldBufferSize > this->bufferTarget->bufferSize){
        ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"smth wrong with buffer expansion first > second, EXPANDBUFFERBYCHUNK");
        return false;
    }
    if (this->deleteData){
        std::cout << "marking after expansion...\n";
        if (!markData(oldBufferSize, this->bufferTarget->bufferSize)){
            ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"marking failed");
        };
    }

    this->bufferFreeZones.push_back(std::pair<BufferInt, BufferInt>(oldBufferSize, this->bufferTarget->bufferSize));
    return true;
}

bool DynamicBuffer::moveBufferPart(BufferInt from, BufferInt to)
{
    
    ExitError("DYNAMIC_BUFFER"  + getBufferTypeString(),"NO SUPPORT OVER MOVING");
    return false;

}
