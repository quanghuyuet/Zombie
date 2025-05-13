#include "Engine.h"
#include "TextureManager.h"
#include "Character.h"
#include <iostream>
#include <string>
#include <cmath>
#include "Input.h"
#include "Timer.h"
#include "Camera.h"
#include "GameObject.h"
#include "algorithm"
Engine* Engine::s_Instance = nullptr;
Engine::Engine() {
    m_IsRuning = false;
    m_Window = nullptr;
    m_Renderer = nullptr;
    m_LevelMap = nullptr;
    m_Player = nullptr;
    m_Boss = nullptr;
    m_InMenu = true;
    bool m_IsGameOver = false;
    m_GameOverTriggered = false;
    m_GameOverTriggerTime = 0;
    m_Music = nullptr;
    m_ZombieDeathSound = nullptr;
    m_BossDeathSound = nullptr;
}

bool Engine::Init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Không thể khởi tạo SDL: %s", SDL_GetError());
        return false;
    }
    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
        SDL_Log("Không thể khởi tạo SDL_image: %s", IMG_GetError());
        return false;
    }
    if (TTF_Init() == -1) {
        std::cerr << "Không thể khởi tạo SDL_ttf! TTF_Error: " << TTF_GetError() << std::endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Không thể khởi tạo SDL_mixer! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }
    m_Window = SDL_CreateWindow("Zombie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!m_Window) {
        SDL_Log("Không thể tạo cửa sổ: %s", SDL_GetError());
        return false;
    }
    m_Renderer = SDL_CreateRenderer(m_Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_Renderer) {
        SDL_Log("Không thể tạo renderer: %s", SDL_GetError());
        return false;
    }
    gFont = TTF_OpenFont("text/The Bomb Sound.ttf", 25);
    if (!gFont) {
        std::cerr << "Không thể tải font! TTF_Error: " << TTF_GetError() << std::endl;
    }
    auto texMgr = TextureManager::GetInstance();
    texMgr->Load("menu_background", "img/Menu/menugame.png");
    texMgr->Load("tutorial", "img/Menu/tutorial.png");
    texMgr->Load("game_over", "img/Menu/gameover.png");
    m_Menu = new Menu(m_Renderer);
    m_Menu->setTextures(
        texMgr->GetTexture("menu_background"),
        texMgr->GetTexture("tutorial"),
        texMgr->GetTexture("game_over")
    );
    texMgr->Load("player_idle",     "img/Knight/Dung.png");
    texMgr->Load("player_run",      "img/Knight/Chay.png");
    texMgr->Load("player_jump",     "img/Knight/Jumpp.png");
    texMgr->Load("player_fall",     "img/Knight/Fall.png");
    texMgr->Load("player_crouch",   "img/Knight/Crouch.png");
    texMgr->Load("player_attrack",  "img/Knight/Attrack.png");
    texMgr->Load("enemy_fly",       "img/Enemy/FLY.png");
    texMgr->Load("enemy_death",     "img/Enemy/DEATH.png");
    texMgr->Load("enemy_attack",    "img/Enemy/ATTACK.png");
    texMgr->Load("zombie_die",      "img/Zombie/die.png");
    texMgr->Load("zombie_dibo",     "img/Zombie/dibo.png");
    texMgr->Load("zombie_chem",     "img/Zombie/chem.png");
    texMgr->Load("boss_death",      "img/Boss/Death.png");
    texMgr->Load("boss_attack",     "img/Boss/Attack.png");
    texMgr->Load("boss_run",        "img/Boss/Run.png");
    texMgr->Load("background1",     "img/Background/background1.png");

    m_Music = Mix_LoadMUS("sound/nhac_chinh.wav");
    if (!m_Music) {
        std::cerr << "Không thể tải nhạc 'sound/nhac_chinh.wav'! Mix_Error: " << Mix_GetError() << std::endl;
        std::cerr << "Vui lòng đảm bảo 'sound/nhac_chinh.wav' nằm đúng thư mục." << std::endl;
    } else {
        Mix_PlayMusic(m_Music, -1);
    }

    m_ZombieDeathSound = Mix_LoadWAV("sound/zombie_death.wav");
    if (!m_ZombieDeathSound) {
        std::cerr << "Không thể tải âm thanh 'sound/zombie_death.wav'! Mix_Error: " << Mix_GetError() << std::endl;
        std::cerr << "Vui lòng đảm bảo 'sound/zombie_death.wav' nằm đúng thư mục." << std::endl;
    }

    m_BossDeathSound = Mix_LoadWAV("sound/boss_death.wav");
    if (!m_BossDeathSound) {
        std::cerr << "Không thể tải âm thanh 'sound/boss_death.wav'! Mix_Error: " << Mix_GetError() << std::endl;
        std::cerr << "Vui lòng đảm bảo 'sound/boss_death.wav' nằm đúng thư mục." << std::endl;
    }

    m_LevelMap = Map::GetInstance();
    m_LevelMap->Init(m_Renderer);
    CollisionHandler::GetInstance();
    m_Bg1Rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    m_Bg1Width = 1280;
    m_Player = new Knight(new Properties("player", 100, 200, 136, 96));
    m_Boss   = new Boss(new Properties("boss", 2600, 100, 400, 400));
    vector<Vector2D> enemyPositions = {
        {100, 230},{400,140},{800, 200}, {1200, 160}, {1600, 170},{2000, 140},{2600, 200}
    };
    for (auto& pos : enemyPositions)
        AddEnemy(new Enemy(new Properties("enemy", pos.X, pos.Y, 81, 71)));
    vector<Vector2D> zombiePositions = {
      {260,200}, {500, 200}, {1000, 200}, {1200, 200},{700, 200},{1900, 200}, {2200, 250}
    };
    for (auto& pos : zombiePositions)
        AddZombie(new Zombie(new Properties("zombie", pos.X, pos.Y, 90, 90)));
    Camera::GetInstance()->SetTarget(m_Player->GetOrigin());
    m_MenuStartTime = SDL_GetTicks();
    return m_IsRuning = true;
}

