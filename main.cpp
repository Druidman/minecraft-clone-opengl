#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #include <GLES3/gl3.h>
#else
    #include <GL/glew.h>
#endif
#include <bits/stdc++.h>
#include <GLFW/glfw3.h>
#include <optional>

#include "betterGL.h"


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/stb_image/stb_image.h"

#include "shader.h"
#include "texture.h"
#include "vertexArray.h"
#include "buffer.h"
#include "block.h"
#include "chunk.h"
#include "camera.h"
#include "player.h"
#include "world.h"

#include "renderer.h"


typedef unsigned int uint;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int RENDER_DISTANCE = 16;



glm::mat4 model(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(45.0),(double)WINDOW_WIDTH/WINDOW_HEIGHT,0.1,1000.0);
glm::mat4 view;
Camera camera = Camera();

GameState *state;
Renderer renderer;
double lastDeltaTime = 0;
std::vector<double> renderFpsS;

bool canRender = true;
bool exitApp = false;
void chunkHandler(World* world, Player *player){
    glm::vec3 lastPlayerPos = player->position;  
    Chunk* lastPlayerChunk = world->getChunkByPos(lastPlayerPos).value(); // no need to check that
    while (!exitApp)    
    {
        if (lastPlayerPos == player->position){
            continue;
        }

        lastPlayerPos = player->position;

        std::optional<Chunk *> chunkRes = world->getChunkByPos(player->position);
        if (!chunkRes.has_value()){
            // idk player is outside map
            continue;
        }
        
        Chunk* chunk = chunkRes.value();

        if (chunk == lastPlayerChunk){
            continue;
        }
        lastPlayerChunk = chunk;
        // it is different chunk so we got to update
        int row = world->getChunkRow(chunk);
        int col = world->getChunkCol(chunk);

        int rows = world->chunks.size();
        int columns = world->chunks[row].size();

        // if (row > rows / 2){
        //     //we need to delete chunks on top and add chunk to the bottom
        //     std::cout << "Add to bottom\n";
            
        //     world->chunks.erase(world->chunks.begin()); // delete top
        //     world->genChunkRefs();
        //     std::cout << "deleted\n";
        //     // now we need to add bottom chunks but these are not generated yet

        //     // filling buffer with new chunk data
        //     canRender = false;
        //     world->fillBuffers();
        //     canRender = true;
        // }
        // else if (row < rows / 2){
        //     //we need to delete chunks on bottom and add chunk to the top
        //     std::cout << "Add to top\n";
        //     world->chunks.pop_back(); // delete bottom
        //     world->genChunkRefs();
        //     std::cout << "deleted\n";
        //     // now we need to add top chunks but these are not generated yet
        //     // filling buffer with new chunk data
        //     canRender = false;
        //     world->fillBuffers();
        //     canRender = true;
        // }

        // if (col > columns / 2){
        //     //we need to delete chunks from left side and add chunk to the right side
        //     std::cout << "Add to right\n";
        //     for (int i =0; i < world->chunks.size(); i++){
        //         world->chunks[i].erase(world->chunks[i].begin()); // delete left
        //     }
        //     world->genChunkRefs();
        //     std::cout << "deleted\n";
        //     // now we need to add right chunks but these are not generated yet
        //     // filling buffer with new chunk data
        //     canRender = false;
        //     world->fillBuffers();
        //     canRender = true;
        // }
        // else if (col < columns / 2){
        //     //we need to delete chunks from right side and add chunk to the left side
        //     std::cout << "Add to left\n";
        //     for (int i =0; i<world->chunks.size(); i++){
        //         world->chunks[i].pop_back(); // delete right
        //     }
        //     world->genChunkRefs();
        //     std::cout << "deleted\n";
        //     // now we need to add left chunks but these are not generated yet
        //     // filling buffer with new chunk data
        //     canRender = false;
        //     world->fillBuffers();
        //     canRender = true;
        // }
    }
}

void resize_callback(GLFWwindow *window, int width, int height){
    glViewport(0,0,width,height);
    projection = glm::perspective(glm::radians(45.0),(double)width/height,0.1,1000.0);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

void process_key_release(GLFWwindow *window, int key){
    if (key ==GLFW_KEY_ESCAPE){
        glfwSetWindowShouldClose(window,true);
    }
     
    camera.keyReleaseEvent(window,key); 
}

void input_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
    if (action == GLFW_RELEASE){
        process_key_release(window,key);
    }
    
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos){
    camera.process_cursor_position_change(xpos,ypos);
}

