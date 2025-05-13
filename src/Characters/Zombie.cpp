#include "Zombie.h"
#include "Engine.h"
#include "CollisionHandler.h"
#include "Timer.h"
#include "Camera.h"

Zombie::Zombie(Properties* props) : Character(props) {
    // Khởi tạo RigidBody để xử lý vật lý (di chuyển, trọng lực)
    m_RigidBody = new RigidBody();
    m_RigidBody->SetGravity(100.0f); // Đặt trọng lực cho zombie

    // Khởi tạo Collider để xử lý va chạm
    m_Collider = new Collider();
    m_Collider->SetBuffer(0, 18, 0, 0); // Đặt vùng đệm cho collider
    // Khởi tạo Animation để quản lý các trạng thái hoạt hình
    m_Animation = new Animation();

    // Thiết lập các thuộc tính cơ bản của zombie
    m_WalkingSpeed = 80.0f; // Tốc độ di chuyển ngang
    m_DetectionRange = 150.0f; // Phạm vi phát hiện người chơi
    m_AttackRange = 50.0f; // Phạm vi tấn công
    m_AttackCooldown = 1.5f; // Thời gian hồi chiêu giữa các lần tấn công
    m_Health = 3; // Máu hiện tại
    m_MaxHealth = 3; // Máu tối đa
    m_IsDead = false; // Trạng thái sống/chết
    m_DeathTime = 0.0f; // Thời gian chờ trước khi zombie biến mất sau khi chết
    m_MovingRight = true; // Hướng di chuyển ban đầu (phải)
    m_IsAttacking = false; // Trạng thái tấn công
    m_IsGrounded = false; // Trạng thái chạm đất
    m_OriginPoint = Vector2D(m_Transform->X, m_Transform->Y); // Điểm gốc ban đầu

    // Đặt vị trí ban đầu của zombie từ props
    m_Transform->X = props->X;
    m_Transform->Y = props->Y;

    // Lưu vị trí an toàn ban đầu để xử lý va chạm
    m_LastSafePosition.X = m_Transform->X;
    m_LastSafePosition.Y = m_Transform->Y;
}

void Zombie::TakeDamage(int damage) {
    // Chỉ xử lý sát thương nếu zombie còn sống
    if (!m_IsDead) {
        m_Health -= damage; // Giảm máu theo sát thương nhận được
        if (m_Health <= 0) { // Nếu máu <= 0, zombie chết
            m_IsDead = true;
            m_DeathTime = 0.7f; // Đặt thời gian chờ để phát hoạt hình chết
            m_RigidBody->SetVelocityX(0); // Dừng di chuyển ngang
            m_RigidBody->SetVelocityY(0); // Dừng di chuyển dọc
            AnimationState(); // Cập nhật trạng thái hoạt hình (chết)
            // Phát âm thanh khi zombie chết
            Mix_Chunk* deathSound = Engine::GetInstance()->GetZombieDeathSound();
            if (deathSound) {
                Mix_PlayChannel(-1, deathSound, 0); // Phát âm thanh trên kênh bất kỳ
            }
        }
    }
}

