#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include "Vector2D.h"
#include "Collider.h"
#include "Camera.h"
#include "TextureManager.h"
#include "Engine.h"
using namespace std;
class Bullet {
public:
    Vector2D position;
    Vector2D velocity;
    Collider* collider;
    bool active;
    string textureID;

    Bullet(float x, float y, Vector2D vel, const string& texID = "bullet");
    void Update(float dt);
    void Draw();
};
