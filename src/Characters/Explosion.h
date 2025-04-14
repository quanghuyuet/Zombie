#pragma once
#include <SDL.h>
#include <vector>
#include "Character.h"

class Explosion {
public:
    float x, y;
    int frame;
    bool active;
    float timer;

    Explosion();
    void Start(float posX, float posY);
    void Update(float deltaTime, std::vector<Character*>& characters);
    void Render(SDL_Renderer* renderer, SDL_Texture* texture);

private:
    static constexpr float EXPLOSION_DURATION = 0.5f; // Thời gian tồn tại của vụ nổ
    static constexpr int EXPLOSION_DAMAGE = 5;       // Sát thương của vụ nổ
};
