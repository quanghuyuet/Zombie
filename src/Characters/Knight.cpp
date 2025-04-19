#include "Knight.h"
#include <iostream>
#include "GameObject.h"
#include "Character.h"
#include <SDL.h>
#include <SDL_image.h>
#include "TextureManager.h"
#include "Animation.h"
#include "RigidBody.h"
#include "Input.h"
#include "Camera.h"
#include "CollisionHandler.h"
#include "Timer.h"
#include "Enemy.h"
#include "Zombie.h"
#include "Engine.h"
#include <SDL_ttf.h>

Knight::Knight(Properties* props) : Character(props) {
    m_JumpTime = JUMP_TIME;
    m_JumpForce = JUMP_FORCE;
    m_AtrackTime = 0.0f;
    m_IsAtracking = false;
    m_IsJumping = false;
    m_IsGrounded = false;
    m_IsRunning = false;
    m_IsCrouching = false;
    m_WasRunning = false; // Khởi tạo trạng thái chạy trước đó

    m_Collider = new Collider();
    m_Collider->SetBuffer(-53, -23, 0, 0);

    m_RigidBody = new RigidBody();
    m_RigidBody->SetGravity(80.0f);

    m_Animation = new Animation();
    m_Animation->SetProps(m_TextureID, 1, 6, 100);

    m_Origin = new Point();
    m_Health = 70;
    m_MaxHealth = 70;
    m_Mana = 5;
    m_MaxMana = 5;
    m_ManaRegenTime = 0.0f;
    m_ManaRegenRate = 3.0f;

    // Tải hiệu ứng âm thanh
    m_RunSound = Mix_LoadWAV("sound/knight_run.wav");
    m_AttackSound = Mix_LoadWAV("sound/knight_chem.wav");
    m_JumpSound = Mix_LoadWAV("sound/knight_jump.wav");

    // Đặt âm lượng cho các hiệu ứng âm thanh (0 đến 128)
    if (m_RunSound) Mix_VolumeChunk(m_RunSound, 110);    // Âm lượng chạy là 50%
    if (m_AttackSound) Mix_VolumeChunk(m_AttackSound, 20); // Âm lượng tấn công là ~62%
    if (m_JumpSound) Mix_VolumeChunk(m_JumpSound, 100);  // Âm lượng nhảy là ~78%
}

void Knight::TakeDamage(int damage) {
    m_Health -= damage;
    if (m_Health <= 0) {
        m_Health = 0;
    }
    m_Mana -= 1;
    if (m_Mana < 0) {
        m_Mana = 0;
    }
}

void Knight::Draw() {
    m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height, m_Flip);

    SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
    TTF_Font* font = Engine::GetInstance()->GetFont();

    if (font == nullptr) {
        std::cerr << "Font is not loaded!" << std::endl;
        return;
    }

    int barWidth = 200;
    int barHeight = 20;
    int hpX = 47;
    int hpY = 10;
    int mpY = hpY + barHeight + 5;

    float hpRatio = (float)m_Health / m_MaxHealth;
    float mpRatio = (float)m_Mana / m_MaxMana;

    // Vẽ chữ "HP"
    SDL_Color textColor1 = {255, 0, 0, 255};
    SDL_Surface* hpSurface = TTF_RenderText_Solid(font, "HP", textColor1);
    SDL_Texture* hpTexture = SDL_CreateTextureFromSurface(renderer, hpSurface);
    SDL_Rect hpTextRect = {11, hpY, hpSurface->w, hpSurface->h};
    SDL_RenderCopy(renderer, hpTexture, nullptr, &hpTextRect);
    SDL_FreeSurface(hpSurface);
    SDL_DestroyTexture(hpTexture);

    // Vẽ thanh HP
    SDL_Rect hpBackground = {hpX, hpY, barWidth, barHeight};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &hpBackground);
    SDL_Rect hpBar = {hpX, hpY, (int)(barWidth * hpRatio), barHeight};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hpBar);

    // Vẽ chữ "MP"
    SDL_Color textColor2 = {0, 0, 255, 255};
    SDL_Surface* mpSurface = TTF_RenderText_Solid(font, "MP", textColor2);
    SDL_Texture* mpTexture = SDL_CreateTextureFromSurface(renderer, mpSurface);
    SDL_Rect mpTextRect = {10, mpY, mpSurface->w, mpSurface->h};
    SDL_RenderCopy(renderer, mpTexture, nullptr, &mpTextRect);
    SDL_FreeSurface(mpSurface);
    SDL_DestroyTexture(mpTexture);

    // Vẽ thanh MP
    SDL_Rect mpBackground = {hpX, mpY, barWidth, barHeight};
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderFillRect(renderer, &mpBackground);
    SDL_Rect mpBar = {hpX, mpY, (int)(barWidth * mpRatio), barHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &mpBar);
}

