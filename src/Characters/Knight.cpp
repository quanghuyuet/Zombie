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

// Hàm khởi tạo hiệp sĩ với các thuộc tính ban đầu
Knight::Knight(Properties* props) : Character(props) {
    // Thiết lập thời gian và lực nhảy từ hằng số
    m_JumpTime = JUMP_TIME;    // Thời gian tối đa để nhảy (15.0f)
    m_JumpForce = JUMP_FORCE;  // Lực nhảy (10.0f)
    m_AtrackTime = 0.0f;       // Thời gian tấn công ban đầu là 0
    m_IsAtracking = false;     // Không tấn công ban đầu
    m_IsJumping = false;       // Không nhảy ban đầu
    m_IsGrounded = false;      // Không trên mặt đất ban đầu
    m_IsRunning = false;       // Không chạy ban đầu
    m_IsCrouching = false;     // Không ngồi ban đầu
    m_WasRunning = false;      // Trạng thái chạy trước đó là không chạy

    // Tạo thành phần va chạm và thiết lập vùng va chạm nhỏ hơn sprite
    m_Collider = new Collider();
    m_Collider->SetBuffer(-53, -23, 0, 0); // Điều chỉnh vùng va chạm (-53px bên trái, -23px phía trên)

    // Tạo thành phần vật lý và đặt trọng lực
    m_RigidBody = new RigidBody();
    m_RigidBody->SetGravity(80.0f); // Trọng lực 80 để hiệp sĩ rơi tự nhiên

    // Tạo thành phần hoạt hình với trạng thái mặc định (đứng yên)
    m_Animation = new Animation();
    m_Animation->SetProps(m_TextureID, 1, 6, 100); // ID texture, hàng 1, 6 frame, tốc độ 100ms

    // Tạo điểm trung tâm của hiệp sĩ
    m_Origin = new Point();

    // Thiết lập máu và mana
    m_Health = 70;        // Máu hiện tại
    m_MaxHealth = 70;     // Máu tối đa
    m_Mana = 5;           // Mana hiện tại
    m_MaxMana = 5;        // Mana tối đa
    m_ManaRegenTime = 0.0f; // Thời gian chờ hồi mana ban đầu
    m_ManaRegenRate = 3.0f; // Hồi 1 mana sau mỗi 3 giây

    // Tải các hiệu ứng âm thanh
    m_RunSound = Mix_LoadWAV("sound/knight_run.wav");    // Âm thanh chạy
    m_AttackSound = Mix_LoadWAV("sound/knight_chem.wav"); // Âm thanh tấn công
    m_JumpSound = Mix_LoadWAV("sound/knight_jump.wav");  // Âm thanh nhảy

    // Đặt âm lượng cho các âm thanh (0-128)
    if (m_RunSound) Mix_VolumeChunk(m_RunSound, 110);    // Âm lượng chạy: 110/128 (~86%)
    if (m_AttackSound) Mix_VolumeChunk(m_AttackSound, 20); // Âm lượng tấn công: 20/128 (~16%)
    if (m_JumpSound) Mix_VolumeChunk(m_JumpSound, 100);  // Âm lượng nhảy: 100/128 (~78%)
}

// Hàm giảm máu và mana khi hiệp sĩ nhận sát thương
void Knight::TakeDamage(int damage) {
    m_Health -= damage; // Giảm máu theo lượng sát thương
    if (m_Health <= 0) {
        m_Health = 0; // Nếu máu nhỏ hơn 0, đặt về 0 (hiệp sĩ chết)
    }
    m_Mana -= 1; // Giảm 1 mana
    if (m_Mana < 0) {
        m_Mana = 0; // Nếu mana nhỏ hơn 0, đặt về 0
    }
}

