
#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "Enemy.h"
#include "Knight.h"
#include <iostream>
#include "Zombie.h"
#include "Map.h"
#include "Boss.h"
#include "Menu.h"
class Enemy;
class Knight;
class Zombie;
class Boss;
class Map;
using namespace std;
class Engine {
public:
    static Engine* GetInstance(){
        return s_Instance = (s_Instance != nullptr)? s_Instance : new Engine();
    }

    bool Init();
    bool Clean();
    void Quit();

    void Update();
    void Render();
    void Events();

    inline Map* GetMap(){return m_LevelMap;}
    inline bool IsRunning(){return m_IsRuning;}
    inline SDL_Renderer* GetRenderer(){return m_Renderer;}

        Knight* GetPlayer() { return m_Player; }
        vector<Enemy*>& GetEnemies() { return m_Enemies; }
        vector<Zombie*>& GetZombies() { return m_Zombies; }
        Boss* GetBoss() {return m_Boss;}


void AddEnemy(Enemy* enemy) {
    m_Enemies.push_back(enemy);
}
void AddZombie(Zombie* zombie)
{
    m_Zombies.push_back(zombie);
}
TTF_Font* GetFont() const { return gFont; }
    int GetScore() const { return m_Score; } // Get score
    bool IsGameOver() const { return m_IsGameOver; }
private:
    Engine();
    bool m_IsRuning;
    Map* m_LevelMap;
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    static Engine* s_Instance;
    SDL_Rect m_Bg1Rect;
    int m_Bg1Width;
    Knight* m_Player;
    vector<Enemy*> m_Enemies;
    vector<Zombie*> m_Zombies;
    Boss* m_Boss;
    TTF_Font* gFont;
    Uint32 m_MenuStartTime; // Thời gian bắt đầu menu
    // Thêm cho skill vụ nổ
    float m_SkillCooldown;       // Cooldown của skill
    bool m_SkillReady;           // Skill có sẵn sàng không
    const float SKILL_COOLDOWN_DURATION = 0.0f; // 10 giây cooldown
    // Thêm Menu
    Menu* m_Menu;
    bool m_InMenu;
    int m_Score = 0;
    bool m_IsGameOver ;
    bool m_GameOverTriggered;       // Biến mới: Kiểm tra xem game over đã được kích hoạt chưa
    Uint32 m_GameOverTriggerTime;   // Biến mới: Lưu thời điểm game over được kích hoạt
};
