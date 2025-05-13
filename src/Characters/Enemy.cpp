#include "Enemy.h"

// Hàm khởi tạo Enemy, kế thừa từ Character
Enemy::Enemy(Properties* props) : Character(props) {
    // Tạo đối tượng Collider để quản lý vùng va chạm
    m_Collider = new Collider();
    // Tạo đối tượng Animation để quản lý hoạt ảnh
    m_Animation = new Animation();
    // Thiết lập hoạt ảnh ban đầu: texture ID, hàng 1, 4 frame, mỗi frame 100ms
    m_Animation->SetProps(m_TextureID, 1, 4, 100);
    // Khởi tạo thời gian hồi chiêu bắn đạn là 0
    m_ShootCooldown = 0;
    // Khởi tạo trạng thái chưa chết
    m_IsDead = false;
    // Khởi tạo thời gian hiển thị hoạt ảnh chết là 0
    m_DeathTime = 0;
    // Thiết lập máu ban đầu là 3
    m_Health = 3;
    // Thiết lập máu tối đa là 3
    m_MaxHealth = 3;
    // Thiết lập phạm vi phát hiện người chơi là 300 pixel
    m_DetectionRange = 300.0f;
    // Thiết lập tốc độ di chuyển là 120 pixel/giây
    m_FlyingSpeed = 120.0f;
    // Thiết lập phạm vi di chuyển qua lại là 400 pixel
    m_FlyingRange = 400.0f;
    // Lưu vị trí gốc của kẻ thù từ props
    m_OriginPoint = Vector2D(props->X, props->Y);
    // Thiết lập hướng di chuyển ban đầu là sang phải
    m_MovingRight = true;
    // Khởi tạo trạng thái không tấn công
    m_IsAttacking = false;
    // Đặt vị trí X của kẻ thù từ props
    m_Transform->X = props->X;
    // Đặt vị trí Y của kẻ thù từ props
    m_Transform->Y = props->Y;
    // Thiết lập vùng va chạm: vị trí (X, Y), rộng 35, cao 45
    m_Collider->Set(m_Transform->X, m_Transform->Y, 35, 45);
}

// Hàm vẽ kẻ thù và thanh máu lên màn hình
void Enemy::Draw() {
    // Chỉ vẽ nếu kẻ thù chưa chết hoặc đang trong thời gian hiển thị hoạt ảnh chết
    if (!m_IsDead || m_DeathTime > 0) {
        // Vẽ hoạt ảnh của kẻ thù tại vị trí (X, Y) với kích thước và hướng lật
        m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height, m_Flip);
        // Lấy renderer từ Engine để vẽ
        SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
        // Lấy vị trí camera để chuyển đổi tọa độ
        Vector2D cameraPos = Camera::GetInstance()->GetPosition();

        // Tính tọa độ màn hình của kẻ thù
        int screenX = static_cast<int>(m_Transform->X - cameraPos.X);
        int screenY = static_cast<int>(m_Transform->Y - cameraPos.Y);

        // Định nghĩa kích thước và khoảng cách của thanh máu
        const int barWidth = 70;  // Chiều rộng thanh máu
        const int barHeight = 10; // Chiều cao thanh máu
        const int padding = 10;   // Khoảng cách phía trên đầu kẻ thù

        // Tính vị trí thanh máu
        int barX = screenX + (m_Width - barWidth) / 2;
        int barY = screenY - barHeight - padding;
        // Tính tỷ lệ
        float hpRatio = max(0.0f, min(1.0f, static_cast<float>(m_Health) / m_MaxHealth));
        // Vẽ nền
        SDL_Rect background = {barX, barY, barWidth, barHeight};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &background);
        // Vẽ thanh
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

// Hàm cập nhật trạng thái của các viên đạn
void Enemy::UpdateBullets(float dt) {
    // Duyệt qua danh sách đạn
    for (auto it = m_Bullets.begin(); it != m_Bullets.end();) {
        if ((*it)->active) {
            // Cập nhật vị trí và collider của đạn
            (*it)->Update(dt);
            // Lấy con trỏ đến người chơi
            Knight* player = Engine::GetInstance()->GetPlayer();
            // Kiểm tra va chạm giữa đạn và người chơi
            if (player && CollisionHandler::GetInstance()->CheckCollision(
                (*it)->collider->Get(), player->GetCollider()->Get())) {
                // Nếu va chạm, vô hiệu hóa đạn và gây sát thương cho người chơi
                (*it)->active = false;
                player->TakeDamage(1);
            }
            // Tăng iterator nếu đạn vẫn hoạt động
            ++it;
        } else {
            // Xóa và giải phóng bộ nhớ của đạn không hoạt động
            delete *it;
            it = m_Bullets.erase(it);
        }
    }
}

