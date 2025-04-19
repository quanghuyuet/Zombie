#include "Zombie.h"
#include "Engine.h"
#include "CollisionHandler.h"
#include "Timer.h"
#include "Camera.h"

Zombie::Zombie(Properties* props) : Character(props) {
    m_RigidBody = new RigidBody();
    m_RigidBody->SetGravity(100.0f);
    m_Collider = new Collider();
    m_Collider->SetBuffer(0, 18, 0, 0);

    m_Animation = new Animation();

    m_WalkingSpeed = 10.0f;
    m_DetectionRange = 150.0f;
    m_AttackRange = 40.0f;
    m_AttackCooldown = 1.5f;
    m_Health = 2;
    m_MaxHealth = 2;
    m_IsDead = false;
    m_DeathTime = 0.0f;
    m_MovingRight = true;
    m_IsAttacking = false;
    m_IsGrounded = false;
    m_OriginPoint = Vector2D(m_Transform->X, m_Transform->Y);

    m_Transform->X = props->X;
    m_Transform->Y = props->Y;

    m_LastSafePosition.X = m_Transform->X;
    m_LastSafePosition.Y = m_Transform->Y;
}

void Zombie::TakeDamage(int damage) {
    if (!m_IsDead) {
        m_Health -= damage;
        if (m_Health <= 0) {
            m_IsDead = true;
            m_DeathTime = 0.7f;
            m_RigidBody->SetVelocityX(0);
            m_RigidBody->SetVelocityY(0);
            AnimationState();
            // Play zombie death sound
            Mix_Chunk* deathSound = Engine::GetInstance()->GetZombieDeathSound();
            if (deathSound) {
                Mix_PlayChannel(-1, deathSound, 0);
            }
        }
    }
}

void Zombie::Update(float dt) {
    if (m_IsDead) {
        m_DeathTime -= dt;
        if (m_DeathTime > 0) {
            AnimationState();
            m_Animation->Update();
        }
        return;
    }

    Knight* player = Engine::GetInstance()->GetPlayer();
    m_IsAttacking = false;

    // Cập nhật vận tốc từ RigidBody
    m_RigidBody->Update(dt);
    Vector2D velocity = m_RigidBody->Velocity();

    // Di chuyển theo trục Y (rơi do trọng lực)
    float newY = m_Transform->Y + velocity.Y * dt;
    m_Collider->Set(m_Transform->X, newY, m_Width, m_Height);

    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
        if (velocity.Y > 0) { // Rơi xuống và va chạm
            m_Transform->Y = m_LastSafePosition.Y;
            m_RigidBody->SetVelocityY(0);
            m_IsGrounded = true;
        }
    } else {
        m_Transform->Y = newY;
        m_LastSafePosition.Y = m_Transform->Y;
        m_IsGrounded = false;
    }

    // Xử lý di chuyển và tấn công
    if (m_IsGrounded) {
        if (player) {
            float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                                 pow(player->GetOrigin()->Y - m_Origin->Y, 2));

            // Trong tầm phát hiện
            if (distance < m_DetectionRange) {
                // Trong tầm tấn công: dừng lại và tấn công
                if (distance < m_AttackRange) {
                    m_IsAttacking = true;
                    m_RigidBody->SetVelocityX(0); // Dừng di chuyển ngang

                    if (player->GetOrigin()->X < m_Origin->X) {
                        m_Flip = SDL_FLIP_HORIZONTAL;
                    } else {
                        m_Flip = SDL_FLIP_NONE;
                    }

                    if (m_AttackCooldown <= 0) {
                        Attack(player);
                        m_AttackCooldown = 1.5f;
                    }
                }
                // Ngoài tầm tấn công nhưng trong tầm phát hiện: di chuyển về phía knight
                else {
                    m_MovingRight = (player->GetOrigin()->X > m_Origin->X);
                    float speed = m_WalkingSpeed * dt;
                    float newX = m_Transform->X + (m_MovingRight ? speed : -speed);
                    m_Collider->Set(newX, m_Transform->Y, m_Width, m_Height);

                    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
                        m_Transform->X = m_LastSafePosition.X;
                        m_MovingRight = !m_MovingRight;
                    } else {
                        m_Transform->X = newX;
                        m_LastSafePosition.X = m_Transform->X;
                    }

                    m_Flip = m_MovingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
                }
            }
            // Ngoài tầm phát hiện: di chuyển qua lại bình thường
            else {
                float speed = m_WalkingSpeed * dt;
                float newX = m_Transform->X + (m_MovingRight ? speed : -speed);
                m_Collider->Set(newX, m_Transform->Y, m_Width, m_Height);

                if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
                    m_Transform->X = m_LastSafePosition.X;
                    m_MovingRight = !m_MovingRight;
                } else {
                    m_Transform->X = newX;
                    m_LastSafePosition.X = m_Transform->X;
                }

                m_Flip = m_MovingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            }
        }

        if (m_AttackCooldown > 0) m_AttackCooldown -= dt;
    }

    // Cập nhật điểm gốc
    m_Origin->X = m_Transform->X + m_Width / 2;
    m_Origin->Y = m_Transform->Y + m_Height / 2;

    AnimationState();
    m_Animation->Update();
}

void Zombie::Draw() {
    if (!m_IsDead || m_DeathTime > 0) {
        m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height, m_Flip);
        SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
        Vector2D cameraPos = Camera::GetInstance()->GetPosition();
        float cameraX = cameraPos.X;
        float cameraY = cameraPos.Y;
        int screenX = static_cast<int>(m_Transform->X - cameraX);
        int screenY = static_cast<int>(m_Transform->Y - cameraY - 60);
        int barWidth = 70;
        int barHeight = 8;
        int padding = -70;

        int barX = screenX + (m_Width - barWidth) / 2;
        int barY = screenY - barHeight - padding;

        float hpRatio = (float)m_Health / m_MaxHealth;
        if (hpRatio < 0) hpRatio = 0;
        if (hpRatio > 1) hpRatio = 1;

        SDL_Rect background = {barX, barY, barWidth, barHeight};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &background);

        SDL_Rect healthBar = {barX, barY, static_cast<int>(barWidth * hpRatio), barHeight};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &healthBar);
    }
}

void Zombie::Attack(Knight* player) {
    if (player) {
        float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                             pow(player->GetOrigin()->Y - m_Origin->Y, 2));
        if (distance < 40.0f) {
            player->TakeDamage(1);
        }
    }
}

void Zombie::AnimationState() {
    if (m_IsDead) {
        m_Animation->SetProps("zombie_die", 0, 15, 120);
    } else if (m_IsAttacking) {
        m_Animation->SetProps("zombie_chem", 0, 12, 100);
    } else {
        m_Animation->SetProps("zombie_dibo", 0, 12, 100);
    }
}
