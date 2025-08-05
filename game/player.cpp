#include "player.h"
#include "world.h"
#include "renderer.h"

void Player::updateState()
{
    std::optional<Chunk*> res = this->world->getChunkByPos(position);
    if (!res.has_value()){
        this->state = IN_AIR; // player is outside of the map
        return ;
    }

    Chunk* chunk = res.value();

    std::optional< Block* > blockRes = chunk->getBlock(position + CAMERA_OFFSET);
    
    if (!blockRes.has_value()){ // player is falling
        this->state = IN_AIR;
        return ;
    } 
    Block* block = blockRes.value();

    switch(block->type){
        case WATER:
            this->state = IN_WATER;
            
            break;
        default:
            this->state = IN_AIR;
            
            break;
    }
}

void Player::updateAction()
{   
    std::optional<Chunk*> res = this->world->getChunkByPos(position);
    if (!res.has_value()){
        this->action = FALLING; //player is outside of the map
        std::cout << "falling1\n";
        return ;
    }

    Chunk* chunk = res.value();

    std::optional< Block* > blockRes = chunk->getBlock(position - glm::vec3(0.0,0.5,0.0)); // underBlock
    
    if (!blockRes.has_value()){
        this->action = FALLING; // no block under player
        std::cout << "falling2\n";
        return ;
    } 

    Block *block = blockRes.value();
    if (block->type != WATER){
        this->action = WALKING; //player is on solid block
        move_to(glm::vec3(this->position.x, block->position.y + 0.5, this->position.z));
        
        return;
    }
    else{
        this->action = FALLING; // player is in water
        std::cout << "falling3\n";
    }
}

void Player::move_by(glm::vec3 dir)
{
    if (this->fly){
        position += dir;
        adjustCamera();
        return;
    }
    glm::vec3 changePos = position + dir;

    std::optional<Block *> blockRes = world->getBlockByPos(changePos);

    if (!blockRes.has_value()){
        position += dir;
        adjustCamera();
        return;
    }
    if (blockRes.value()->type == WATER){
        position += dir;
        adjustCamera();
        return;
    }
    
    position += dir;
    position.y = blockRes.value()->position.y + 0.5;
    adjustCamera();


    
}
void Player::move_to(glm::vec3 pos)
{
    position = pos;
    adjustCamera();
    
}

void Player::adjustCamera()
{
    camera->position = position + CAMERA_OFFSET;
}

void Player::process_input(double delta)
{
    float speed = PLAYER_SPEED * delta;
    glm::vec3 camRight = this->camera->getRightVec();
    glm::vec3 walkDir;
    glm::vec3 walkSideDir;

    if (this->fly){
        speed *= 5;
        walkDir = camera->direction;
        walkSideDir = camRight;
    }
    else {
        walkDir = glm::vec3(this->camera->direction.x,0.0,this->camera->direction.z);
        walkSideDir = glm::vec3(camRight.x,0.0,camRight.z);
    }
    

    glm::vec3 stepForward = speed * walkDir;
    glm::vec3 stepRight = speed * walkSideDir;

    if (glfwGetKey(window,GLFW_KEY_W)){
        move_by(stepForward);

    }
    if (glfwGetKey(window,GLFW_KEY_S)){
        move_by(-stepForward);
     
    }
    if (glfwGetKey(window,GLFW_KEY_D)){
        move_by(stepRight);

    }
    if (glfwGetKey(window,GLFW_KEY_A)){
        move_by(-stepRight);
    
    }

    int leftState = glfwGetMouseButton(window,leftMouseButton);
    if (leftState == GLFW_PRESS && this->lastLeftButtonState == GLFW_RELEASE){
        buttonPressed(leftMouseButton);
    }
    this->lastLeftButtonState = leftState;

    int rightState = glfwGetMouseButton(window,rightMouseButton);
    if (rightState == GLFW_PRESS && this->lastRightButtonState == GLFW_RELEASE){
        buttonPressed(rightMouseButton);
    }
    this->lastRightButtonState = rightState;

    int flyState = glfwGetKey(window,GLFW_KEY_F);
    if (flyState == GLFW_PRESS && this->lastFlyButtonState == GLFW_RELEASE){
        fly = !fly;
    }
    this->lastFlyButtonState = flyState;
}

void Player::destroy_block()
{
    std::optional<BlockIntersection> result = shootRay(this->camera->position,PLAYER_RANGE,{WATER},this->camera->direction,world);
    if (!result.has_value()){
        return ;
    }
    BlockIntersection intersection = result.value();

    Block *currentBlock = intersection.block;
    Chunk *chunk = intersection.chunk;
    glm::vec3 hitPos = intersection.hitPos;

    chunk->removeBlock(hitPos);
    chunk->genChunkMesh();
    world->renderer->fillBuffers();
    
}

void Player::place_block()
{
    std::optional<BlockIntersection> result = shootRay(this->camera->position,PLAYER_RANGE,{WATER},this->camera->direction,world);
    if (!result.has_value()){
        return ;
    }
    BlockIntersection intersection = result.value();

    Block *currentBlock = intersection.block;
    glm::vec3 hitPos = intersection.hitPos;

    std::vector <float > distanceToFaces = {
        glm::distance(hitPos, currentBlock->position + FRONT_FACE_POS),
        glm::distance(hitPos, currentBlock->position + BACK_FACE_POS),
        glm::distance(hitPos, currentBlock->position + TOP_FACE_POS),
        glm::distance(hitPos, currentBlock->position + BOTTOM_FACE_POS),
        glm::distance(hitPos, currentBlock->position + LEFT_FACE_POS),
        glm::distance(hitPos, currentBlock->position + RIGHT_FACE_POS)
    };
    float minDistance = 100;
    int minDistanceInd = 0;

    for (int i=0; i<distanceToFaces.size();i++){
        if (distanceToFaces[i] < minDistance){
            minDistance = distanceToFaces[i];
            minDistanceInd = i;
        }
    }
    glm::vec3 placePos = currentBlock->position;
    switch (minDistanceInd)
    {
        case 0:
            placePos.z += 1;
            break;
        case 1:
            placePos.z -= 1;
            break;
        case 2:
            placePos.y += 1;
            break;
        case 3:
            placePos.y -= 1;
            break;
        case 4:
            placePos.x -= 1;
            break;
        case 5:
            placePos.x += 1;
            break;
        
        default:
            break;
    }
    

    std::optional<Chunk*> res = world->getChunkByPos(placePos);
    if (!res.has_value()){
        return ;
    }

    Chunk *placeChunk = res.value();
    Block blockToAdd = Block(STONE,placePos);
    placeChunk->addBlock(blockToAdd);
    placeChunk->genChunkMesh();


    world->renderer->fillBuffers();
}

void Player::update(double delta)
{
    
    updateAction();
    updateState();

    
    if (action == FALLING && !fly){
        std::cout << "fallll\n";
        move_by(glm::vec3(0.0,-5.0,0.0) * (float)delta);
        
    }

    process_input(delta);
    world->renderer->fillChunkStorageBuffer();
}

void Player::buttonPressed(int button)
{
    if (button == leftMouseButton){
        destroy_block();
    }
    else if (button == rightMouseButton){
        place_block();
    }
}
