#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <optional>

#define FNL_IMPL
#include "vendor/fastNoise/FastNoiseLite.h"

#include "betterGL.h"


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/stb_image/stb_image.h"

#include "shader.h"
#include "texture.h"
#include "vertexArray.h"
#include "vertexBuffer.h"
#include "elementBuffer.h"
#include "block.h"
#include "chunk.h"


enum PlayerState{
    IN_AIR, IN_WATER, ON_GROUND
};


typedef unsigned int uint;

const int PLAYER_RANGE = 100;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
glm::vec3 cameraPos = glm::vec3(0.0,0.0,0.0);
glm::vec3 cameraFront = glm::vec3(0.0,0.0,3.0);
glm::vec3 cameraUp = glm::vec3(0.0,1.0,0.0);

glm::vec2 cursorPos = glm::vec2(0.0,0.0);

glm::vec2 mouseoffset = glm::vec2(0.0,0.0);

double yaw = 90, pitch = 0;

bool firstMouse = true;
glm::mat4 model(1.0f);
glm::mat4 view = glm::lookAt(cameraPos,cameraPos + cameraFront,cameraUp);
glm::mat4 projection = glm::perspective(glm::radians(45.0),(double)WINDOW_WIDTH/WINDOW_HEIGHT,0.1,1000.0);

PlayerState playerState = IN_AIR;
std::vector< std::vector <Chunk> > chunks;
bool removeChunk = false;
bool stopMovingCamera = false;
template <typename T>
struct Result
{
    T *result; 
    bool success;
};

struct BlockIntersection{
    Block *block;
    Chunk *chunk;
    glm::vec3 hitPos;

};

void resize_callback(GLFWwindow *window, int width, int height){
    glViewport(0,0,width,height);
    projection = glm::perspective(glm::radians(45.0),(double)width/height,0.1,1000.0);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}
std::optional<Chunk*> getChunkByPos(glm::vec3 pointPositionInWorld, auto &chunks){
    int chunkRow = (int)(std::floor(pointPositionInWorld.z / (float)CHUNK_WIDTH));
    int chunkCol = (int)(std::floor(pointPositionInWorld.x / (float)CHUNK_WIDTH));
    if (chunkRow >= chunks.size() ||
        chunkCol >= chunks[chunkRow].size()
    ){
        return std::nullopt;
    }

    

    if (pointPositionInWorld.y < chunks[chunkRow][chunkCol].position.y){
        return std::nullopt;
    }

    return &chunks[chunkRow][chunkCol];
}

void placeBlock(auto &chunks){
    std::optional<BlockIntersection> result = lookAtBlock(chunks);
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
    

    std::optional<Chunk*> res = getChunkByPos(placePos,chunks);
    if (!res.has_value()){
        return ;
    }

    Chunk *placeChunk = res.value();
    placeChunk->addBlock(Block(STONE,placePos));
    placeChunk->genChunkMesh();
    
}

void destroyBlock(auto &chunks){
    std::optional<BlockIntersection> result = lookAtBlock(chunks);
    if (!result.has_value()){
        return ;
    }
    BlockIntersection intersection = result.value();

    Block *currentBlock = intersection.block;
    Chunk *chunk = intersection.chunk;
    glm::vec3 hitPos = intersection.hitPos;

    
    int platform = floor(hitPos.y) - chunk->position.y;
    int col = floor(hitPos.x) - (chunk->position.x - (CHUNK_WIDTH / 2));
    int row = floor(hitPos.z) - (chunk->position.z - (CHUNK_WIDTH / 2));
    chunk->removeBlock(platform,row,col);
    chunk->genChunkMesh();
    
    
}

std::optional<BlockIntersection> lookAtBlock(std::vector< std::vector< Chunk > > &chunks){
    glm::vec3 playerPos = cameraPos;
    glm::vec3 playerDirection = glm::normalize(cameraFront);

    glm::vec3 target = playerPos;
    bool targetFound = false;
    for (int i=0; i<PLAYER_RANGE * 50; i++){
    
        target += playerDirection / 50.0f;

        std::optional<Chunk*> result = getChunkByPos(target,chunks);
        if (!result.has_value()){
            continue ;
        }

        Chunk *currentChunk = result.value();

        std::optional<Block*> blockRes = currentChunk->getBlock(target);
        if (!blockRes.has_value()){
            continue ;
        }
        Block *currentBlock = blockRes.value();

        if (currentBlock->type == WATER){
            continue ;
        }

        BlockIntersection intersection;
        intersection.block = currentBlock;
        intersection.chunk = currentChunk;
        intersection.hitPos = target;
        
        return intersection;
        
    }
    
    return std::nullopt;
}