void Knight::Update(float dt) {
    dt = Timer::GetInstance()->GetDeltaTime();
    m_IsRunning = false;
    m_IsCrouching = false;
    m_RigidBody->UnSetForce();
    m_RigidBody->UnSetFriction();

    if (!m_IsAtracking) {
        int moveDir = Input::GetInstance()->GetAxisKey(HORIZONTAL);
        if (moveDir == FORWARD) {
            m_RigidBody->ApplyForceX(300.0f);
            m_Flip = SDL_FLIP_NONE;
            m_IsRunning = true;
        } else if (moveDir == BACKWARD) {
            m_RigidBody->ApplyForceX(-300.0f);
            m_Flip = SDL_FLIP_HORIZONTAL;
            m_IsRunning = true;
        } else {
            if (m_RigidBody->Velocity().X > 0) {
                m_RigidBody->ApplyFriction(Vector2D(-200.0f, 0));
                if (m_RigidBody->Velocity().X < 5.0f && m_RigidBody->Velocity().X > -5.0f) {
                    m_RigidBody->SetVelocityX(0);
                }
            } else if (m_RigidBody->Velocity().X < 0) {
                m_RigidBody->ApplyFriction(Vector2D(200.0f, 0));
                if (m_RigidBody->Velocity().X < 5.0f && m_RigidBody->Velocity().X > -5.0f) {
                    m_RigidBody->SetVelocityX(0);
                }
            }
        }
    }

    // Phát âm thanh chạy
    if (m_IsRunning && !m_WasRunning && m_RunSound) {
        Mix_PlayChannel(-1, m_RunSound, 0);
    }
    m_WasRunning = m_IsRunning;

    if (Input::GetInstance()->GetKeyDown(SDL_SCANCODE_S) && m_IsGrounded) {
        m_IsCrouching = true;
    }

    if (Input::GetInstance()->GetKeyDown(SDL_SCANCODE_J) && !m_IsAtracking) {
        m_IsAtracking = true;
        m_AtrackTime = 0.5f;
        if (m_AttackSound) Mix_PlayChannel(-1, m_AttackSound, 0); // Phát âm thanh chém

        SDL_Rect knightAttackBox = m_Collider->Get();
        knightAttackBox.w += 20;
        if (m_Flip == SDL_FLIP_HORIZONTAL) {
            knightAttackBox.x -= 20;
        }

        for (auto enemy : Engine::GetInstance()->GetEnemies()) {
            if (enemy != nullptr && CollisionHandler::GetInstance()->CheckCollision(knightAttackBox, enemy->GetCollider()->Get())) {
                enemy->TakeDamage(2);
            }
        }
        for (auto zombie : Engine::GetInstance()->GetZombies()) {
            if (zombie != nullptr && CollisionHandler::GetInstance()->CheckCollision(knightAttackBox, zombie->GetCollider()->Get())) {
                zombie->TakeDamage(1);
            }
        }
        Boss* boss = Engine::GetInstance()->GetBoss();
        if (boss != nullptr && CollisionHandler::GetInstance()->CheckCollision(knightAttackBox, boss->GetCollider()->Get())) {
            boss->TakeDamage(2);
        }
    }

    if (m_IsAtracking) {
        m_AtrackTime -= dt;
        if (m_AtrackTime <= 0) {
            m_IsAtracking = false;
        }
    }

    if (Input::GetInstance()->GetAxisKey(VERTICAL) == 1 && m_IsGrounded && !m_IsAtracking) {
        m_IsJumping = true;
        m_IsGrounded = false;
        m_RigidBody->ApplyForceY(-1500.0f); // UPWARD là -1
        if (m_JumpSound) Mix_PlayChannel(-1, m_JumpSound, 0); // Phát âm thanh nhảy
    }

    m_RigidBody->Update(dt);

    m_LastSafePosition.X = m_Transform->X;
    m_Transform->X += m_RigidBody->Velocity().X * dt;
    m_Collider->Set(m_Transform->X, m_Transform->Y, 30, 50);

    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
        m_Transform->X = m_LastSafePosition.X;
        m_RigidBody->SetVelocityX(0);
    }

    m_LastSafePosition.Y = m_Transform->Y;
    m_Transform->Y += m_RigidBody->Velocity().Y * dt;
    m_Collider->Set(m_Transform->X, m_Transform->Y, 30, 50);

    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
        m_Transform->Y = m_LastSafePosition.Y;
        if (m_RigidBody->Velocity().Y > 0) {
            m_RigidBody->SetVelocityY(0);
            m_IsGrounded = true;
            m_IsJumping = false;
        } else if (m_RigidBody->Velocity().Y < 0) {
            m_RigidBody->SetVelocityY(0);
        }
    } else {
        m_IsGrounded = false;
    }

    m_ManaRegenTime += dt;
    if (m_ManaRegenTime >= m_ManaRegenRate) {
        m_ManaRegenTime = 0.0f;
        if (m_Mana < m_MaxMana) {
            m_Mana += 1;
        }
    }

    m_Origin->X = m_Transform->X + m_Width / 2;
    m_Origin->Y = m_Transform->Y + m_Height / 2;

    AnimationState();
    m_Animation->Update();
}

void Knight::AnimationState() {
    m_Animation->SetProps("player_idle", 0, 6, 100);
    if (m_IsRunning) { m_Animation->SetProps("player_run", 0, 8, 100); }
    if (m_IsCrouching) { m_Animation->SetProps("player_crouch", 0, 6, 200); }
    if (m_IsJumping && m_RigidBody->Velocity().Y < 0) { m_Animation->SetProps("player_jump", 0, 2, 200); }
    if (!m_IsGrounded && m_RigidBody->Velocity().Y > 0) { m_Animation->SetProps("player_fall", 0, 2, 350); }
    if (m_IsAtracking) { m_Animation->SetProps("player_attrack", 0, 14, 80); }
}

void Knight::Clean() {
    TextureManager::GetInstance()->Drop(m_TextureID);
    delete m_Collider;
    delete m_RigidBody;
    delete m_Animation;
    delete m_Origin;

    // Giải phóng âm thanh
    if (m_RunSound) Mix_FreeChunk(m_RunSound);
    if (m_AttackSound) Mix_FreeChunk(m_AttackSound);
    if (m_JumpSound) Mix_FreeChunk(m_JumpSound);
}
