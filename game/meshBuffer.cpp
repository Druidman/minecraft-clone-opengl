#include "meshBuffer.h"

void MeshBuffer::mergeFreeZones()
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

int MeshBuffer::getChunkBufferSpaceIndex(Chunk* chunk)
// -1 = invalid get request
// -2 = buffer requires realloc
{
    if (this->bufferFreeZones.size() == 0){
        return -2;
    }
    BufferInt meshSize = chunk->getMeshSize();
    if (meshSize == 0){
        return -1;
    }   
    BufferInt minSpaceSize = meshSize;
    


    bool zoneFound = false;
    int lowestDifferenceZoneIndex = 0;
    BufferInt minSpaceDifference = bufferSize; // max difference

    
    for (int i = 0; i <this->bufferFreeZones.size();i++){
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

bool MeshBuffer::allocateMeshBuffer(BufferInt meshSize)
{
    allocateBuffer(meshSize + static_cast<BufferInt>(meshSize * (BUFFER_PADDING / 100.0))); // allocate with padding

    this->bufferFreeZones.clear();
    this->bufferFreeZones.push_back(std::pair<BufferInt, BufferInt>(0, this->bufferSize));
    return true;
}

bool MeshBuffer::insertChunkToBuffer(Chunk *chunk)
{   
    BufferInt meshSize = chunk->getMeshSize();
    if (!chunk->hasBufferSpace){
        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            expandBufferByChunk(chunk);
        }
        else if (assignRes == -1){
            return false;
        }

        insertChunkToBuffer(chunk);
        
    }
    return updateChunkBuffer(chunk);
}

bool MeshBuffer::deleteChunkFromBuffer(Chunk *chunk)
{
    bool insertedZone = false;
    for (int i = 0; i<this->bufferFreeZones.size(); i++){
        if (this->bufferFreeZones[i].first == chunk->buffer_zone_end){

            this->bufferFreeZones.insert(
                this->bufferFreeZones.begin() + i, // cause i can't be 0
                std::pair<BufferInt, BufferInt>(
                    chunk->buffer_zone_start,
                    chunk->buffer_zone_end
                )
            );
            insertedZone = true;
            break;
        }
        else if (this->bufferFreeZones[i].second == chunk->buffer_zone_start){
            this->bufferFreeZones.insert(
                this->bufferFreeZones.begin() + i + 1, 
                std::pair<BufferInt, BufferInt>(
                    chunk->buffer_zone_start,
                    chunk->buffer_zone_end
                )
            );
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

void MeshBuffer::addBufferFreeZone(BufferInt start, BufferInt end){

}

int MeshBuffer::assignChunkBufferZone(Chunk* chunk){
    int zoneIndex = getChunkBufferSpaceIndex(chunk);

    if (zoneIndex == -2){
        return -2; // buffer requires realloc
    }
    if (zoneIndex == -1){
        return -1; // no chunk mesh
    }

    BufferInt meshSize = chunk->getMeshSize();
    // now we have zone but what if it is some very huge zone then we would take it 
    // and loose a lot of usable space so we would do this
    // shrink our zone if it is too big and add newly created zone

    BufferInt maxSpaceSize = meshSize + static_cast<BufferInt>(meshSize * (CHUNK_PADDING / 100.0));

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

    chunk->buffer_zone_start = this->bufferFreeZones[zoneIndex].first;
    chunk->buffer_zone_end = this->bufferFreeZones[zoneIndex].second;
    chunk->hasBufferSpace = true;

    this->bufferFreeZones.erase(this->bufferFreeZones.begin() + zoneIndex);
    return 0;
}

void MeshBuffer::expandBufferByChunk(Chunk* chunk){
    BufferInt meshSize = chunk->getMeshSize();
    BufferInt oldBufferSize = this->bufferSize;
    expandBuffer(meshSize + static_cast<BufferInt>(meshSize * (BUFFER_PADDING / 100.0)));

    this->bufferFreeZones.push_back(std::pair<BufferInt, BufferInt>(oldBufferSize, this->bufferSize));
}

bool MeshBuffer::updateChunkBuffer(Chunk *chunk)
{
    BufferInt meshSize = chunk->getMeshSize();
    if (meshSize > chunk->buffer_zone_end - chunk->buffer_zone_start){
        // chunk is to big so we either find new free zone OR reallocate buffer
        

        // ! in all of these scenarios we need to remove chunk from its current location !
        deleteChunkFromBuffer(chunk);

        // lets try to find new freeZone

        int assignRes = assignChunkBufferZone(chunk);
        if (assignRes == -2){
            // we need to expand buffer because we have no usable space left
            expandBufferByChunk(chunk);
        }
        else if (assignRes == -1){
            return false;
        }

        insertChunkToBuffer(chunk);
    }
    BufferInt transMeshOffset = chunk->getOpaqueMesh()->size() * sizeof(CHUNK_MESH_DATATYPE);
    if (chunk->getOpaqueMesh()->size() != 0){
        if (!updateData<CHUNK_MESH_DATATYPE>(
            chunk->getOpaqueMesh(), 
            chunk->buffer_zone_start, 
            chunk->buffer_zone_end
        )){
            return false;
        }
    }
    if (chunk->getTransparentMesh()->size() != 0){
        if (!updateData<CHUNK_MESH_DATATYPE>(
            chunk->getTransparentMesh(), 
            chunk->buffer_zone_start + transMeshOffset, 
            chunk->buffer_zone_end
        )){
            return false;
        }
    }
    
    
    return true;
}