void process_key_release(GLFWwindow *window, int key){
    if (key ==GLFW_KEY_ESCAPE){
        glfwSetWindowShouldClose(window,true);
    }
     
    if (key == GLFW_KEY_ENTER){
        stopMovingCamera = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (key == GLFW_KEY_SPACE){
        stopMovingCamera = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void process_key_press(GLFWwindow *window, int key){
    
}

void process_input(GLFWwindow *window, auto &chunks, double delta){
    float speed = 100 * delta;
    if (glfwGetKey(window,GLFW_KEY_W)){
        cameraPos += speed * cameraFront;
        setPlayerState(cameraPos,chunks);
    
    }
    if (glfwGetKey(window,GLFW_KEY_S)){
        cameraPos -= speed * cameraFront;
        setPlayerState(cameraPos,chunks);
    }
    if (glfwGetKey(window,GLFW_KEY_D)){
        cameraPos -= speed * glm::normalize(glm::cross(cameraUp,cameraFront));
        setPlayerState(cameraPos,chunks);
    
    }
    if (glfwGetKey(window,GLFW_KEY_A)){
        cameraPos += speed * glm::normalize(glm::cross(cameraUp,cameraFront));
        setPlayerState(cameraPos,chunks);
        
    }

    
}

void input_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
    if (action == GLFW_RELEASE){
        process_key_release(window,key);
    }
    else if (action == GLFW_PRESS){
        process_key_press(window,key);
    }
    

}
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos){
    
    if (stopMovingCamera){
        return ;
    }

    if (firstMouse)
    {
        
        cursorPos.x = xpos;
        cursorPos.y = ypos;
        firstMouse = false;
    }
    
    
    
    double changeX = cursorPos.x - xpos;
    double changeY = cursorPos.y - ypos;

    

    yaw -= changeX * 0.1;
    pitch += changeY * 0.1;

    if (pitch <= -89.0){
        pitch = -89.0;
    }
    else if (pitch >= 89.0){
        pitch = 89.0;
    }


    glm::vec3 direction;

        
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));


    cameraFront = glm::normalize(direction);

    cursorPos.x = xpos;
    cursorPos.y = ypos;

}

std::vector<float> tree_gen(int sizex , int sizey){
    std::vector<float> trees(sizex * sizey, 0.0f);

        // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.fractal_type = FNL_FRACTAL_NONE;
    noise.frequency = 0.011;
    noise.octaves = 3;
    noise.seed = 1337;

    noise.noise_type = FNL_NOISE_VALUE;

    // Gather noise data
    
    unsigned long int index = 0;
    for (int y = 0; y < sizey; y++)
    {
        for (int x = 0; x < sizex; x++) 
        {
            float gen = fnlGetNoise2D(&noise, y, x);
            gen = (gen + 1) / 2;

            trees[index++] = gen;
            
        }
    }
    
    return trees;
}

std::vector<int> world_gen(int sizex , int sizey){
    std::vector<int> world(sizex * sizey, 0.0f);

        // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.frequency = 0.005;
    noise.octaves = 4;
    noise.seed = 1652;

    noise.noise_type = FNL_NOISE_PERLIN;

    // Gather noise data
    
    unsigned long int index = 0;
    int min = 1000;
    for (int y = 0; y < sizey; y++)
    {
        for (int x = 0; x < sizex; x++) 
        {
            float gen = fnlGetNoise2D(&noise, y, x);
            gen = (gen + 1) / 2 * 100;

            world[index++] = (int)gen;
            if ((int)gen < min){
                min = (int)gen;
            }
        }
    }
    // lets make world's wfirst block be settled on 0.0
    for (int i=0; i<world.size();i++){
        world[i] -= min;
    } 


    
        

    return world;

}

