#include "texture.h"

#include <iostream>

#include "betterGL.h"
#include "vendor/stb_image/stb_image.h"


Texture::Texture(std::string filePath, std::string imgFormat)
{
    unsigned int INTERNAL_FORMAT;
    int width,height,nChannels;
    unsigned char *img;
    if (imgFormat == std::string("png")){
        INTERNAL_FORMAT = GL_RGBA;
        img = stbi_load(filePath.c_str(),&width,&height,&nChannels,4);
    }
    else if (imgFormat == std::string("jpg")){
        INTERNAL_FORMAT = GL_RGB;
        img = stbi_load(filePath.c_str(),&width,&height,&nChannels,3);
    }
    else{
        std::cout << "[ ERROR TEXTURE] WRONG FILE FORMAT" << "\n";
        std::exit(EXIT_FAILURE);
    }

    
    
    
    if (img == NULL){
        std::cout << "[ ERROR TEXTURE] LOADING TEXTURE IMG" << "\n";
        std::exit(EXIT_FAILURE);
    }

    GLCall( glGenTextures(1, &m_texture) );
  
    GLCall( glBindTexture(GL_TEXTURE_2D,m_texture) );

    GLCall( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT) );
    GLCall( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT) );
    GLCall( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) );
    GLCall( glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) );

    
    GLCall( glTexImage2D(GL_TEXTURE_2D,0,INTERNAL_FORMAT,width,height,0,INTERNAL_FORMAT,GL_UNSIGNED_BYTE,img) );
    
    
    
    stbi_image_free(img);
}