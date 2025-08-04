#ifndef MODELS_H

#define MODELS_H
#include <vector>
#include "vendor/glm/glm.hpp"

struct TREE {
    inline static const std::vector<glm::vec3> woodPositions = {
        glm::vec3(0.0,0.0,0.0),
        glm::vec3(0.0,1.0,0.0),
        glm::vec3(0.0,2.0,0.0),
        glm::vec3(0.0,3.0,0.0),
        glm::vec3(0.0,4.0,0.0),
        glm::vec3(0.0,5.0,0.0),
    };
    inline static const std::vector<glm::vec3> leafPositions = {
        glm::vec3(0.0,6.0,0.0),
        glm::vec3(0.0,5.0,1.0),
        glm::vec3(1.0,5.0,1.0),
        glm::vec3(1.0,5.0,0.0),
        glm::vec3(1.0,5.0,-1.0),
        glm::vec3(0.0,5.0,-1.0),
        glm::vec3(-1.0,5.0,-1.0),
        glm::vec3(-1.0,5.0,0.0),
        glm::vec3(-1.0,5.0,1.0),
        glm::vec3(0.0,5.0,2.0),
        glm::vec3(1.0,5.0,2.0),
        glm::vec3(2.0,5.0,2.0),
        glm::vec3(2.0,5.0,1.0),
        glm::vec3(2.0,5.0,0.0),
        glm::vec3(2.0,5.0,-1.0),
        glm::vec3(2.0,5.0,-2.0),
        glm::vec3(1.0,5.0,-2.0),
        glm::vec3(0.0,5.0,-2.0),
        glm::vec3(-1.0,5.0,-2.0),
        glm::vec3(-2.0,5.0,-2.0),
        glm::vec3(-2.0,5.0,-1.0),
        glm::vec3(-2.0,5.0,0.0),
        glm::vec3(-2.0,5.0,1.0),
        glm::vec3(-2.0,5.0,2.0),
        glm::vec3(-1.0,5.0,2.0),
        




    };
};

#endif
