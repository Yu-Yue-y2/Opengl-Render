#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include<iostream>
#include <assert.h>
Texture::Texture() 
    : name(nullptr), width(0), height(0), color_format1(0), color_format2(0)
{
    glGenTextures(1, &id);
}
Texture::Texture(const char* path, const char* _name, bool srgb, bool reverse_y)
{
    glGenTextures(1, &id);
    Load(path, srgb, reverse_y);
    name = _name;
}
Texture::Texture(int _width, int _height, GLenum _color_format1, GLenum _color_format2)
    : width(_width), height(_height), color_format1(_color_format1),color_format2(_color_format2), name(nullptr)
{
    glGenTextures(1, &id);
    Load(nullptr);
}
void Texture::Active()
{
    glBindTexture(GL_TEXTURE_2D, id);
}
void Texture::DisActive()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
void Texture::bind()
{
    glBindTexture(GL_TEXTURE_2D, id);
}
void Texture::Resize(int _width, int _height)
{
    //if (name)
     //   return;
    width = _width;
    height = _height;
    Load(nullptr);
}
void Texture::AttachDepth()
{
    if (color_format1 == GL_DEPTH_COMPONENT && color_format2 == GL_DEPTH_COMPONENT)
    {
        Active();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
        DisActive();
    }
}
void Texture::SetName(const char* _name)
{
    name = _name;
}
void Texture::Load(const char* path, bool srgb, bool reverse_y)
{
    Active();
    if (!path)
    {
        assert(width != 0 && height != 0 && color_format1 != 0 && color_format2 != 0);
        glTexImage2D(GL_TEXTURE_2D, 0, color_format1, width, height, 0, color_format2, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //depth texture
        if (color_format1 == GL_DEPTH_COMPONENT && color_format2 == GL_DEPTH_COMPONENT)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }
    }
    else
    {
        int nrComponents;
        stbi_set_flip_vertically_on_load(reverse_y);
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum color_format1, color_format2;
            if (nrComponents == 1) {
                color_format1 = GL_RED;
                color_format2 = GL_RED;
            }
            else if (nrComponents == 3) {
                if (srgb)
                    color_format1 = GL_SRGB;
                else
                    color_format1 = GL_RGB;
                color_format2 = GL_RGB;
            }
            else if (nrComponents == 4) {
                if (srgb)
                    color_format1 = GL_SRGB_ALPHA;
                else
                    color_format1 = GL_RGBA;
                color_format2 = GL_RGBA;
            }
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, color_format1, width, height, 0, color_format2, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }
    }
	DisActive();
}
void CubeTexture::Load(std::vector<std::string>& faces)
{
    Active();
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    DisActive();
}