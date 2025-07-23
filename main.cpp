#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #include <GLES3/gl3.h>
#else
    #include <GL/glew.h>
#endif

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


typedef unsigned int uint;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;


glm::mat4 model(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(45.0),(double)WINDOW_WIDTH/WINDOW_HEIGHT,0.1,1000.0);
glm::mat4 view;
Camera camera = Camera();


struct AppState {
    double *last;
    std::vector<double> *fpsS;

    Player *player;
    Shader *shader;
    VertexArray *vao;
    World *world;
    GLFWwindow *window;

    Shader *crosshairShader;
    VertexArray *crosshairVAO;
};

struct DrawArraysIndirectCommand{
    uint  count;
    uint  instanceCount;
    uint  first;
    uint  baseInstance;
};

AppState *state;
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
    double delta = glfwGetTime() - *(state->last);
    *(state->last) = glfwGetTime();
    double fps = 1 / delta;
    if (state->fpsS->size() >= 10){
        double avgFPS = 0;
        int divide = state->fpsS->size();
        for (int i=state->fpsS->size() -1 ; i>= 0; i--){
            avgFPS += state->fpsS->at(i);
            state->fpsS->pop_back();
        }
        avgFPS /= divide;
        std::cout << "FPS: " << avgFPS << "\n";
    }
    else{
        state->fpsS->push_back(fps);
    }
    
    state->player->update(delta);
    
    view = camera.getViewMatrix();
    GLCall( glClearColor(0.68f, 0.84f, 0.9f, 1.0f) );
    GLCall( glClear(GL_COLOR_BUFFER_BIT) );
    GLCall( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );

    state->shader->use();
    
    state->shader->setInt("playerState",state->player->state);
    state->shader->setMatrixFloat("projection",GL_FALSE,projection);
    state->shader->setMatrixFloat("view",GL_FALSE,view);
    state->shader->setMatrixFloat("model",GL_FALSE,model);
    #ifndef __EMSCRIPTEN__
        state->shader->setVec3Float("LightPos",glm::vec3(256,100,256));
    #endif
    state->vao->bind();
    
    GLCall( glMultiDrawArraysIndirect(GL_TRIANGLES,0,state->world->chunkRefs.size(),sizeof(DrawArraysIndirectCommand)) );

    
    
            

    state->crosshairShader->use();
    state->crosshairVAO->bind();
    GLCall( glDrawArrays(GL_TRIANGLES,0,3) );
    
    
    
    /* Swap front and back buffers */
    glfwSwapBuffers(state->window);

    /* Poll for and process events */
    glfwPollEvents();
    
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
    crosshairVBO.fillData<float>(crosshairVertices);
    crosshairVAO.setAttr(0,3,GL_FLOAT, 3 * sizeof(float), 0);

    VertexArray vao;
    Buffer vertexVbo = Buffer(GL_ARRAY_BUFFER);
    Buffer vboInst = Buffer(GL_ARRAY_BUFFER);
    Buffer indirectBuffer = Buffer(GL_DRAW_INDIRECT_BUFFER);
    Buffer ssbo = Buffer(GL_SHADER_STORAGE_BUFFER);

   

    
    vertexVbo.fillData<float>(BLOCK_FACE_VERTEX_POS);

    vertexVbo.bind();
    vao.setAttr(0,3,GL_FLOAT,4 * sizeof(float),0);
    vao.setAttr(1,1,GL_FLOAT,4 * sizeof(float),3 * sizeof(float));

    vboInst.bind();
    vao.setAttr(2,1,GL_FLOAT,sizeof(VertexDataInt),0);
    GLCall( glVertexAttribDivisor(2,1) );

    GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    GLCall( glBindVertexArray(0) );

    int worldWidth = 2048;
    World world = World(worldWidth);
    world.genWorld();
    unsigned long long sizeToAlloc = 0;
    for (Chunk* chunk: world.chunkRefs){
        chunk->createBuffer(&vboInst);
        sizeToAlloc += chunk->getMeshSize();
    }
    std::cout << sizeToAlloc << "\n";

    vboInst.allocateBuffer(sizeToAlloc);
    indirectBuffer.allocateBuffer(sizeof(DrawArraysIndirectCommand) * world.chunkRefs.size());
    ssbo.allocateBuffer(sizeof(glm::vec4) * world.chunkRefs.size()); // vec4 due to std430 in shader
    for (Chunk* chunk: world.chunkRefs){
        DrawArraysIndirectCommand data = {
            BLOCK_FACE_VERTICES_COUNT,
            (uint)chunk->transparentFacesData.size() + (uint)chunk->opaqueFacesData.size(),
            0,
            vboInst.getFilledDataSize() / sizeof(VertexDataInt)
        };
        indirectBuffer.addData<DrawArraysIndirectCommand>(data);
        chunk->fillBuffer();

        
        ssbo.addData<glm::vec4>(glm::vec4(chunk->position,0.0)); // vec4 due to std430 in shader
        
        
    }
    GLCall( glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo.getId()) );
    
    

    
    Player player = Player(glm::vec3(0.0,60.0,0.0),&world,&camera,window);
    double last = glfwGetTime();
    double avgFPS = 0;
    std::vector<double> fpsS;

    AppState cState = {
        &last,
        &fpsS,
        &player,
        &shader,
        &vao,
        &world,
        window,
        &crosshairShader,
        &crosshairVAO
    };
    state = &cState;
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(loop, 0, true);  // 0 = use requestAnimationFrame()
    #else
        while (!glfwWindowShouldClose(window)) {
            loop();
        }
        glfwTerminate();
    #endif
    

    return 0;
}


