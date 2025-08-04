// #include <GL/glew.h>
// #include "betterGL.h"
// #include <iostream>
// #include <sys/time.h>
// #include "world.h"
// #include "chunk.h"
// #include "buffer.h"


// int main(){
//     GLFWwindow* window;
//     glfwInit();
//     window = glfwCreateWindow(200, 200, "Hello World", NULL, NULL);
//     if (!window)
//     {
//         glfwTerminate();
//         return -1;
//     }
//     /* Make the window's context current */
//     glfwMakeContextCurrent(window);
//     glewInit();
//     Buffer vbo = Buffer(GL_ARRAY_BUFFER);
//     World world = World(176, glm::vec3(0.0,0.0,0.0), &vbo, &vbo, &vbo);
    
//     std::vector<Chunk> chunks;
//     for (int i=0; i<64; i++){
//         Chunk chunk(glm::vec3(16,0.0,16),&world);
//         chunks.push_back(chunk);
//     }
//     for (Chunk &chunk : chunks){
//         chunk.genChunk();
//         chunk.genChunkMesh();
//     }
//     std::cout << "\nstarting copying\n";
//     timespec startTime;
//     timespec endTime;
//     std::vector< Chunk > chunksCPY;

//     clock_gettime(CLOCK_REALTIME, &startTime);
//     chunksCPY = chunks;
//     // for (Chunk &chunk : chunks){
//     //     chunksCPY.push_back(chunk);
//     // }
//     clock_gettime(CLOCK_REALTIME, &endTime);

//     std::cout << "Copy time was: " << (endTime.tv_nsec - startTime.tv_nsec) /1000 /1000 << " millis\n";

    
//     return 0;
// }