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
#include "vertexBuffer.h"
#include "elementBuffer.h"
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
    VertexBuffer *vbo;
    VertexBuffer *vboInst;
    VertexArray *vao;
    ElementBuffer *ebo;
    World *world;
    GLFWwindow *window;

    Shader *crosshairShader;
    VertexBuffer *crosshairVBO;
    VertexArray *crosshairVAO;
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
    state->shader->setVec3Float("LightPos",glm::vec3(256,100,256));
    state->shader->setMatrixFloat("projection",GL_FALSE,projection);
    state->shader->setMatrixFloat("view",GL_FALSE,view);
    state->shader->setMatrixFloat("model",GL_FALSE,model);

    state->vao->bind();
    state->vbo->bind();
    state->ebo->bind();


    for (Chunk* chunk : state->world->chunkRefs){
        
        state->shader->setVec3Float("chunkPos",chunk->position);
        
        chunk->renderOpaque(state->vboInst);
    }   
    for (Chunk* chunk : state->world->chunkRefs){
        
        state->shader->setVec3Float("chunkPos",chunk->position);
        
        chunk->renderTransparent(state->vboInst);
    }   
    
            

    state->crosshairShader->use();
    state->crosshairVAO->bind();
    state->crosshairVBO->bind();
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
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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


    std::string vsPath = "shaders/vertexShader.vs";
    std::string fsPath = "shaders/fragmentShader.fs";
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
    vao.setAttr(2,1,GL_FLOAT, sizeof(VertexDataInt),0);
    GLCall( glVertexAttribDivisor(2,1) );
    

    GLCall( glBindBuffer(GL_ARRAY_BUFFER, 0) );
    GLCall( glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0) );
    GLCall( glBindVertexArray(0) );


    World world = World(1024);
    world.genWorld();

    
    Player player = Player(glm::vec3(0.0,60.0,0.0),&world,&camera,window);
    double last = glfwGetTime();
    double avgFPS = 0;
    std::vector<double> fpsS;

    AppState cState = {
        &last,
        &fpsS,
        &player,
        &shader,
        &vbo,
        &vboInst,
        &vao,
        &ebo,
        &world,
        window,
        &crosshairShader,
        &crosshairVBO,
        &crosshairVAO,
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


