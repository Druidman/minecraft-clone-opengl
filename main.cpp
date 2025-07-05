#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>

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

bool removeChunk = false;


void resize_callback(GLFWwindow *window, int width, int height){
    glViewport(0,0,width,height);
    projection = glm::perspective(glm::radians(45.0),(double)width/height,0.1,1000.0);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}
void destroyBlock(std::vector< std::vector< Chunk > > &chunks){
    glm::vec3 playerPos = cameraPos;
    glm::vec3 playerDirection = glm::normalize(cameraFront);

    // / glm::vec3 playerTarget = playerPos - ((float)PLAYER_RANGE * playerDirection);
    // chunk row = ind * CHUNK_WIDTH;
    //raycast approach;
    glm::vec3 target = playerPos;
    bool targetFound = false;
    for (int i=0; i<PLAYER_RANGE; i++){
    
        target -= playerDirection;

        int chunkRow = (int)(std::floor(target.z / (float)CHUNK_WIDTH));
        int chunkCol = (int)(std::floor(target.x / (float)CHUNK_WIDTH));
        if (chunkRow >= chunks.size() ||
            chunkCol >= chunks[chunkRow].size()
        ){
            
            continue ;
        }

        Chunk &currentChunk = chunks[chunkRow][chunkCol];

        if (target.y < currentChunk.position.y){
            
            continue ; 
        }

        glm::vec3 inChunk = target - currentChunk.position + glm::vec3( CHUNK_WIDTH / 2, 0.0 , CHUNK_WIDTH / 2 );
        int platformB = std::floor(inChunk.y);
        int rowB = std::floor(inChunk.z);
        int columnB = std::floor(inChunk.x);

        if (platformB > currentChunk.blocks.size() - 1){
       
            continue ; 
        }
        if (currentChunk.blocks[platformB][rowB][columnB].type != NONE_BLOCK){
            // that would be it for destroy logic
            // currentChunk.blocks[platformB][rowB][columnB].type = NONE_BLOCK;
            // currentChunk.update();

            //lets find blocks right next to each FOUND block face to know at which face place block;
            // back ray of length BLOCK_WIDTH / 2 then lengths IMO?



            
            break;
        }
        
        
    }

    
    
   
    
    
    

    

    
    
    
}
void process_key_release(GLFWwindow *window, int key){
    if (key ==GLFW_KEY_ESCAPE){
        glfwSetWindowShouldClose(window,true);
    }
     
    if (key == GLFW_KEY_ENTER){
        removeChunk = true;
    }
    // if (key == GLFW_KEY_SPACE){
    //     destroyBlock();
    // }
}

void process_key_press(GLFWwindow *window, int key){
    
}

