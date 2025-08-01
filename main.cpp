// testing phase is defined as:
//  running game with web or desktop rendering technique ON DESKTOP

// during testing define TEST and define which instance you are testing:
// WEB_GL_INSTANCE
// OPENGL_INSTANCE
#define TEST
#define WEB_GL_INSTANCE

#ifndef TEST 
    #ifdef __EMSCRIPTEN__
        #define WEB_GL_INSTANCE
        #include <emscripten/emscripten.h>
    #else
        #define OPENGL_INSTANCE
    #endif
#endif

#include "vendor/glad/glad.h"


#include <bits/stdc++.h>
#include <GLFW/glfw3.h>
#include <optional>

#include "betterGL.h"


#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"
#include "vendor/stb_image/stb_image.h"

#include "block.h"
#include "camera.h"
#include "player.h"
#include "world.h"

#include "renderer.h"
#include "desktopRenderer.h"
#include "webRenderer.h"


typedef unsigned int uint;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int RENDER_DISTANCE = 16;



glm::mat4 model(1.0f);
glm::mat4 projection = glm::perspective(glm::radians(45.0),(double)WINDOW_WIDTH/WINDOW_HEIGHT,0.1,1000.0);
glm::mat4 view;
Camera camera = Camera();

GameState *state;
Renderer* renderer;
double lastDeltaTime = 0;
std::vector<double> renderFpsS;
bool exitApp = false;

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
        std::cout << "FPS: " << avgFPS << "\n";
    }
    else{
        renderFpsS.push_back(fps);
    }

    state->world->player->update(delta);
    state->world->updateChunks();
    
    view = camera.getViewMatrix();

    renderer->render(state);
   
    
}


void setWindowHints(){
    #ifdef WEB_GL_INSTANCE // no matter if testing or no
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); // WebGL 2.0 maps to ES 3.0
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #endif
}

int main()
{

    if (!glfwInit())
        return -1;
    
    setWindowHints();

    GLFWwindow* window;
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

    
    #ifdef WEB_GL_INSTANCE
        
        if (!gladLoadGLES2(glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD for OpenGL ES 3.0" << std::endl;
            return -1;
        }
    #else
        
        if (!gladLoadGL(glfwGetProcAddress)) {
            std::cerr << "Failed to initialize GLAD for OpenGL 4.6 Core" << std::endl;
            return -1;
        }
    #endif

    glfwSwapInterval( 0 );

    stbi_set_flip_vertically_on_load(true);
    
    GLCall( glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT) ) ;
    GLCall( glEnable(GL_DEPTH_TEST) );
    GLCall( glEnable(GL_BLEND) ) ;
    GLCall( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) ) ;

   
    #if defined(WEB_GL_INSTANCE)
        renderer = new WebRenderer();
    #elif defined(OPENGL_INSTANCE)
        renderer = new DesktopRenderer();
    #else
        #error Is this webGlInstance or desktopInstance? Why is it not defined??
    #endif

    int worldWidth = 512;
    glm::vec3 worldMiddle = glm::vec3(30000,60.0,30000);

    std::cout << "initializing world\n";
    World world = World(worldWidth, worldMiddle, renderer);

    std::cout << "initializing player\n";
    Player player = Player(worldMiddle,&world,&camera,window);

    std::cout << "Renderer init\n";
    renderer->init(&world);

    std::cout << "World init\n";
    world.init(&player);


    world.genWorldBase();


    double last = glfwGetTime();
    double avgFPS = 0;
    std::vector<double> fpsS;
    GameState cState = {
        window,
        &model,
        &view,
        &projection,
        &world
    };
    state = &cState;
    #if defined(WEB_GL_INSTANCE) && !defined(TEST)
        emscripten_set_main_loop(loop, 0, true);  // 0 = use requestAnimationFrame()
        exitApp = true;
        delete renderer;

    #else
        while (!glfwWindowShouldClose(window)) {
            loop();
        }
        exitApp = true;
        std::cout << "\nexiting\n";
        delete renderer;
        glfwTerminate();
    #endif
    

    return 0;
}


