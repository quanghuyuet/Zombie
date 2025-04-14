#include "Animation.h"
#include "TextureManager.h"
using namespace std;

void Animation::Draw(float x, float y, int spriteWidth,int spriteheight,SDL_RendererFlip flip){
    TextureManager::GetInstance()->DrawFrame(m_TextureID,x,y,spriteWidth,spriteheight,m_SpriteRow,m_CurrentFrame, flip);
}

void Animation::Update(){

   m_CurrentFrame = (SDL_GetTicks()/ m_Speed) % m_FrameCount;

}

void Animation::SetProps(string textureID,int spriteRow,int frameCount, int speed){

    m_TextureID = textureID;
    m_SpriteRow = spriteRow;
    m_FrameCount = frameCount;
    m_Speed = speed;
}