void Engine::Update() {
    if (m_Menu->isGamePaused()) {
        if (m_Music) Mix_PauseMusic();
        return;
    }
    if (!m_IsGameOver && m_Music && Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
    float dt = Timer::GetInstance()->GetDeltaTime();
    Camera::GetInstance()->Update(dt);
    m_Player->Update(dt);
    m_Boss->Update(dt);
    for (auto enemy : m_Enemies) {
        if (enemy != nullptr) {
            enemy->Update(dt);
        }
    }

    for (auto zombie : m_Zombies) {
        if (zombie != nullptr) {
            zombie->Update(dt);
        }
    }

    if (!m_GameOverTriggered) {
        if (m_Player->GetHealth() <= 0 || m_Boss->IsDead()) {
            m_GameOverTriggered = true;
            m_GameOverTriggerTime = SDL_GetTicks();
            if (m_Music) Mix_PauseMusic();
        }
    } else {
        if (SDL_GetTicks() - m_GameOverTriggerTime >= 3000) {
            m_IsGameOver = true;
        }
    }

    m_Enemies.erase(
        remove_if(m_Enemies.begin(), m_Enemies.end(),
            [this](Enemy* enemy) {
                bool shouldRemove = (enemy->IsDead() && enemy->GetDeathTime() <= 0);
                if (shouldRemove) {
                    m_Score += 1;
                    delete enemy;
                }
                return shouldRemove;
            }),
        m_Enemies.end()
    );

    m_Zombies.erase(
        remove_if(m_Zombies.begin(), m_Zombies.end(),
            [this](Zombie* zombie) {
                bool shouldRemove = (zombie->IsDead() && zombie->GetDeathTime() <= 0);
                if (shouldRemove) {
                    m_Score += 1;
                    delete zombie;
                }
                return shouldRemove;
            }),
        m_Zombies.end()
    );

    m_LevelMap->Update();
}

void Engine::Render() {
    if (m_InMenu) {
        m_Menu->render();
    } else if (m_IsGameOver) {
        m_Menu->setScore(m_Score);
        m_Menu->setGameOver(true);
        m_Menu->render();
    } else {
        Vector2D camPos = Camera::GetInstance()->GetPosition();
        int bg1StartX = -static_cast<int>(camPos.X) % m_Bg1Width;
        if (bg1StartX > 0) bg1StartX -= m_Bg1Width;
        for (int x = bg1StartX; x < SCREEN_WIDTH + m_Bg1Width; x += m_Bg1Width) {
            TextureManager::GetInstance()->Draw("background1", x, m_Bg1Rect.y, m_Bg1Width, m_Bg1Rect.h);
        }
        m_LevelMap->Render();
        m_Player->Draw();
        m_Boss->Draw();
        for (auto enemy : m_Enemies) {
            if (enemy != nullptr) {
                enemy->Draw();
            }
        }
        for (auto zombie : m_Zombies) {
            if (zombie != nullptr) {
                zombie->Draw();
            }
        }

        m_Menu->setScore(m_Score);
        m_Menu->render();
    }
    SDL_RenderPresent(m_Renderer);
}

void Engine::Events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        Uint32 t = SDL_GetTicks();
        if (m_InMenu) {
            if (t - m_MenuStartTime < 1200) continue;
            m_Menu->handleEvents(event, m_IsRuning);
            if (m_Menu->shouldStart()) {
                m_InMenu = false;
                m_Menu->setInGame(true);
            }
            if (m_Menu->shouldQuit()) m_IsRuning = false;
        }
        else if (m_IsGameOver) {
            m_Menu->setGameOver(true); m_Menu->setInGame(false);
            m_Menu->handleEvents(event, m_IsRuning);
            if (m_Menu->shouldRestart()) {
                m_IsGameOver = m_GameOverTriggered = false;
                m_GameOverTriggerTime = m_Score = 0;
                m_InMenu = true; m_Menu->setInGame(false); m_Menu->setGameOver(false); m_Menu->resetRestartState();

                delete m_Player; m_Player = new Knight(new Properties("player", 100, 200, 136, 96));
                delete m_Boss; m_Boss = new Boss(new Properties("boss", 2600, 100, 400, 400));

                for (auto e : m_Enemies) delete e; m_Enemies.clear();
                for (auto z : m_Zombies) delete z; m_Zombies.clear();

                for (auto p : {Vector2D(800, 200), Vector2D(1200, 200), Vector2D(2000, 170), Vector2D(2600, 200)})
                    AddEnemy(new Enemy(new Properties("enemy", p.X, p.Y, 81, 71)));

                for (auto p : {Vector2D(500, 200), Vector2D(1000, 200), Vector2D(1200, 200), Vector2D(1900, 200), Vector2D(2200, 250)})
                    AddZombie(new Zombie(new Properties("zombie", p.X, p.Y, 90, 90)));

                Camera::GetInstance()->SetTarget(m_Player->GetOrigin());
            }
            if (m_Menu->shouldQuit()) m_IsRuning = false;
        }
        else {
            m_Menu->handleEvents(event, m_IsRuning);
            if (m_Menu->shouldQuit()) { m_IsRuning = false; continue; }
            if (!m_Menu->isInGame()) { m_InMenu = true; continue; }
            if (!m_Menu->isGamePaused()) {
                Input::GetInstance()->Listen();
                if (event.type == SDL_QUIT) m_IsRuning = false;
            }
        }
    }
}