// Hàm vẽ hiệp sĩ và thanh máu/mana lên màn hình
void Knight::Draw() {
    // Vẽ sprite hiệp sĩ tại vị trí hiện tại với hướng và kích thước
    m_Animation->Draw(m_Transform->X, m_Transform->Y, m_Width, m_Height, m_Flip);

    // Lấy renderer và font từ Engine để vẽ giao diện
    SDL_Renderer* renderer = Engine::GetInstance()->GetRenderer();
    TTF_Font* font = Engine::GetInstance()->GetFont();

    // Thiết lập kích thước và vị trí thanh máu/mana
    int barWidth = 200; // Chiều rộng thanh: 200px
    int barHeight = 20; // Chiều cao thanh: 20px
    int hpX = 47;       // Vị trí X của thanh máu
    int hpY = 10;       // Vị trí Y của thanh máu
    int mpY = hpY + barHeight + 5; // Vị trí Y của thanh mana (dưới thanh máu)

    // Tính tỷ lệ máu và mana
    float hpRatio = (float)m_Health / m_MaxHealth; // Tỷ lệ máu hiện tại/tối đa
    float mpRatio = (float)m_Mana / m_MaxMana;     // Tỷ lệ mana hiện tại/tối đa

    // Vẽ chữ "HP" màu đỏ
    SDL_Color textColor1 = {255, 0, 0, 255}; // Màu đỏ
    SDL_Surface* hpSurface = TTF_RenderText_Solid(font, "HP", textColor1);
    SDL_Texture* hpTexture = SDL_CreateTextureFromSurface(renderer, hpSurface);
    SDL_Rect hpTextRect = {11, hpY, hpSurface->w, hpSurface->h}; // Vị trí chữ HP
    SDL_RenderCopy(renderer, hpTexture, nullptr, &hpTextRect);
    SDL_FreeSurface(hpSurface);
    SDL_DestroyTexture(hpTexture);

    // Vẽ thanh máu: nền xám, thanh đỏ theo tỷ lệ máu
    SDL_Rect hpBackground = {hpX, hpY, barWidth, barHeight}; // Nền thanh máu
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Màu xám
    SDL_RenderFillRect(renderer, &hpBackground);
    SDL_Rect hpBar = {hpX, hpY, (int)(barWidth * hpRatio), barHeight}; // Thanh máu
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ
    SDL_RenderFillRect(renderer, &hpBar);

    // Vẽ chữ "MP" màu xanh
    SDL_Color textColor2 = {0, 0, 255, 255}; // Màu xanh
    SDL_Surface* mpSurface = TTF_RenderText_Solid(font, "MP", textColor2);
    SDL_Texture* mpTexture = SDL_CreateTextureFromSurface(renderer, mpSurface);
    SDL_Rect mpTextRect = {10, mpY, mpSurface->w, mpSurface->h}; // Vị trí chữ MP
    SDL_RenderCopy(renderer, mpTexture, nullptr, &mpTextRect);
    SDL_FreeSurface(mpSurface);
    SDL_DestroyTexture(mpTexture);

    // Vẽ thanh mana: nền xám, thanh xanh theo tỷ lệ mana
    SDL_Rect mpBackground = {hpX, mpY, barWidth, barHeight}; // Nền thanh mana
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // Màu xám
    SDL_RenderFillRect(renderer, &mpBackground);
    SDL_Rect mpBar = {hpX, mpY, (int)(barWidth * mpRatio), barHeight}; // Thanh mana
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Màu xanh
    SDL_RenderFillRect(renderer, &mpBar);
}

