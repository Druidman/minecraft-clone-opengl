#ifndef PLAYER_H
#define PLAYER_H


#include <vector>

#include "vendor/glm/glm.hpp"
#include "chunk.h"
#include "camera.h"
#include "world.h"
#include "math.h"

#include <optional>

#include <GLFW/glfw3.h>

const int PLAYER_RANGE = 100;
const int PLAYER_SPEED = 10;

enum PlayerState{
    IN_AIR, IN_WATER, ON_GROUND
};
enum PlayerAction{
    WALKING, RUNNING, FALLING
};


// Player is defined as character 2 blocks tall and one block width
// postion is a its feet
class Player{
    private:
        const glm::vec3 CAMERA_OFFSET = glm::vec3(0.0,1.5,0.0);
        const int leftMouseButton = GLFW_MOUSE_BUTTON_LEFT;
        const int rightMouseButton = GLFW_MOUSE_BUTTON_RIGHT;
        int lastRightButtonState = GLFW_RELEASE;
        int lastLeftButtonState = GLFW_RELEASE;

        bool fly = false;
        int lastFlyButtonState = GLFW_RELEASE;
    private:
        void updateState();
        void updateAction();

        void move_by(glm::vec3 dir);
        void move_to(glm::vec3 pos);
        void adjustCamera();


    public:
        GLFWwindow *window;
    

        PlayerState state;
        PlayerAction action;

        glm::vec3 position;
        Camera *camera;
        World *world;
        
    public:
        Player(glm::vec3 playerPos, World *world, Camera *camera, GLFWwindow *window){
            this->position = playerPos;
            this->camera = camera;
            this->camera->position = position + CAMERA_OFFSET;
            this->world = world;
            this->window = window;
        };
        void process_input(double delta, bool &playerMoved);
        void destroy_block();
        void place_block();

        void update(double delta);
        
        void buttonPressed(int button);
        
};

#endif