void process_input(GLFWwindow *window, auto &chunks){
    float speed = 1;
    if (glfwGetKey(window,GLFW_KEY_W)){
        cameraPos -= speed * cameraFront;
    }
    if (glfwGetKey(window,GLFW_KEY_S)){
        cameraPos += speed * cameraFront;
    }
    if (glfwGetKey(window,GLFW_KEY_D)){
        cameraPos += speed * glm::normalize(glm::cross(cameraUp,cameraFront));
    }
    if (glfwGetKey(window,GLFW_KEY_A)){
        cameraPos -= speed * glm::normalize(glm::cross(cameraUp,cameraFront));
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE){
        destroyBlock(chunks);
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
    
    if (firstMouse)
    {
        
        cursorPos.x = xpos;
        cursorPos.y = ypos;
        firstMouse = false;
    }
    
    
    
    double changeX = cursorPos.x - xpos;
    double changeY = cursorPos.y - ypos;

    

    yaw -= changeX * 0.1;
    pitch -= changeY * 0.1;

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

std::vector<int> world_gen(int sizex , int sizey){
    std::vector<int> world(sizex * sizey, 0.0f);

        // Create and configure noise state
    fnl_state noise = fnlCreateState();
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.frequency = 0.005;
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

int main(void)
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glewInit();

    stbi_set_flip_vertically_on_load(true);
    
    GLCall( glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT) ) ;
    GLCall( glEnable(GL_DEPTH_TEST) );



    


    std::filesystem::path cwd = std::filesystem::current_path();

    std::filesystem::path vsPath = cwd / "shaders/vertexShader.vs";
    std::filesystem::path fsPath = cwd / "shaders/fragmentShader.fs";
    
    Shader shader = Shader(vsPath.string(), fsPath.string());


    std::filesystem::path texturePath = cwd / "textures/texture.jpg";

    Texture texture = Texture(texturePath, "jpg");

    VertexArray vao = VertexArray();
    VertexBuffer vbo = VertexBuffer();
    ElementBuffer ebo = ElementBuffer();
    VertexBuffer vboInstancedPos = VertexBuffer();
    VertexBuffer vboInstancedTex = VertexBuffer();
    
 
    vbo.fillData<float>(blockVertices,BLOCK_VERTICES_COUNT);
    ebo.fillData(blockIndicies,BLOCK_INDICIES_COUNT);

    vbo.bind();
    vao.setAttr(0,3,GL_FLOAT,8 * sizeof(float),0);
    vao.setAttr(1,2,GL_FLOAT,8 * sizeof(float),3 * sizeof(float));
    vao.setAttr(2,3,GL_FLOAT,8 * sizeof(float),5 * sizeof(float));

    vboInstancedPos.bind();
    vao.setAttr(3,3,GL_FLOAT,3 * sizeof(float),0);
    GLCall( glVertexAttribDivisor(3,1) );

    vboInstancedTex.bind();
    vao.setAttr(4,2,GL_FLOAT,2 * sizeof(float),0);
    GLCall( glVertexAttribDivisor(4,1) );

    GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    GLCall( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    GLCall( glBindVertexArray(0) );


    const int WORLD_WIDTH = 1024;
    const unsigned long long WORLD_BLOCKS_COUNT = WORLD_WIDTH * WORLD_WIDTH;
    const unsigned long long WORLD_CHUNKS_COUNT = WORLD_BLOCKS_COUNT / (CHUNK_WIDTH * CHUNK_WIDTH);
    std::vector<int> world = world_gen(WORLD_WIDTH,WORLD_WIDTH);

    std::vector< std::vector <Chunk> > chunks;


    int startX = 0;
    int startY = 0;

    for (int row = 0; row < WORLD_WIDTH / CHUNK_WIDTH; row++){
        for (int column = 0; column < WORLD_WIDTH / CHUNK_WIDTH; column++){
            if (startX >= WORLD_WIDTH){
                startX = 0;
                startY += CHUNK_WIDTH;
            }

            glm::vec3 chunkPos = glm::vec3(startX + (CHUNK_WIDTH / 2), 0.0, startY + (CHUNK_WIDTH / 2));
            Chunk chunk = Chunk(chunkPos,vboInstancedPos,vboInstancedTex);
            
            
            for (int i =startX; i< startX + CHUNK_WIDTH ; i++){
                for (int j =startY; j< startY + CHUNK_WIDTH; j++){

                    float zCoord = world[j * WORLD_WIDTH + i] + 0.5;
                    BlockType blockType = GRASS_DIRT;
                    if (zCoord < 30){
                     
                        blockType = SAND;
                        
                        
                    }
                    else if (zCoord > 60){
                        blockType = STONE;
                    }
                    
                    Block block(blockType,glm::vec3(i + 0.5, zCoord ,j + 0.5));
                    
                    chunk.addBlock(block);
                    
                }
            }
            
            chunk.update();
            
            
            if (column == 0){
                chunks.push_back(std::vector< Chunk >(1,chunk));
            }
            else{
                chunks[row].push_back(chunk);
            }
            
            startX += CHUNK_WIDTH;
        }
    }

  
    
        
    

    double last = glfwGetTime();
    /* Loop until the user closes the window */
    double avgFPS = 0;
    std::vector<double> fpsS;
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
        process_input(window,chunks);
        /* Render here */
        if (removeChunk){
            int rows = chunks.size();
            int lastRow = rows - 1;
            if (chunks[lastRow].size() == 1){
                chunks.pop_back();
            }
            else {
                chunks[lastRow].pop_back();
            }
            
            removeChunk = false;
        }
        
        GLCall( glClearColor(0.68f, 0.84f, 0.9f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT) );

        shader.use();

        view = glm::lookAt(cameraPos,cameraPos - cameraFront, cameraUp);
        
        shader.setVec3Float("LightPos",cameraPos);
        shader.setMatrixFloat("projection",GL_FALSE,projection);
        shader.setMatrixFloat("view",GL_FALSE,view);
        shader.setMatrixFloat("model",GL_FALSE,model);

        vao.bind();
        vbo.bind();
        ebo.bind();

        
        for (int row = 0; row < chunks.size(); row++){
            for (int column = 0; column < chunks[row].size(); column++){
                
                chunks[row][column].render();

                
                
            }
            
        }
        
        
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    }
    
    glfwTerminate();
    return 0;
}

