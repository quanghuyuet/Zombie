#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
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
    static Engine* GetInstance() {
        return s_Instance = (s_Instance != nullptr) ? s_Instance : new Engine();
    }
    bool Init(); // Khởi tạo game
    bool Clean(); // Dọn dẹp tài nguyên
    void Quit(); // Thoát game
    void Update(); // Cập nhật trạng thái game
    void Render(); // Vẽ khung hình
    void Events(); // Xử lý sự kiện
    inline Map* GetMap() { return m_LevelMap; } // Lấy bản đồ
    inline bool IsRunning() { return m_IsRuning; } // Kiểm tra game đang chạy
    inline SDL_Renderer* GetRenderer() { return m_Renderer; } // Lấy renderer

    Knight* GetPlayer() { return m_Player; } // Lấy nhân vật người chơi
    vector<Enemy*>& GetEnemies() { return m_Enemies; } // Lấy danh sách kẻ thù
    vector<Zombie*>& GetZombies() { return m_Zombies; } // Lấy danh sách zombie
    Boss* GetBoss() { return m_Boss; } // Lấy boss
    Mix_Chunk* GetZombieDeathSound() { return m_ZombieDeathSound; } // Lấy âm thanh zombie chết
    Mix_Chunk* GetBossDeathSound() { return m_BossDeathSound; } // Lấy âm thanh boss chết

    void AddEnemy(Enemy* enemy) {
        m_Enemies.push_back(enemy); // Thêm kẻ thù
    }
    void AddZombie(Zombie* zombie) {
        m_Zombies.push_back(zombie); // Thêm zombie
    }

    TTF_Font* GetFont() const { return gFont; } // Lấy font chữ
    int GetScore() const { return m_Score; } // Lấy điểm số
    bool IsGameOver() const { return m_IsGameOver; } // Kiểm tra game over

private:
    Engine();
    bool m_IsRuning; // Trạng thái chạy của game
    Map* m_LevelMap; // Bản đồ level
    SDL_Window* m_Window; // Cửa sổ SDL
    SDL_Renderer* m_Renderer; // Renderer SDL
    static Engine* s_Instance; // Instance singleton
    SDL_Rect m_Bg1Rect; // Hình chữ nhật nền
    int m_Bg1Width; // Chiều rộng nền
    Knight* m_Player; // Nhân vật người chơi
    vector<Enemy*> m_Enemies; // Danh sách kẻ thù
    vector<Zombie*> m_Zombies; // Danh sách zombie
    Boss* m_Boss; // Boss
    TTF_Font* gFont; // Font chữ
    Uint32 m_MenuStartTime; // Thời gian bắt đầu menu
    Menu* m_Menu; // Menu
    bool m_InMenu; // Trạng thái trong menu
    int m_Score = 0; // Điểm số
    bool m_IsGameOver; // Trạng thái game over
    bool m_GameOverTriggered; // Kiểm tra game over đã kích hoạt
    Uint32 m_GameOverTriggerTime; // Thời điểm kích hoạt game over
    Mix_Music* m_Music; // Nhạc nền
    Mix_Chunk* m_ZombieDeathSound; // Âm thanh khi zombie chết
    Mix_Chunk* m_BossDeathSound; // Âm thanh khi boss chết
};

