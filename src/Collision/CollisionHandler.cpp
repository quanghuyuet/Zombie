#include "CollisionHandler.h"

// Khởi tạo con trỏ tĩnh s_Instance thành nullptr (chưa có instance)
CollisionHandler* CollisionHandler::s_Instance = nullptr;

// Hàm khởi tạo CollisionHandler
CollisionHandler::CollisionHandler() {
    // Lấy tilemap va chạm từ lớp Map (bản đồ)
    m_CollisionTilemap = Map::GetInstance()->GetTileMap();
    // Tilemap là mảng 2D, mỗi ô có giá trị:
    // > 0: Ô rắn (tường, nền đất)
    // 0: Ô rỗng (không khí)
}

// Hàm hủy để giải phóng tài nguyên
CollisionHandler::~CollisionHandler() {
    delete s_Instance; // Xóa instance duy nhất
    s_Instance = nullptr; // Đặt lại con trỏ về null
}

// Hàm lấy instance duy nhất của CollisionHandler (singleton)
CollisionHandler* CollisionHandler::GetInstance() {
    if (s_Instance == nullptr) { // Nếu chưa có instance
        s_Instance = new CollisionHandler(); // Tạo instance mới
    }
    return s_Instance; // Trả về instance
}

// Hàm kiểm tra va chạm giữa một hình chữ nhật và bản đồ
bool CollisionHandler::MapCollision(SDL_Rect a) {
    int tileSize = TILE_SIZE; // Kích thước mỗi ô (tile) trên bản đồ
    int RowCount = m_CollisionTilemap.size(); // Số hàng của tilemap
    int ColCount = (RowCount > 0) ? m_CollisionTilemap[0].size() : 0; // Số cột của tilemap

    // Tính toán các ô mà hình chữ nhật (a) bao phủ
    int left_tile = a.x / tileSize; // Ô bên trái
    int right_tile = (a.x + a.w - 1) / tileSize; // Ô bên phải
    int top_tile = a.y / tileSize; // Ô phía trên
    int bottom_tile = (a.y + a.h - 1) / tileSize; // Ô phía dưới

    // Giới hạn để không vượt ra ngoài tilemap
    if (left_tile < 0) left_tile = 0;
    if (right_tile >= ColCount) right_tile = ColCount - 1;
    if (top_tile < 0) top_tile = 0;
    if (bottom_tile >= RowCount) bottom_tile = RowCount - 1;

    // Kiểm tra từng ô trong vùng bao phủ
    for (int i = left_tile; i <= right_tile; i++) {
        for (int j = top_tile; j <= bottom_tile; j++) {
            if (m_CollisionTilemap[j][i] > 0) { // Nếu ô là rắn (có va chạm)
                return true; // Có va chạm với bản đồ
            }
        }
    }
    return false; // Không có va chạm
}

// Hàm kiểm tra va chạm giữa hai hình chữ nhật
bool CollisionHandler::CheckCollision(SDL_Rect a, SDL_Rect b) {
    // Kiểm tra xem hai hình chữ nhật có giao nhau không
    return (a.x < b.x + b.w && // Bên trái của a nhỏ hơn bên phải của b
            a.x + a.w > b.x && // Bên phải của a lớn hơn bên trái của b
            a.y < b.y + b.h && // Phía trên của a nhỏ hơn phía dưới của b
            a.y + a.h > b.y);  // Phía dưới của a lớn hơn phía trên của b
}