bool Engine::Clean() {
    if (m_Player) {
        m_Player->Clean();
        delete m_Player;
        m_Player = nullptr;
    }
    if (m_Boss) {
        m_Boss->Clean();
        delete m_Boss;
        m_Boss = nullptr;
    }
    for (Enemy* enemy : m_Enemies) {
        enemy->Clean();
        delete enemy;
    }
    m_Enemies.clear();
    for (Zombie* zombie : m_Zombies) {
        zombie->Clean();
        delete zombie;
    }
    m_Zombies.clear();
    m_LevelMap->Clean();
    m_LevelMap = nullptr;
    TextureManager::GetInstance()->Clean();
    delete m_Menu;

    if (m_Music) {
        Mix_FreeMusic(m_Music);
        m_Music = nullptr;
    }
    if (m_ZombieDeathSound) {
        Mix_FreeChunk(m_ZombieDeathSound);
        m_ZombieDeathSound = nullptr;
    }
    if (m_BossDeathSound) {
        Mix_FreeChunk(m_BossDeathSound);
        m_BossDeathSound = nullptr;
    }
    if (gFont != nullptr) {
        TTF_CloseFont(gFont);
        gFont = nullptr;
    }
    SDL_DestroyRenderer(m_Renderer);
    SDL_DestroyWindow(m_Window);
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return true;
}

void Engine::Quit() {
    m_IsRuning = false;
}
