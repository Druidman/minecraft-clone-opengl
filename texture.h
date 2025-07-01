#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>



class Texture{
    private:
        unsigned int m_texture;

    public:
        Texture(std::string imagePath, std::string imgFormat);


};

#endif