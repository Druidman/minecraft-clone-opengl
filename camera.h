#ifndef CAMERA_H
#define CAMERA_H

#include <GLFW/glfw3.h>

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

class Camera {
    private:
        bool firstMouse = true;
        bool stopMoving = false;
        double yaw = 90, pitch = 0;
        
        glm::vec2 cursorPos = glm::vec2(0.0,0.0);
        glm::vec2 mouseoffset = glm::vec2(0.0,0.0);
    public:
        
        glm::vec3 up = glm::vec3(0.0,1.0,0.0);
        glm::vec3 direction = glm::vec3(0.0,0.0,1.0);

        glm::vec3 position;
        

        Camera(){}
        glm::vec3 getRightVec(){
            return glm::normalize(glm::cross(direction,up));
        }

        glm::mat4 getViewMatrix(){
            // problem with that is that camera can be at very big coord and then out precision drops
            // return glm::lookAt(this->position,this->position + this->direction, this->up);
            // SO we use this
            // however it places camera always at 0,0,0 so we need to shift our world!
            return glm::lookAt(glm::vec3(0.0f),this->direction, this->up);
        };
        void process_cursor_position_change(double xpos, double ypos){

            if (stopMoving){
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
        
        
            glm::vec3 dir;
        
                
            dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            dir.y = sin(glm::radians(pitch));
            dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        
        
            this->direction = glm::normalize(dir);
        
            cursorPos.x = xpos;
            cursorPos.y = ypos;
        
        }

        void keyReleaseEvent(GLFWwindow *window, int key){
            if (key == GLFW_KEY_ENTER){
                this->stopMoving = true;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            else if (key == GLFW_KEY_SPACE){
                this->stopMoving = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }

            
        }
};
#endif