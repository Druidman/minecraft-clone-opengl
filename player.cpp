#include "player.h"
#include "world.h"

void Player::updateState()
{
    std::optional<Chunk*> res = this->world->getChunkByPos(position);
    if (!res.has_value()){
        this->state = IN_AIR;
        return ;
    }

    Chunk* chunk = res.value();

    std::optional< Block* > blockRes = chunk->getBlock(position + CAMERA_OFFSET);
    
    if (!blockRes.has_value()){ //NONE BLOCK so player is not standing on anything
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
        this->action = FALLING;
        return ;
    }

    Chunk* chunk = res.value();

    std::optional< Block* > blockRes = chunk->getBlock(position - glm::vec3(0.0,0.5,0.0), true); // underBlock
    
    if (!blockRes.has_value()){
        this->action = FALLING;
        return ;
    } 

    Block *block = blockRes.value();
    if (block->type != NONE_BLOCK && block->type != WATER){
        this->action = WALKING;
        move_to(glm::vec3(this->position.x, block->position.y + 0.5, this->position.z));
        return;
    }
    else{
        this->action = FALLING; // Who cares?
    }

    // NAH man we don't need that SKIP IT dont event read it 


    // // player is standing on water or none block so we check if he stands fully on it
    
    // glm::vec3 playerBottomLeft = position - glm::vec3(0.5,0.0,0.5);
    // glm::vec3 playerBottomRight = position - glm::vec3(-0.5,0.0,0.5);
    // glm::vec3 playerTopLeft = position + glm::vec3(-0.5,0.0,0.5);
    // glm::vec3 playerTopRight = position + glm::vec3(0.5,0.0,0.5);

    // // now that we have all corners lets check if any of them is within any of surrounding cubes
    

    
}

void Player::move_by(glm::vec3 dir)
{
    if (this->fly){
        position += dir;
        adjustCamera();
        return;
    }
    glm::vec3 changePos = position + dir;

    std::optional<Block *> blockRes = world->getBlockByPos(changePos, true);
    if (!blockRes.has_value()){
        return ;
    }
    if (
        blockRes.value()->type == NONE_BLOCK ||
        blockRes.value()->type == WATER
    ){
        position += dir;
        adjustCamera();
        return;
    }
    // now we have block where we want to move so we will move player on top of it
    // BUT only when there is no block above

    std::optional<Block *> blockRes2 = world->getBlockByPos(changePos + glm::vec3(0.0,1.0,0.0)); // + 1.0 on y axis
    if (blockRes2.has_value()){
        if (blockRes2.value()->type != WATER){
            return; // there is block on our height so we can't move
        }   
        
    }
    // there was no block so we move player up 
    // BUT only if the block we want to get in isn't noneblock, water ...
    position += dir;
    if (
        blockRes.value()->type != NONE_BLOCK &&
        blockRes.value()->type != WATER
    ){
        position.y = blockRes.value()->position.y;
    }
    
    
    adjustCamera();




    
    
    // check if move allowed and if so then do some stuff

    
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

void Player::process_input(double delta, bool &playerMoved)
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
        playerMoved = true;
    }
    if (glfwGetKey(window,GLFW_KEY_S)){
        move_by(-stepForward);
        playerMoved = true;
    }
    if (glfwGetKey(window,GLFW_KEY_D)){
        move_by(stepRight);
        playerMoved = true;
    }
    if (glfwGetKey(window,GLFW_KEY_A)){
        move_by(-stepRight);
        playerMoved = true;
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
}

void Player::update(double delta)
{
    
    // updateAction();
    // updateState();
    bool playerMoved = false;
    
    if (action == FALLING && !fly){
        move_by(glm::vec3(0.0,-5.0,0.0) * (float)delta);
        playerMoved = true;
    }

    process_input(delta, playerMoved);
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