// Hàm cập nhật trạng thái hiệp sĩ mỗi khung hình
void Knight::Update(float dt) {
    // Lấy thời gian delta từ Timer
    dt = Timer::GetInstance()->GetDeltaTime();
    m_IsRunning = false; // Đặt lại trạng thái chạy
    m_IsCrouching = false; // Đặt lại trạng thái ngồi
    m_RigidBody->UnSetForce(); // Xóa các lực hiện tại
    m_RigidBody->UnSetFriction(); // Xóa ma sát hiện tại

    // Xử lý di chuyển ngang nếu không tấn công
    if (!m_IsAtracking) {
        int moveDir = Input::GetInstance()->GetAxisKey(HORIZONTAL); // Lấy hướng di chuyển (trái/phải)
        if (moveDir == FORWARD) { // Nhấn phím phải
            m_RigidBody->ApplyForceX(300.0f); // Áp dụng lực sang phải
            m_Flip = SDL_FLIP_NONE; // Hướng sprite sang phải
            m_IsRunning = true; // Đặt trạng thái chạy
        } else if (moveDir == BACKWARD) { // Nhấn phím trái
            m_RigidBody->ApplyForceX(-300.0f); // Áp dụng lực sang trái
            m_Flip = SDL_FLIP_HORIZONTAL; // Hướng sprite sang trái
            m_IsRunning = true; // Đặt trạng thái chạy
        } else { // Không di chuyển
            if (m_RigidBody->Velocity().X > 0) { // Đang di chuyển sang phải
                m_RigidBody->ApplyFriction(Vector2D(-200.0f, 0)); // Áp dụng ma sát để dừng
                if (m_RigidBody->Velocity().X < 5.0f && m_RigidBody->Velocity().X > -5.0f) {
                    m_RigidBody->SetVelocityX(0); // Đặt vận tốc ngang về 0 nếu nhỏ
                }
            } else if (m_RigidBody->Velocity().X < 0) { // Đang di chuyển sang trái
                m_RigidBody->ApplyFriction(Vector2D(200.0f, 0)); // Áp dụng ma sát để dừng
                if (m_RigidBody->Velocity().X < 5.0f && m_RigidBody->Velocity().X > -5.0f) {
                    m_RigidBody->SetVelocityX(0); // Đặt vận tốc ngang về 0 nếu nhỏ
                }
            }
        }
    }

    // Phát âm thanh chạy khi bắt đầu chạy
    if (m_IsRunning && !m_WasRunning && m_RunSound) {
        Mix_PlayChannel(-1, m_RunSound, 0); // Phát âm thanh chạy
    }
    m_WasRunning = m_IsRunning; // Cập nhật trạng thái chạy trước đó

    // Xử lý ngồi khi nhấn phím S và đang trên mặt đất
    if (Input::GetInstance()->GetKeyDown(SDL_SCANCODE_S) && m_IsGrounded) {
        m_IsCrouching = true; // Đặt trạng thái ngồi
    }

    // Xử lý tấn công khi nhấn phím J
    if (Input::GetInstance()->GetKeyDown(SDL_SCANCODE_J) && !m_IsAtracking) {
        m_IsAtracking = true; // Đặt trạng thái tấn công
        m_AtrackTime = 0.5f;  // Thời gian tấn công: 0.5 giây
        if (m_AttackSound) Mix_PlayChannel(-1, m_AttackSound, 0); // Phát âm thanh tấn công

        // Tạo vùng va chạm tấn công
        SDL_Rect knightAttackBox = m_Collider->Get();
        knightAttackBox.w += 20; // Mở rộng vùng va chạm 20px
        if (m_Flip == SDL_FLIP_HORIZONTAL) { // Nếu hướng trái
            knightAttackBox.x -= 20; // Dịch vùng va chạm sang trái
        }

        // Kiểm tra va chạm với kẻ thù
        for (auto enemy : Engine::GetInstance()->GetEnemies()) {
            if (enemy != nullptr && CollisionHandler::GetInstance()->CheckCollision(knightAttackBox, enemy->GetCollider()->Get())) {
                enemy->TakeDamage(2); // Gây 2 sát thương cho kẻ thù
            }
        }
        // Kiểm tra va chạm với zombie
        for (auto zombie : Engine::GetInstance()->GetZombies()) {
            if (zombie != nullptr && CollisionHandler::GetInstance()->CheckCollision(knightAttackBox, zombie->GetCollider()->Get())) {
                zombie->TakeDamage(1); // Gây 1 sát thương cho zombie
            }
        }
        // Kiểm tra va chạm với boss
        Boss* boss = Engine::GetInstance()->GetBoss();
        if (boss != nullptr && CollisionHandler::GetInstance()->CheckCollision(knightAttackBox, boss->GetCollider()->Get())) {
            boss->TakeDamage(2); // Gây 2 sát thương cho boss
        }
    }

    // Cập nhật thời gian tấn công
    if (m_IsAtracking) {
        m_AtrackTime -= dt; // Giảm thời gian tấn công
        if (m_AtrackTime <= 0) {
            m_IsAtracking = false; // Kết thúc trạng thái tấn công
        }
    }

    // Xử lý nhảy khi nhấn phím nhảy
    if (Input::GetInstance()->GetAxisKey(VERTICAL) == 1 && m_IsGrounded && !m_IsAtracking) {
        m_IsJumping = true; // Đặt trạng thái nhảy
        m_IsGrounded = false; // Không còn trên mặt đất
        m_RigidBody->ApplyForceY(-1500.0f); // Áp dụng lực nhảy lên
        if (m_JumpSound) Mix_PlayChannel(-1, m_JumpSound, 0); // Phát âm thanh nhảy
    }

    // Cập nhật vật lý
    m_RigidBody->Update(dt); // Cập nhật vị trí dựa trên lực và vận tốc

    // Xử lý va chạm ngang
    m_LastSafePosition.X = m_Transform->X; // Lưu vị trí an toàn
    m_Transform->X += m_RigidBody->Velocity().X * dt; // Cập nhật vị trí X
    m_Collider->Set(m_Transform->X, m_Transform->Y, 30, 50); // Cập nhật vùng va chạm
    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) { // Nếu va chạm bản đồ
        m_Transform->X = m_LastSafePosition.X; // Khôi phục vị trí an toàn
        m_RigidBody->SetVelocityX(0); // Đặt vận tốc ngang về 0
    }

    // Xử lý va chạm dọc
    m_LastSafePosition.Y = m_Transform->Y; // Lưu vị trí an toàn
    m_Transform->Y += m_RigidBody->Velocity().Y * dt; // Cập nhật vị trí Y
    m_Collider->Set(m_Transform->X, m_Transform->Y, 30, 50); // Cập nhật vùng va chạm
    if (CollisionHandler::GetInstance()->MapCollision(m_Collider->Get())) { // Nếu va chạm bản đồ
        m_Transform->Y = m_LastSafePosition.Y; // Khôi phục vị trí an toàn
        if (m_RigidBody->Velocity().Y > 0) { // Nếu đang rơi
            m_RigidBody->SetVelocityY(0); // Đặt vận tốc dọc về 0
            m_IsGrounded = true; // Đặt trạng thái trên mặt đất
            m_IsJumping = false; // Không còn nhảy
        } else if (m_RigidBody->Velocity().Y < 0) { // Nếu đang nhảy lên
            m_RigidBody->SetVelocityY(0); // Đặt vận tốc dọc về 0
        }
    } else {
        m_IsGrounded = false; // Không trên mặt đất
    }

    // Hồi mana
    m_ManaRegenTime += dt; // Tăng thời gian chờ hồi mana
    if (m_ManaRegenTime >= m_ManaRegenRate) { // Nếu đủ 3 giây
        m_ManaRegenTime = 0.0f; // Đặt lại thời gian chờ
        if (m_Mana < m_MaxMana) { // Nếu mana chưa đầy
            m_Mana += 1; // Hồi 1 mana
        }
    }

    // Cập nhật điểm trung tâm
    m_Origin->X = m_Transform->X + m_Width / 2; // Tọa độ X trung tâm
    m_Origin->Y = m_Transform->Y + m_Height / 2; // Tọa độ Y trung tâm

    // Cập nhật hoạt hình
    AnimationState(); // Chọn hoạt hình phù hợp
    m_Animation->Update(); // Cập nhật frame hoạt hình
}

