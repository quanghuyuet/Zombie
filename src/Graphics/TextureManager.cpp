#include "TextureManager.h"
#include "Engine.h"
#include "Camera.h"

TextureManager* TextureManager::s_Instance = nullptr;

bool TextureManager::Load(std::string id, std::string filename) {
    SDL_Surface* surface = IMG_Load(filename.c_str()); // Tải file ảnh
    if (surface == nullptr) {
        SDL_Log("Failed to load texture: %s, %s", filename.c_str(), SDL_GetError());
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(Engine::GetInstance()->GetRenderer(), surface); // Chuyển surface thành texture
    if (texture == nullptr) {
        SDL_Log("Failed to create texture from surface: %s", SDL_GetError());
        return false;
    }
    SDL_FreeSurface(surface); // Giải phóng surface
    m_TextureMap[id] = texture; // Lưu texture vào map
    return true;
}

void TextureManager::Draw(std::string id, int x, int y, int width, int height, SDL_RendererFlip flip) {
    SDL_Rect srcRect = {0, 0, width, height}; // Vùng nguồn của texture
    Vector2D cam = Camera::GetInstance()->GetPosition() * 0.5; // Vị trí camera (tỷ lệ 0.5)
    SDL_Rect dstRect = {x - cam.X, y - cam.Y, width, height}; // Vùng đích trên màn hình
    if (m_TextureMap.find(id) == m_TextureMap.end()) { // Kiểm tra texture có tồn tại
        SDL_Log("Texture ID %s not found!", id.c_str());
        return;
    }
    SDL_RenderCopyEx(Engine::GetInstance()->GetRenderer(), m_TextureMap[id], &srcRect, &dstRect, 0, nullptr, flip); // Vẽ texture
}

void TextureManager::DrawFrame(std::string id, int x, int y, int width, int height, int row, int frame, SDL_RendererFlip flip) {
    SDL_Rect srcRect = {width * frame, height * row, width, height}; // Vùng nguồn (frame trong sprite sheet)
    Vector2D cam = Camera::GetInstance()->GetPosition(); // Vị trí camera
    SDL_Rect dstRect = {x - cam.X, y - cam.Y, width, height}; // Vùng đích
    SDL_RenderCopyEx(Engine::GetInstance()->GetRenderer(), m_TextureMap[id], &srcRect, &dstRect, 0, 0, flip); // Vẽ frame
}

void TextureManager::DrawTile(std::string tilesetID, int tileSize, int x, int y, int row, int frame, SDL_RendererFlip flip) {
    SDL_Rect srcRect = {tileSize * frame, tileSize * row, tileSize, tileSize}; // Vùng nguồn (tile trong tileset)
    Vector2D cam = Camera::GetInstance()->GetPosition(); // Vị trí camera
    SDL_Rect dstRect = {x - cam.X, y - cam.Y, tileSize, tileSize}; // Vùng đích
    SDL_RenderCopyEx(Engine::GetInstance()->GetRenderer(), m_TextureMap[tilesetID], &srcRect, &dstRect, 0, 0, flip); // Vẽ tile
}

void TextureManager::Drop(std::string id) {
    if (m_TextureMap.find(id) != m_TextureMap.end()) { // Kiểm tra texture tồn tại
        SDL_DestroyTexture(m_TextureMap[id]); // Giải phóng texture
        m_TextureMap.erase(id); // Xóa khỏi map
    }
}

void TextureManager::Clean() {
    std::map<std::string, SDL_Texture*>::iterator it;
    for (it = m_TextureMap.begin(); it != m_TextureMap.end(); it++)
        SDL_DestroyTexture(it->second); // Giải phóng từng texture
    m_TextureMap.clear(); // Xóa map
}
