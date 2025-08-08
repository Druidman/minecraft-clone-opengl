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

std::optional<int> MeshBuffer::getChunkBufferSpaceIndex(
    BufferInt meshSize)
{
    if (this->bufferFreeZones.size() == 0 || meshSize == 0){
        return std::nullopt;
    }   
    BufferInt minSpaceSize = meshSize;
    


    bool zoneFound = false;
    int lowestDifferenceZoneIndex = 0;
    BufferInt minSpaceDifference = bufferSize; // rollover so max difference

    
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
        return std::nullopt;
    }
    return lowestDifferenceZoneIndex;
}

bool MeshBuffer::allocateMeshBuffer(BufferInt meshSize)
{
    allocateBuffer(meshSize + static_cast<BufferInt>(meshSize * (BUFFER_PADDING / 100.0))); // allocate with padding
    return true;
}

bool MeshBuffer::insertChunkToBuffer(Chunk *chunk)
{   
    BufferInt meshSize = chunk->getMeshSize();
    if (!chunk->hasBufferSpace){
        std::optional<int> bufferSpaceRes = getChunkBufferSpaceIndex
        (
            meshSize
        );
        if (!bufferSpaceRes.has_value()){
            return false; // buffer requires realloc
        }
        // now we have zone but what if it is some very huge zone then we would take it 
        // and loose a lot of usable space so we would do this
        // shrink our zone if it is too big and add newly created zone
        int zoneIndex = bufferSpaceRes.value();

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
 
            // with this approach we might have caused memory gap
            // what I mean is freeZone that is very small so no chunk will fit so It will never be used 
            // ! UNLESS THERE IS DYNAMIC CHUNK BUFFER SPACE LOGIC APPLIED !
        }

        chunk->buffer_zone_start = this->bufferFreeZones[zoneIndex].first;
        chunk->buffer_zone_end = this->bufferFreeZones[zoneIndex].second;
        chunk->hasBufferSpace = true;

        // now that zone is being occupied we remove it
        this->bufferFreeZones.erase(this->bufferFreeZones.begin() + zoneIndex);
    }
    updateChunkBuffer(chunk);
    return true;
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

bool MeshBuffer::updateChunkBuffer(Chunk *chunk)
{
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
