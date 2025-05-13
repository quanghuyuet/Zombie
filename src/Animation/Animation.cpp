#include "Animation.h"
#include "TextureManager.h"
using namespace std;

// Vẽ frame hiện tại của animation tại vị trí (x, y) với kích thước và lật (flip)
void Animation::Draw(float x, float y, int spriteWidth, int spriteheight, SDL_RendererFlip flip) {
    // Gọi TextureManager để vẽ frame từ texture với ID, vị trí, kích thước, hàng sprite, frame hiện tại, lật
    TextureManager::GetInstance()->DrawFrame(m_TextureID, x, y, spriteWidth, spriteheight, m_SpriteRow, m_CurrentFrame, flip);
}

// Cập nhật frame hiện tại của animation
void Animation::Update() {
    m_CurrentFrame = (SDL_GetTicks() / m_Speed) % m_FrameCount;
}

// Thiết lập thuộc tính cho animation
void Animation::SetProps(string textureID, int spriteRow, int frameCount, int speed) {

    m_TextureID = textureID;
    m_SpriteRow = spriteRow;
    m_FrameCount = frameCount;
    m_Speed = speed;
}
