#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <filesystem>
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
    glfwSetKeyCallback(window,input_callback);
    glfwSetCursorPosCallback(window,cursor_pos_callback);
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


    World world = World(512);
    world.genWorld();

    
    Player player = Player(glm::vec3(0.0,60.0,0.0),&world,&camera,window);


    double last = glfwGetTime();
    double avgFPS = 0;
    std::vector<double> fpsS;
    while (!glfwWindowShouldClose(window))
    {
        
        double delta = glfwGetTime() - last;
        last = glfwGetTime();
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
        
        player.update(delta);
        
        view = camera.getViewMatrix();
        GLCall( glClearColor(0.68f, 0.84f, 0.9f, 1.0f) );
        GLCall( glClear(GL_COLOR_BUFFER_BIT) );

        shader.use();
        
        shader.setInt("playerState",player.state);
        shader.setVec3Float("LightPos",glm::vec3(0.0,60,0.0));
        shader.setMatrixFloat("projection",GL_FALSE,projection);
        shader.setMatrixFloat("view",GL_FALSE,view);
        shader.setMatrixFloat("model",GL_FALSE,model);

        vao.bind();
        vbo.bind();
        ebo.bind();
    
    
        for (Chunk* chunk : world.chunkRefs){
         
            shader.setVec3Float("chunkPos",(*chunk).position);
            
            (*chunk).renderOpaque(&vboInst);
        }   
        for (Chunk* chunk : world.chunkRefs){
         
            shader.setVec3Float("chunkPos",(*chunk).position);
            
            (*chunk).renderTransparent(&vboInst);
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


