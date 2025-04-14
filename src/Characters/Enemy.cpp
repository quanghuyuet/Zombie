#include "Enemy.h"

Enemy::Enemy(Properties* props) : Character(props) {
    m_Collider = new Collider();
    m_Animation = new Animation();
    m_Animation->SetProps(m_TextureID, 1, 4, 100);

    m_ShootCooldown = 0;
    m_IsDead = false;
    m_DeathTime = 0;
    m_Health = 2;
    m_MaxHealth = 2;

    m_DetectionRange = 300.0f; // Táº§m nháº£ Ä‘áº¡n
    m_FlyingSpeed = 120.0f;
    m_FlyingRange = 400.0f; // Pháº¡m vi di chuyá»ƒn cá»‘ Ä‘á»‹nh
    m_OriginPoint = Vector2D(props->X, props->Y);
    m_MovingRight = true;
    m_IsAttacking = false;

    TextureManager::GetInstance()->Load("bullet", "img/Enemy/bullet.png");

    m_Transform->X = props->X;
    m_Transform->Y = props->Y;
    m_Collider->Set(m_Transform->X, m_Transform->Y, 35, 45);

}

void Enemy::Draw() {
    // KhÃ´ng Ä‘iá»u chá»‰nh tá»a Ä‘á»™ theo camera ná»¯a, chá»‰ váº½ á»Ÿ tá»a Ä‘á»™ tháº¿ giá»›i
    if (!m_IsDead || m_DeathTime > 0) {
        // Giáº£ sá»­ renderer sáº½ tá»± xá»­ lÃ½ chuyá»ƒn Ä‘á»•i sang tá»a Ä‘á»™ mÃ n hÃ¬nh náº¿u cáº§n
        m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height, m_Flip);
        SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
        // BÆ°á»›c 1: Láº¥y tá»a Ä‘á»™ camera tá»« GetPosition()
        Vector2D cameraPos = Camera::GetInstance()->GetPosition();
        float cameraX = cameraPos.X;
        float cameraY = cameraPos.Y;
        // BÆ°á»›c 2: TÃ­nh tá»a Ä‘á»™ cá»§a boss trÃªn mÃ n hÃ¬nh (screen space)
        int screenX = static_cast<int>(m_Transform->X - cameraX);
        int screenY = static_cast<int>(m_Transform->Y - cameraY - 60);
        // BÆ°á»›c 3: TÃ­nh vá»‹ trÃ­ thanh mÃ¡u dá»±a trÃªn tá»a Ä‘á»™ mÃ n hÃ¬nh cá»§a boss
        int barWidth = 70;
        int barHeight = 10;
        int padding = -60;

        int barX = screenX + (m_Width - barWidth) / 2; // CÄƒn giá»¯a thanh mÃ¡u theo chiá»u ngang cá»§a boss
        int barY = screenY - barHeight - padding;      // Äáº·t thanh mÃ¡u ngay trÃªn Ä‘áº§u boss

        float hpRatio = (float)m_Health / m_MaxHealth;
        if (hpRatio < 0) hpRatio = 0;
        if (hpRatio > 1) hpRatio = 1;

        // Váº½ ná»n cá»§a thanh mÃ¡u
        SDL_Rect background = {barX, barY, barWidth, barHeight};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &background);

        // Váº½ thanh mÃ¡u
        SDL_Rect healthBar = {barX, barY, static_cast<int>(barWidth * hpRatio), barHeight};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &healthBar);
    }

        // Váº½ Ä‘áº¡n á»Ÿ tá»a Ä‘á»™ tháº¿ giá»›i
        for (auto bullet : m_Bullets) {
            if (bullet->active) {
                bullet->Draw();
            }
        }

}