// Hàm chọn hoạt hình dựa trên trạng thái
void Knight::AnimationState() {
    // Mặc định là hoạt hình đứng yên
    m_Animation->SetProps("player_idle", 0, 6, 100); // 6 frame, 100ms
    if (m_IsRunning) { // Nếu đang chạy
        m_Animation->SetProps("player_run", 0, 8, 100); // 8 frame, 100ms
    }
    if (m_IsCrouching) { // Nếu đang ngồi
        m_Animation->SetProps("player_crouch", 0, 6, 200); // 6 frame, 200ms
    }
    if (m_IsJumping && m_RigidBody->Velocity().Y < 0) { // Nếu đang nhảy lên
        m_Animation->SetProps("player_jump", 0, 2, 200); // 2 frame, 200ms
    }
    if (!m_IsGrounded && m_RigidBody->Velocity().Y > 0) { // Nếu đang rơi
        m_Animation->SetProps("player_fall", 0, 2, 350); // 2 frame, 350ms
    }
    if (m_IsAtracking) { // Nếu đang tấn công
        m_Animation->SetProps("player_attrack", 0, 14, 80); // 14 frame, 80ms
    }
}

// Hàm giải phóng tài nguyên
void Knight::Clean() {
    TextureManager::GetInstance()->Drop(m_TextureID); // Xóa texture
    delete m_Collider; // Xóa thành phần va chạm
    delete m_RigidBody; // Xóa thành phần vật lý
    delete m_Animation; // Xóa thành phần hoạt hình
    delete m_Origin; // Xóa điểm trung tâm

    // Giải phóng âm thanh
    if (m_RunSound) Mix_FreeChunk(m_RunSound); // Giải phóng âm thanh chạy
    if (m_AttackSound) Mix_FreeChunk(m_AttackSound); // Giải phóng âm thanh tấn công
    if (m_JumpSound) Mix_FreeChunk(m_JumpSound); // Giải phóng âm thanh nhảy
}