void Zombie::Update(float dt) {
    // Nếu zombie đã chết, chỉ cập nhật hoạt hình chết
    if (m_IsDead) {
        m_DeathTime -= dt; // Giảm thời gian chờ
        if (m_DeathTime > 0) { // Nếu vẫn còn thời gian, tiếp tục phát hoạt hình
            AnimationState();
            m_Animation->Update();
        }
        return; // Thoát hàm, không xử lý gì thêm
    }

    // Lấy thông tin người chơi từ Engine
    Knight* player = Engine::GetInstance()->GetPlayer();
    m_IsAttacking = false; // Đặt lại trạng thái tấn công

    // Cập nhật vận tốc từ RigidBody
    m_RigidBody->Update(dt);
    Vector2D velocity = m_RigidBody->Velocity(); // Lấy vận tốc hiện tại

    // Xử lý di chuyển theo trục Y (rơi do trọng lực)
    float newY = m_Transform->Y + velocity.Y * dt; // Tính vị trí Y mới
    m_Collider->Set(m_Transform->X, newY, m_Width, m_Height); // Cập nhật collider

    // Kiểm tra va chạm với bản đồ
    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
        if (velocity.Y > 0) { // Nếu đang rơi và va chạm
            m_Transform->Y = m_LastSafePosition.Y; // Giữ vị trí Y an toàn
            m_RigidBody->SetVelocityY(0); // Dừng rơi
            m_IsGrounded = true; // Đặt trạng thái chạm đất
        }
    } else {
        m_Transform->Y = newY; // Cập nhật vị trí Y mới
        m_LastSafePosition.Y = m_Transform->Y; // Lưu vị trí an toàn
        m_IsGrounded = false; // Không chạm đất
    }

    // Xử lý di chuyển và tấn công khi chạm đất
    if (m_IsGrounded) {
        if (player) { // Nếu có người chơi
            // Tính khoảng cách từ zombie đến người chơi
            float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                                 pow(player->GetOrigin()->Y - m_Origin->Y, 2));

            // Nếu người chơi trong tầm phát hiện
            if (distance < m_DetectionRange) {
                // Nếu trong tầm tấn công, dừng di chuyển và tấn công
                if (distance < m_AttackRange) {
                    m_IsAttacking = true;
                    m_RigidBody->SetVelocityX(0); // Dừng di chuyển ngang

                    // Xoay zombie về phía người chơi
                    if (player->GetOrigin()->X < m_Origin->X) {
                        m_Flip = SDL_FLIP_HORIZONTAL; // Lật hình sang trái
                    } else {
                        m_Flip = SDL_FLIP_NONE; // Không lật
                    }

                    // Kiểm tra thời gian hồi chiêu tấn công
                    if (m_AttackCooldown <= 0) {
                        Attack(player); // Tấn công người chơi
                        m_AttackCooldown = 1.5f; // Đặt lại thời gian hồi chiêu
                    }
                }
                // Nếu ngoài tầm tấn công nhưng trong tầm phát hiện, đuổi theo người chơi
                else {
                    m_MovingRight = (player->GetOrigin()->X > m_Origin->X); // Xác định hướng
                    float speed = m_WalkingSpeed * dt; // Tính khoảng cách di chuyển
                    float newX = m_Transform->X + (m_MovingRight ? speed : -speed); // Tính vị trí X mới
                    m_Collider->Set(newX, m_Transform->Y, m_Width, m_Height); // Cập nhật collider

                    // Kiểm tra va chạm với bản đồ
                    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
                        m_Transform->X = m_LastSafePosition.X; // Giữ vị trí X an toàn
                        m_MovingRight = !m_MovingRight; // Đổi hướng
                    } else {
                        m_Transform->X = newX; // Cập nhật vị trí X mới
                        m_LastSafePosition.X = m_Transform->X; // Lưu vị trí an toàn
                    }

                    // Cập nhật hướng lật hình
                    m_Flip = m_MovingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
                }
            }
            // Nếu người chơi ngoài tầm phát hiện, di chuyển qua lại bình thường
            else {
                float speed = m_WalkingSpeed * dt; // Tính khoảng cách di chuyển
                float newX = m_Transform->X + (m_MovingRight ? speed : -speed); // Tính vị trí X mới
                m_Collider->Set(newX, m_Transform->Y, m_Width, m_Height); // Cập nhật collider

                // Kiểm tra va chạm với bản đồ
                if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) {
                    m_Transform->X = m_LastSafePosition.X; // Giữ vị trí X an toàn
                    m_MovingRight = !m_MovingRight; // Đổi hướng
                } else {
                    m_Transform->X = newX; // Cập nhật vị trí X mới
                    m_LastSafePosition.X = m_Transform->X; // Lưu vị trí an toàn
                }

                // Cập nhật hướng lật hình
                m_Flip = m_MovingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
            }
        }

        // Giảm thời gian hồi chiêu tấn công
        if (m_AttackCooldown > 0) m_AttackCooldown -= dt;
    }

    // Cập nhật điểm gốc (trung tâm của zombie)
    m_Origin->X = m_Transform->X + m_Width / 2;
    m_Origin->Y = m_Transform->Y + m_Height / 2;

    // Cập nhật trạng thái hoạt hình và frame
    AnimationState();
    m_Animation->Update();
}

void Zombie::Draw() {
    // Chỉ vẽ nếu zombie chưa chết hoặc đang trong giai đoạn hoạt hình chết
    if (!m_IsDead || m_DeathTime > 0) {
        // Vẽ hoạt hình của zombie
        m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height, m_Flip);

        // Lấy renderer và vị trí camera
        SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
        Vector2D cameraPos = Camera::GetInstance()->GetPosition();
        float cameraX = cameraPos.X;
        float cameraY = cameraPos.Y;

        // Tính toán vị trí thanh máu trên màn hình
        int screenX = static_cast<int>(m_Transform->X - cameraX);
        int screenY = static_cast<int>(m_Transform->Y - cameraY - 60);
        int barWidth = 70; // Chiều rộng thanh máu
        int barHeight = 8; // Chiều cao thanh máu
        int padding = -70; // Khoảng cách từ zombie đến thanh máu

        int barX = screenX + (m_Width - barWidth) / 2; // Vị trí X của thanh máu
        int barY = screenY - barHeight - padding; // Vị trí Y của thanh máu

        // Tính tỷ lệ máu
        float hpRatio = (float)m_Health / m_MaxHealth;
        if (hpRatio < 0) hpRatio = 0; // Đảm bảo tỷ lệ không âm
        if (hpRatio > 1) hpRatio = 1; // Đảm bảo tỷ lệ không vượt quá 1

        // Vẽ nền thanh máu
        SDL_Rect background = {barX, barY, barWidth, barHeight};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Màu xám
        SDL_RenderFillRect(renderer, &background);

        // Vẽ thanh máu
        SDL_Rect healthBar = {barX, barY, static_cast<int>(barWidth * hpRatio), barHeight};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
        SDL_RenderFillRect(renderer, &healthBar);
        /*
        // Vẽ hộp bao quanh (collider) của zombie để debug
        SDL_Rect colliderBox = m_Collider->Get(); // Lấy hộp bao quanh từ collider
        SDL_Rect screenBox = {
            static_cast<int>(colliderBox.x - cameraX),
            static_cast<int>(colliderBox.y - cameraY),
            colliderBox.w,
            colliderBox.h
        };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Màu xanh lá
        SDL_RenderDrawRect(renderer, &screenBox); // Vẽ đường viền hộp
        */
    }
}

void Zombie::Attack(Knight* player) {
    if (player) {
        // Tính khoảng cách từ zombie đến người chơi
        float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                             pow(player->GetOrigin()->Y - m_Origin->Y, 2));
        if (distance < 40.0f) {
            player->TakeDamage(1);
        }
    }
}

void Zombie::AnimationState() {
    if (m_IsDead) {
        // Hoạt hình khi chết
        m_Animation->SetProps("zombie_die", 0, 15, 120);
    } else if (m_IsAttacking) {
        // Hoạt hình khi tấn công
        m_Animation->SetProps("zombie_chem", 0, 12, 100);
    } else {
        // Hoạt hình khi di chuyển
        m_Animation->SetProps("zombie_dibo", 0, 12, 100);
    }
}