void loop(){
    double delta = glfwGetTime() - lastDeltaTime;
    lastDeltaTime = glfwGetTime();
    double fps = 1 / delta;
    if (renderFpsS.size() >= 10){
        double avgFPS = 0;
        int divide = renderFpsS.size();
        for (int i=renderFpsS.size() -1 ; i>= 0; i--){
            avgFPS += renderFpsS.at(i);
            renderFpsS.pop_back();
        }
        avgFPS /= divide;
        // std::cout << "FPS: " << avgFPS << "\n";
    }
    else{
        renderFpsS.push_back(fps);
    }

    state->player->update(delta);
    
    
    view = camera.getViewMatrix();

    //wait for buffers to get ready
    while (!canRender){

    }
    renderer.render(state);
    
}

int main()
{
    
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    /* Create a windowed mode window and its OpenGL context */
    #ifdef __EMSCRIPTEN__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); // WebGL 2.0 maps to ES 3.0
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif
    

    
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);




    glfwSetWindowSizeCallback(window,resize_callback);
    glfwSetKeyCallback(window,input_callback);
    glfwSetCursorPosCallback(window,cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);


    #ifndef  __EMSCRIPTEN__
        glewInit();
    #endif

    glfwSwapInterval( 0 );

    stbi_set_flip_vertically_on_load(true);
    
    GLCall( glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT) ) ;
    GLCall( glEnable(GL_DEPTH_TEST) );
    GLCall( glEnable(GL_BLEND) ) ;
    GLCall( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ) ;

    #ifdef __EMSCRIPTEN__
        std::string vsPath = "shaders/vertexShaderWeb.vs";
        std::string fsPath = "shaders/fragmentShaderWeb.fs";
    #else
        std::string vsPath = "shaders/vertexShader.vs";
        std::string fsPath = "shaders/fragmentShader.fs";
    #endif
    std::string cvsPath = "shaders/vertexShaderCrosshair.vs";
    std::string cfsPath = "shaders/fragmentShaderCrosshair.fs";
    
    Shader shader = Shader(vsPath, fsPath);
    Shader crosshairShader = Shader(cvsPath,cfsPath);

    std::string texturePath = "textures/texture.png";

    Texture texture = Texture(texturePath, "png");

    std::vector<float> crosshairVertices = {
        -0.01f, -0.01f, 0.0f,
        0.01f, -0.01f, 0.0f,
        0.0f,  0.0f, 0.0f
    };

    VertexArray crosshairVAO = VertexArray();
    Buffer crosshairVBO = Buffer(GL_ARRAY_BUFFER);
    crosshairVBO.fillData<float>(&crosshairVertices);
    crosshairVAO.setAttr(0,3,GL_FLOAT, 3 * sizeof(float), 0);

    VertexArray vao;
    Buffer baseVbo = Buffer(GL_ARRAY_BUFFER);
    Buffer instanceBuffer = Buffer(GL_ARRAY_BUFFER);
    Buffer indirectBuffer = Buffer(GL_DRAW_INDIRECT_BUFFER);
    Buffer ssbo = Buffer(GL_SHADER_STORAGE_BUFFER);

   

    
    baseVbo.fillData<float>(&BLOCK_FACE_VERTICES);

    baseVbo.bind();
    vao.setAttr(0,3,GL_FLOAT,4 * sizeof(float),0);
    vao.setAttr(1,1,GL_FLOAT,4 * sizeof(float),3 * sizeof(float));

    instanceBuffer.bind();
    vao.setAttr(2,1,GL_FLOAT,sizeof(CHUNK_MESH_DATATYPE),0);
    GLCall( glVertexAttribDivisor(2,1) );

    GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    GLCall( glBindVertexArray(0) );

    int worldWidth = 160;
    
    World world = World(worldWidth, &instanceBuffer, &indirectBuffer, &ssbo);
    Player player = Player(glm::vec3(30000,60.0,30000),&world,&camera,window);

    world.player = &player;

    world.genWorldBase();
    world.fillBuffers();

    std::thread chunkHandle(chunkHandler, &world, &player);

    double last = glfwGetTime();
    double avgFPS = 0;
    std::vector<double> fpsS;
    GameState cState = {
        &player,
        &world,
        &shader,
        &vao,
        window,
        &crosshairShader,
        &crosshairVAO,
        &model,
        &view,
        &projection
    };
    state = &cState;
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(loop, 0, true);  // 0 = use requestAnimationFrame()
        exitApp = true;
        chunkHandle.join();
    #else
        while (!glfwWindowShouldClose(window)) {
            loop();
        }
        exitApp = true;
        chunkHandle.join();
        glfwTerminate();
    #endif
    

    return 0;
}