void setPlayerState(glm::vec3 playerPos, auto &chunks){
    std::optional<Chunk*> res = getChunkByPos(playerPos,chunks);
    if (!res.has_value()){
        playerState = IN_AIR;
        return ;
    }
    
    Chunk* chunk = res.value();
    std::optional< Block* > blockRes = chunk->getBlock(playerPos);
    if (!blockRes.has_value()){ //NONE BLOCK so player is in AIR
        playerState = IN_AIR;
        return ;
    } //return because ground checking logic isn't done ! TODO !
    Block* block = blockRes.value();
    // std::cout << "block found\n";
    switch (block->type)
    {
    case WATER:
        playerState = IN_WATER;
        break;
    default:
        playerState = ON_GROUND;
        break;
    }

}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        placeBlock(chunks);
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        destroyBlock(chunks);
    }
}

int main()
{
    
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    /* Create a windowed mode window and its OpenGL context */
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);




    glfwSetWindowSizeCallback(window,resize_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window,input_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);




    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval( 0 );
    
    glewInit();

    stbi_set_flip_vertically_on_load(true);
    
    GLCall( glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT) ) ;
    GLCall( glEnable(GL_DEPTH_TEST) );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    std::filesystem::path cwd = std::filesystem::current_path();

    std::filesystem::path vsPath = cwd / "shaders/vertexShader.vs";
    std::filesystem::path fsPath = cwd / "shaders/fragmentShader.fs";


    std::filesystem::path cvsPath = cwd / "shaders/vertexShaderCrosshair.vs";
    std::filesystem::path cfsPath = cwd / "shaders/fragmentShaderCrosshair.fs";
    
    Shader shader = Shader(vsPath.string(), fsPath.string());
    Shader crosshairShader = Shader(cvsPath.string(),cfsPath.string());

    std::filesystem::path texturePath = cwd / "textures/texture.png";

    Texture texture = Texture(texturePath, "png");

    std::vector<float> crosshairVertices = {
        -0.01f, -0.01f, 0.0f,
        0.01f, -0.01f, 0.0f,
        0.0f,  0.0f, 0.0f
    };

    VertexArray crosshairVAO = VertexArray();
    VertexBuffer crosshairVBO = VertexBuffer();
    crosshairVBO.fillData<float>(crosshairVertices);
    crosshairVAO.setAttr(0,3,GL_FLOAT, 3 * sizeof(float), 0);

    VertexArray vao = VertexArray();
    VertexBuffer vbo = VertexBuffer();
    ElementBuffer ebo = ElementBuffer();
    VertexBuffer vboInst = VertexBuffer();
    
 
    vbo.fillData<float>(BLOCK_FACE_VERTEX_POS);
    ebo.fillData<int>(BLOCK_FACE_INDICES);

    vbo.bind();
    vao.setAttr(0,3,GL_FLOAT,4 * sizeof(float),0);
    vao.setAttr(1,1,GL_FLOAT,4 * sizeof(float),3 * sizeof(float));
    vboInst.bind();
    vao.setAttrI(2,1,sizeof(VertexDataInt),0);
    GLCall( glVertexAttribDivisor(2,1) );
    

    GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    GLCall( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    GLCall( glBindVertexArray(0) );




   


    const int WORLD_WIDTH = 768;
    const unsigned long long WORLD_BLOCKS_COUNT = WORLD_WIDTH * WORLD_WIDTH;
    const unsigned long long WORLD_CHUNKS_COUNT = WORLD_BLOCKS_COUNT / ((unsigned long long)CHUNK_WIDTH * CHUNK_WIDTH);
    std::vector<int> world = world_gen(WORLD_WIDTH,WORLD_WIDTH);
    std::vector<float> trees = tree_gen(WORLD_WIDTH,WORLD_WIDTH);

    
    std::vector<Chunk*> chunkRefs;


    int startX = 0;
    int startY = 0;

    for (int row = 0; row < WORLD_WIDTH / CHUNK_WIDTH; row++){
        for (int column = 0; column < WORLD_WIDTH / CHUNK_WIDTH; column++){
            if (startX >= WORLD_WIDTH){
                startX = 0;
                startY += CHUNK_WIDTH;
            }

            glm::vec3 chunkPos = glm::vec3(startX + (CHUNK_WIDTH / 2), 0.0, startY + (CHUNK_WIDTH / 2));
            Chunk chunk = Chunk(chunkPos,vboInst,&cameraPos);
            
            
            for (int i =startX; i< startX + CHUNK_WIDTH ; i++){
                for (int j =startY; j< startY + CHUNK_WIDTH; j++){

                    float zCoord = world[j * WORLD_WIDTH + i] + 0.5;
                    BlockType blockType = GRASS_DIRT;
                    if (zCoord < 20){
                     
                        blockType = SAND;
                    }
                    
                    else if (zCoord > 60){
                        blockType = STONE;
                    }
                    
                    Block block(blockType,glm::vec3(i + 0.5, zCoord ,j + 0.5));
                    
                    chunk.addBlock(block);
                    
                    chunk.fillUnderBlock(block);
                    
                }
            }
            
            chunk.fillWater();

            for (int i =startX + 3; i< startX + CHUNK_WIDTH - 3; i+= 6){
                for (int j =startY + 3; j< startY + CHUNK_WIDTH - 3; j+=6){

                    float treeChance = trees[j * WORLD_WIDTH + i];
                    if (treeChance < 0.5){
                        continue ;
                    }
                    float zCoord = world[j * WORLD_WIDTH + i] + 0.5;
                    
                    glm::vec3 treePos = glm::vec3(i + 0.5 + (rand() % 6), zCoord ,j + 0.5 + (rand() % 6));
                    auto blockRes = chunk.getBlock(treePos);
                    if (!blockRes.has_value()){ // cause that means smth went really wrong 
                        continue ;
                    }
                    
                    Block *block = blockRes.value();
                    if (block->type != GRASS_DIRT){
                        continue ;
                    }
                    chunk.addTree(block->position + glm::vec3(0.0,1.0,0.0));

                    
                }
            }
            
            if (column == 0){
                chunks.push_back(std::vector<Chunk>(1,chunk));
                
            }
            else{
                chunks[row].push_back(chunk);
            }
            
            startX += CHUNK_WIDTH;
            
        }
    }
    for (int row =0; row < chunks.size(); row++){
        for (int col =0; col < chunks[row].size(); col++){
            chunkRefs.push_back(&chunks[row][col]);
        }   
    }
    for (Chunk* chunk : chunkRefs){
        chunk->genChunkMesh();
    }
    

    double last = glfwGetTime();
    /* Loop until the user closes the window */
    double avgFPS = 0;
    std::vector<double> fpsS;
 
    
    std::cout << "SIEMA" << "\n";
    

    while (!glfwWindowShouldClose(window))
    {
        
        double delta = glfwGetTime() - last;
        double fps = 1 / delta;
        if (fpsS.size() >= 10){
            avgFPS = 0;
            int divide = fpsS.size();
            for (int i=fpsS.size() -1 ; i>= 0; i--){
                avgFPS += fpsS[i];
                fpsS.pop_back();
            }
            avgFPS /= divide;
            std::cout << "FPS: " << avgFPS << "\n";
        }
        else{
            fpsS.push_back(fps);
        }
        

        last = glfwGetTime();
        process_input(window,chunks,delta);
        
        // std::sort(chunkRefs.begin(),chunkRefs.end(),[&](Chunk *a, Chunk *b){ 
 
        //     float aDist = glm::distance(a->position,cameraPos);
        //     float bDist = glm::distance(b->position,cameraPos);
        //     return aDist > bDist; 
        // });
        /* Render here */
        
        
        GLCall( glClearColor(0.68f, 0.84f, 0.9f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT) );

        shader.use();

        view = glm::lookAt(cameraPos,cameraPos + cameraFront, cameraUp);
        
        shader.setInt("playerState",playerState);
        shader.setVec3Float("LightPos",cameraPos);
        shader.setMatrixFloat("projection",GL_FALSE,projection);
        shader.setMatrixFloat("view",GL_FALSE,view);
        shader.setMatrixFloat("model",GL_FALSE,model);

        vao.bind();
        vbo.bind();
        ebo.bind();
    
    
        for (Chunk* chunk : chunkRefs){
         
            shader.setVec3Float("chunkPos",(*chunk).position);
            
            (*chunk).renderOpaque();
        }   
        for (Chunk* chunk : chunkRefs){
         
            shader.setVec3Float("chunkPos",(*chunk).position);
            
            (*chunk).renderTransparent();
        }   
        
                
          
        


        crosshairShader.use();
        crosshairVAO.bind();
        crosshairVBO.bind();
        GLCall( glDrawArrays(GL_TRIANGLES,0,3) );
        
        
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    }
    
    glfwTerminate();
    return 0;
}