void Enemy::UpdateBullets(float dt) {
    for (auto it = m_Bullets.begin(); it != m_Bullets.end();) {
        if ((*it)->active) {
            (*it)->Update(dt);
            Knight* player = Engine::GetInstance()->GetPlayer();
            if (player && CollisionHandler::GetInstance()->CheckCollision(
                (*it)->collider->Get(), player->GetCollider()->Get())) {
                // In thÃªm thÃ´ng tin Ä‘á»ƒ debug
                std::cout << "Bullet position: (" << (*it)->position.X << ", " << (*it)->position.Y << "), "
                          << "Bullet collider: (" << (*it)->collider->Get().x << ", " << (*it)->collider->Get().y << "), "
                          << "Knight position: (" << player->GetOrigin()->X << ", " << player->GetOrigin()->Y << "), "
                          << "Knight collider: (" << player->GetCollider()->Get().x << ", " << player->GetCollider()->Get().y << ")\n";
                (*it)->active = false;
                player->TakeDamage(1);
            }
            ++it;
        } else {
            delete *it;
            it = m_Bullets.erase(it);
        }
    }
}

void Enemy::Update(float dt) {
    dt = Timer::GetInstance()->GetDeltaTime();

    if (m_IsDead) {
        m_DeathTime -= dt;
        if (m_DeathTime <= 0) return;
        AnimationState();
        m_Animation->Update();
        return;
    }

    m_Origin->X = m_Transform->X + m_Width / 2;
    m_Origin->Y = m_Transform->Y + m_Height / 2;

    Knight* player = Engine::GetInstance()->GetPlayer();
    m_IsAttacking = false;

    if (player) {
        float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                             pow(player->GetOrigin()->Y - m_Origin->Y, 2));
        m_IsAttacking = (distance < m_DetectionRange);

        if (!m_IsAttacking) {
            float distanceFromOrigin = abs(m_Transform->X - m_OriginPoint.X);
            if (distanceFromOrigin >= m_FlyingRange) {
                m_MovingRight = !m_MovingRight;
            }
            float speed = m_FlyingSpeed * dt;
            m_Transform->X += m_MovingRight ? speed : -speed;
            m_Flip = m_MovingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        } else {
            if (player->GetOrigin()->X < m_Origin->X) {
                m_Flip = SDL_FLIP_NONE;
            } else {
                m_Flip = SDL_FLIP_HORIZONTAL;
            }
            if (m_ShootCooldown <= 0) {
                Shoot();
                m_ShootCooldown = 1.0f;
            }
        }

        m_Collider->Set(m_Transform->X, m_Transform->Y, 35, 45);
        UpdateBullets(dt);
        if (m_ShootCooldown > 0) m_ShootCooldown -= dt;

        AnimationState();
        m_Animation->Update();
    }
}

void Enemy::Shoot() {
    Knight* player = Engine::GetInstance()->GetPlayer();
    if (!player) return;
    Vector2D direction(
        player->GetOrigin()->X - m_Origin->X,
        player->GetOrigin()->Y - m_Origin->Y
    );
    float magnitude = sqrt(direction.X * direction.X + direction.Y * direction.Y);
    if (magnitude > 0) {
        direction.X /= magnitude;
        direction.Y /= magnitude;
    }

    float bulletSpeed = 150.0f;
    Vector2D bulletVelocity = direction * bulletSpeed;

    float bulletStartX = m_Origin->X;
    float bulletStartY = m_Origin->Y;

    if (m_Flip == SDL_FLIP_NONE) {
        bulletStartX -= m_Width / 2;
    } else {
        bulletStartX += m_Width / 2;
    }

    Bullet* bullet = new Bullet(
        bulletStartX,
        bulletStartY,
        bulletVelocity,
        "bullet"
    );
    m_Bullets.push_back(bullet);
}

void Enemy::TakeDamage(int damage) {
    if (!m_IsDead) {
        m_Health -= damage;
        if (m_Health <= 0) {
            m_IsDead = true;
            m_DeathTime = DEAD_TIME;
        }
    }
}

void Enemy::AnimationState() {
    if (m_IsDead) {
        m_Animation->SetProps("enemy_death", 0, 7, 100);
    } else if (m_IsAttacking) {
        m_Animation->SetProps("enemy_attack", 0, 8, 100);
    } else {
        m_Animation->SetProps("enemy_fly", 0, 4, 100);
    }
}

void Enemy::Clean() {
    TextureManager::GetInstance()->Drop(m_TextureID);
    for (auto bullet : m_Bullets) {
        delete bullet;
    }
    m_Bullets.clear();
    delete m_Collider;
    delete m_Animation;
}
