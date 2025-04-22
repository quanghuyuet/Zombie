#include "Enemy.h"

Enemy::Enemy(Properties* props) : Character(props) {
    m_Collider = new Collider();
    m_Animation = new Animation();
    m_Animation->SetProps(m_TextureID, 1, 4, 100);
    m_ShootCooldown = 0;
    m_IsDead = false;
    m_DeathTime = 0;
    m_Health = 3;
    m_MaxHealth = 3;
    m_DetectionRange = 300.0f;
    m_FlyingSpeed = 120.0f;
    m_FlyingRange = 400.0f;
    m_OriginPoint = Vector2D(props->X, props->Y);
    m_MovingRight = true;
    m_IsAttacking = false;
    m_Transform->X = props->X;
    m_Transform->Y = props->Y;
    m_Collider->Set(m_Transform->X, m_Transform->Y, 35, 45);

}

void Enemy::Draw() {
    if (!m_IsDead || m_DeathTime > 0) {
        m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height, m_Flip);
        SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
        Vector2D cameraPos = Camera::GetInstance()->GetPosition();

        // Tọa độ màn hình
        int screenX = static_cast<int>(m_Transform->X - cameraPos.X);
        int screenY = static_cast<int>(m_Transform->Y - cameraPos.Y);

        // Thanh máu
        const int barWidth = 70;
        const int barHeight = 10;
        const int padding = 10; // Khoảng cách phía trên đầu enemy

        int barX = screenX + (m_Width - barWidth) / 2; // Căn giữa
        int barY = screenY - barHeight - padding; // Phía trên đầu
        float hpRatio = max(0.0f, min(1.0f, static_cast<float>(m_Health) / m_MaxHealth));
        SDL_Rect background = {barX, barY, barWidth, barHeight};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &background);
        SDL_Rect healthBar = {barX, barY, static_cast<int>(barWidth * hpRatio), barHeight};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &healthBar);
    }

    for (auto bullet : m_Bullets) {
        if (bullet->active) {
            bullet->Draw();
        }
    }
}

void Enemy::UpdateBullets(float dt) {
    for (auto it = m_Bullets.begin(); it != m_Bullets.end();) {
        if ((*it)->active) {
            (*it)->Update(dt); // Cập nhật vị trí và collider
            Knight* player = Engine::GetInstance()->GetPlayer();
            if (player && CollisionHandler::GetInstance()->CheckCollision(
                (*it)->collider->Get(), player->GetCollider()->Get())) {
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
    // Không ghi đè dt, sử dụng giá trị truyền vào
    // dt = Timer::GetInstance()->GetDeltaTime();

    if (m_IsDead) {
        m_DeathTime -= dt;
        if (m_DeathTime <= 0) return;
        AnimationState();
        m_Animation->Update();
        return;
    }

    // Cập nhật gốc (center) của enemy
    m_Origin->X = m_Transform->X + m_Width / 2;
    m_Origin->Y = m_Transform->Y + m_Height / 2;

    Knight* player = Engine::GetInstance()->GetPlayer();
    m_IsAttacking = false;

    // Kiểm tra tấn công nếu player tồn tại
    if (player) {
        float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                             pow(player->GetOrigin()->Y - m_Origin->Y, 2));
        m_IsAttacking = (distance < m_DetectionRange);
    }
    // Di chuyển qua lại nếu không tấn công
    if (!m_IsAttacking) {
        float distanceFromOrigin = abs(m_Transform->X - m_OriginPoint.X);
        if (distanceFromOrigin >= m_FlyingRange) {
            m_MovingRight = !m_MovingRight;
        }
        float speed = m_FlyingSpeed * dt;
        m_Transform->X += m_MovingRight ? speed : -speed;
        m_Flip = m_MovingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    } else if (player) {
        // Chỉ cập nhật flip và bắn nếu player tồn tại
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

    // Cập nhật collider và đạn
    m_Collider->Set(m_Transform->X, m_Transform->Y, 35, 45);
    UpdateBullets(dt);
    if (m_ShootCooldown > 0) m_ShootCooldown -= dt;

    AnimationState();
    m_Animation->Update();
}

void Enemy::Shoot() {
    Knight* player = Engine::GetInstance()->GetPlayer();
    if (!player) return;

    SDL_Rect knightCollider = player->GetCollider()->Get();
    Vector2D knightCenter(
        knightCollider.x + knightCollider.w / 2.0f,
        knightCollider.y + knightCollider.h / 2.0f
    );

    Vector2D direction(
        knightCenter.X - m_Origin->X,
        knightCenter.Y - m_Origin->Y
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
        bulletStartX -= 10.0f;
    } else {
        bulletStartX += 10.0f;
    }

    Bullet* bullet = new Bullet(bulletStartX, bulletStartY, bulletVelocity, "bullet");
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
