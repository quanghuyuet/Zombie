#pragma once

#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
using namespace std;
class Animation{

public:
    void Update();
    void Draw(float x, float y, int spriteWidth,int spriteheight,SDL_RendererFlip flip = SDL_FLIP_NONE);
    void SetProps(string textureID,int spriteRow,int frameCount, int speed);
private:
    int m_SpriteRow, m_CurrentFrame;
    int m_Speed,m_FrameCount;
    string m_TextureID;
};