// Hàm cập nhật trạng thái của kẻ thù mỗi frame
void Enemy::Update(float dt) {
    // Nếu kẻ thù đã chết
    if (m_IsDead) {
        // Giảm thời gian hiển thị hoạt ảnh chết
        m_DeathTime -= dt;
        // Thoát hàm nếu hết thời gian
        if (m_DeathTime <= 0) return;
        // Cập nhật hoạt ảnh chết
        AnimationState();
        m_Animation->Update();
        return;
    }

    // Cập nhật vị trí trung tâm của kẻ thù
    m_Origin->X = m_Transform->X + m_Width / 2;
    m_Origin->Y = m_Transform->Y + m_Height / 2;

    // Lấy con trỏ đến người chơi và đặt trạng thái không tấn công
    Knight* player = Engine::GetInstance()->GetPlayer();
    m_IsAttacking = false;

    // Kiểm tra khoảng cách đến người chơi để quyết định tấn công
    if (player) {
        float distance = sqrt(pow(player->GetOrigin()->X - m_Origin->X, 2) +
                             pow(player->GetOrigin()->Y - m_Origin->Y, 2));
        m_IsAttacking = (distance < m_DetectionRange);
    }

    // Nếu không tấn công, di chuyển qua lại
    if (!m_IsAttacking) {
        // Tính khoảng cách từ vị trí hiện tại đến điểm gốc
        float distanceFromOrigin = abs(m_Transform->X - m_OriginPoint.X);
        // Đảo hướng nếu vượt quá phạm vi di chuyển
        if (distanceFromOrigin >= m_FlyingRange) {
            m_MovingRight = !m_MovingRight;
        }
        // Tính tốc độ di chuyển dựa trên thời gian
        float speed = m_FlyingSpeed * dt;
        // Cập nhật vị trí X: sang phải hoặc trái
        m_Transform->X += m_MovingRight ? speed : -speed;
        // Cập nhật hướng sprite: lật ngang nếu sang phải
        m_Flip = m_MovingRight ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    } else if (player) {
        // Nếu tấn công, hướng sprite theo vị trí người chơi
        if (player->GetOrigin()->X < m_Origin->X) {
            m_Flip = SDL_FLIP_NONE; // Người chơi ở bên trái
        } else {
            m_Flip = SDL_FLIP_HORIZONTAL; // Người chơi ở bên phải
        }
        // Bắn đạn nếu hết thời gian hồi chiêu
        if (m_ShootCooldown <= 0) {
            Shoot();
            m_ShootCooldown = 1.0f; // Đặt lại hồi chiêu 1 giây
        }
    }

    // Cập nhật vùng va chạm của kẻ thù
    m_Collider->Set(m_Transform->X, m_Transform->Y, 35, 45);
    // Cập nhật trạng thái các viên đạn
    UpdateBullets(dt);
    // Giảm thời gian hồi chiêu bắn
    if (m_ShootCooldown > 0) m_ShootCooldown -= dt;

    // Cập nhật trạng thái hoạt ảnh
    AnimationState();
    m_Animation->Update();
}

// Hàm xử lý việc bắn đạn
void Enemy::Shoot() {
    // Lấy con trỏ đến người chơi, thoát nếu không có
    Knight* player = Engine::GetInstance()->GetPlayer();
    if (!player) return;
    // Lấy vùng va chạm của người chơi và tính trung tâm
    SDL_Rect knightCollider = player->GetCollider()->Get();
    Vector2D knightCenter(
        knightCollider.x + knightCollider.w / 2.0f,
        knightCollider.y + knightCollider.h / 2.0f
    );

    // Tính vector hướng từ kẻ thù đến người chơi
    Vector2D direction(
        knightCenter.X - m_Origin->X,
        knightCenter.Y - m_Origin->Y
    );

    // Chuẩn hóa vector hướng (độ dài = 1)
    float magnitude = sqrt(direction.X * direction.X + direction.Y * direction.Y);
    if (magnitude > 0) {
        direction.X /= magnitude;
        direction.Y /= magnitude;
    }

    // Tính vận tốc đạn: hướng * tốc độ (150 pixel/giây)
    float bulletSpeed = 150.0f;
    Vector2D bulletVelocity = direction * bulletSpeed;

    // Đặt vị trí xuất phát của đạn tại trung tâm kẻ thù
    float bulletStartX = m_Origin->X;
    float bulletStartY = m_Origin->Y;

    // Điều chỉnh vị trí xuất phát theo hướng sprite
    if (m_Flip == SDL_FLIP_NONE) {
        bulletStartX -= 10.0f; // Lệch trái nếu hướng trái
    } else {
        bulletStartX += 10.0f; // Lệch phải nếu hướng phải
    }

    // Tạo viên đạn mới và thêm vào danh sách
    Bullet* bullet = new Bullet(bulletStartX, bulletStartY, bulletVelocity, "bullet");
    m_Bullets.push_back(bullet);
}

// Hàm xử lý khi kẻ thù nhận sát thương
void Enemy::TakeDamage(int damage) {
    // Chỉ xử lý nếu kẻ thù chưa chết
    if (!m_IsDead) {
        // Giảm máu
        m_Health -= damage;
        // Nếu máu <= 0, chuyển sang trạng thái chết
        if (m_Health <= 0) {
            m_IsDead = true;
            m_DeathTime = DEAD_TIME; // Đặt thời gian hiển thị hoạt ảnh chết
        }
    }
}

// Hàm chọn hoạt ảnh dựa trên trạng thái
void Enemy::AnimationState() {
    if (m_IsDead) {
        // Hoạt ảnh chết: 7 frame, mỗi frame 100ms
        m_Animation->SetProps("enemy_death", 0, 7, 100);
    } else if (m_IsAttacking) {
        // Hoạt ảnh tấn công: 8 frame, mỗi frame 100ms
        m_Animation->SetProps("enemy_attack", 0, 8, 100);
    } else {
        // Hoạt ảnh bay: 4 frame, mỗi frame 100ms
        m_Animation->SetProps("enemy_fly", 0, 4, 100);
    }
}

// Hàm dọn dẹp tài nguyên
void Enemy::Clean() {
    // Xóa texture của kẻ thù
    TextureManager::GetInstance()->Drop(m_TextureID);
    // Giải phóng bộ nhớ của tất cả các viên đạn
    for (auto bullet : m_Bullets) {
        delete bullet;
    }
    // Xóa danh sách đạn
    m_Bullets.clear();
    // Giải phóng bộ nhớ của Collider
    delete m_Collider;
    // Giải phóng bộ nhớ của Animation
    delete m_Animation;
}
