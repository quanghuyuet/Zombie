#pragma once
#include <string>
#include "SDL.h"
#include <map>

using namespace std;
class TextureManager
{
    public:
        static TextureManager* GetInstance(){ return s_Instance = (s_Instance != nullptr)? s_Instance : new TextureManager();}

        bool Load(string id, string filename);
        void Drop(string id);
        void Clean();
        void Draw(string id, int x, int y, int width, int heigt, SDL_RendererFlip flip=SDL_FLIP_NONE);
        void DrawTile(string tilesetID, int tileSize, int x, int y, int row,int frame, SDL_RendererFlip flip=SDL_FLIP_NONE);
        void DrawFrame(string id, int x, int y, int width, int heigt, int row, int frame, SDL_RendererFlip flip=SDL_FLIP_NONE);
// Thêm phương thức GetTexture
    SDL_Texture* GetTexture(std::string id) {
        if (m_TextureMap.find(id) != m_TextureMap.end()) {
            return m_TextureMap[id];
        }
        SDL_Log("Texture ID %s not found!", id.c_str());
        return nullptr;
    }
    private:
        TextureManager(){}
        map<string, SDL_Texture*> m_TextureMap;
        static TextureManager* s_Instance;
};
