#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <GL/gl.h>
#include <GL/glu.h>

#include <string>
#include <map>
#include <iostream>
#include <vector>

#include "stb/stb_image.h"
#include "cg_exception.hpp"

void FilpImageDataDiagonally(unsigned char *image, int w, int h, int comp) {
    auto swap_color = [&comp, &w, &image](int x, int y) {
        for (int i = 0; i < comp; i++) {
            unsigned char tmp = image[x + i];
            image[x + i] = image[y + i];
            image[y + i] = tmp;
        }
    };

    for (int i = 0; i < h / 2; i++) {
        for (int j = 0; j < w; j++)
            swap_color((i * w + j) * comp, ((h - 1 - i) * w + (w - 1 - j)) * comp);
    }
}

uint32_t LoadTexture(std::string url) {
    using namespace std;
    for (int i = 0; i < url.length(); i++) if (url[i] == '\\') url[i] = '/';
    GLuint texture;
    static map<string, GLuint> mem;
    if (mem.count(url)) return mem[url];
    int w, h, comp;
    unsigned char* image = stbi_load(url.c_str(), &w, &h, &comp, 0);

    if (image == nullptr) throw LoadPictureError(url);
    
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(comp == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    else if (comp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(image);
    mem[url] = texture;
    return texture;
}

uint32_t LoadCubeMap(const std::vector<std::string> &urls) {
    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, channels;
    for (int i = 0; i < urls.size(); i++) {
        auto url = urls[i];
        unsigned char *data = stbi_load(url.c_str(), &width, &height, &channels, 0);
        if (i == 2 || i == 3) {
            FilpImageDataDiagonally(data, width, height, channels);
        }
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            throw LoadPictureError(url);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

uint32_t GenerateDepthTexture(int width, int height) {
    uint32_t texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;    
}

uint32_t GenerateFrameBuffer(int width, int height, uint32_t texture) {
    uint32_t fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, texture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return fbo;
}