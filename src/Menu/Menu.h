#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include "RigidBody.h"
#include "TextureManager.h"
using namespace std;

// Cấu trúc Button dùng để lưu thông tin nút bấm văn bản
struct Button {
    std::string text; // Nội dung văn bản của nút
    SDL_Rect rect;    // Hình chữ nhật xác định vị trí và kích thước nút
    bool hover;       // Trạng thái chuột có đang di chuột qua nút
    float scale;      // Tỷ lệ phóng to của nút
};

// Cấu trúc ImageButton dùng để lưu thông tin nút bấm dạng hình ảnh
struct ImageButton {
    SDL_Texture* texture;       // Texture mặc định của nút
    SDL_Texture* pauseTexture;  // Texture khi ở trạng thái tạm dừng
    SDL_Texture* playTexture;   // Texture khi ở trạng thái phát
    SDL_Rect rect;             // Hình chữ nhật xác định vị trí và kích thước nút
};

// Lớp Menu quản lý giao diện người dùng và các trạng thái của menu
class Menu {
private:
    SDL_Renderer* renderer;         // Con trỏ đến renderer của SDL
    TTF_Font* font;                 // Font chữ cho văn bản menu
    TTF_Font* scoreFont;            // Font chữ cho điểm số
    vector<Button> buttons;         // Danh sách các nút văn bản
    bool inTutorial;                // Trạng thái có đang trong hướng dẫn
    Button backButton;              // Nút quay lại
    ImageButton homeButton;         // Nút về màn hình chính
    ImageButton pauseButton;        // Nút tạm dừng/tiếp tục
    Button playAgainButton;         // Nút chơi lại
    bool shouldExit;                // Trạng thái thoát game
    bool shouldStartGame;           // Trạng thái bắt đầu game
    bool isPaused;                  // Trạng thái game tạm dừng
    bool inGame;                    // Trạng thái đang chơi game
    bool isGameOver;                // Trạng thái game kết thúc
    bool shouldRestartGame;         // Trạng thái cần khởi động lại game
    SDL_Texture* tutorialTexture;   // Texture cho màn hình hướng dẫn
    SDL_Texture* menuBackgroundTexture; // Texture cho nền menu
    SDL_Texture* gameOverTexture;   // Texture cho màn hình game over
    SDL_Color white = {255, 255, 255, 255}; // Màu trắng
    SDL_Color yellow = {255, 255, 0, 255};  // Màu vàng
    SDL_Color gray = {128, 128, 128, 255};  // Màu xám
    int score = 0;                  // Điểm số hiện tại

public:
    Menu(SDL_Renderer* renderer);
    ~Menu();
    void render();
    // Hàm thiết lập các texture cho menu
    void setTextures(SDL_Texture* bgTexture, SDL_Texture* tutorialTexture, SDL_Texture* gameOverTexture);

    // Hàm xử lý sự kiện (chuột, bàn phím)
    void handleEvents(SDL_Event& event, bool& running);

    // Hàm vẽ văn bản lên màn hình
    void renderText(const string& text, int x, int y, SDL_Color color, float scale, bool isScore = false);

    // Hàm kiểm tra chuột có đang ở trên nút văn bản
    bool isMouseOverButton(const Button& btn, int mouseX, int mouseY);

    // Hàm kiểm tra chuột có đang ở trên nút hình ảnh
    bool isMouseOverImageButton(const ImageButton& btn, int mouseX, int mouseY);

    // Hàm kiểm tra trạng thái bắt đầu game
    bool shouldStart() const { return shouldStartGame;}

    // Hàm kiểm tra trạng thái thoát game
    bool shouldQuit() const { return shouldExit;}

    // Hàm kiểm tra trạng thái trong hướng dẫn
    bool isInTutorial() const { return inTutorial;}

    // Hàm kiểm tra trạng thái tạm dừng game
    bool isGamePaused() const { return isPaused;}

    // Hàm kiểm tra trạng thái đang chơi game
    bool isInGame() const { return inGame;}

    // Hàm thiết lập trạng thái đang chơi game
    void setInGame(bool state) {inGame = state;}

    // Hàm thiết lập điểm số
    void setScore(int score) { this->score = score; }

    // Hàm thiết lập trạng thái game kết thúc
    void setGameOver(bool isOver) {isGameOver = isOver;}

    // Hàm kiểm tra trạng thái khởi động lại game
    bool shouldRestart() const { return shouldRestartGame;}

    // Hàm đặt lại trạng thái khởi động lại game
    void resetRestartState() { shouldRestartGame = false;}
};

