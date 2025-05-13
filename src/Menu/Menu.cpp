#include "Menu.h"
#include <iostream>
Menu::Menu(SDL_Renderer* renderer): renderer(renderer), font(nullptr), scoreFont(nullptr), inTutorial(false), shouldExit(false), shouldStartGame(false), isPaused(false),
    inGame(false), isGameOver(false), shouldRestartGame(false), tutorialTexture(nullptr), menuBackgroundTexture(nullptr), gameOverTexture(nullptr) {

    font = TTF_OpenFont("text/The Bomb Sound.ttf", 75);
    scoreFont = TTF_OpenFont("text/The Bomb Sound.ttf", 50);
    if (!font || !scoreFont) {return;}

    // Tạo danh sách các nút văn bản cho menu chính
    vector<string> buttonTexts = {"Play", "Tutorial", "Exit"};
    int yOffset = SCREEN_HEIGHT / 4 + 38; // Vị trí bắt đầu của các nút

    for (const auto& text : buttonTexts) {
        Button btn;
        btn.text = text;
        btn.hover = false;
        btn.scale = 1.0f;
        // Tạo surface từ văn bản để lấy kích thước
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
        if (!surface) continue;

        // Thiết lập kích thước và vị trí của nút
        btn.rect.w = surface->w;
        btn.rect.h = surface->h;
        btn.rect.x = (SCREEN_WIDTH - btn.rect.w) / 2; // Căn giữa
        btn.rect.y = yOffset;
        yOffset += 100; // Tăng khoảng cách cho nút tiếp theo
        buttons.push_back(btn);
        SDL_FreeSurface(surface); // Giải phóng surface
    }

    // Thiết lập nút "Play Again" cho màn hình game over
    playAgainButton.text = "Play Again";
    playAgainButton.hover = false;
    playAgainButton.scale = 1.0f;
    SDL_Surface* playAgainSurface = TTF_RenderText_Solid(font, playAgainButton.text.c_str(), white);
    if (playAgainSurface) {
        playAgainButton.rect.w = playAgainSurface->w;
        playAgainButton.rect.h = playAgainSurface->h;
        playAgainButton.rect.x = (SCREEN_WIDTH - playAgainSurface->w) / 2; // Căn giữa
        playAgainButton.rect.y = SCREEN_HEIGHT / 2 + 50;
        SDL_FreeSurface(playAgainSurface);
    }

    // Thiết lập nút "Back to Menu" cho màn hình hướng dẫn
    backButton.text = "Back to Menu";
    backButton.hover = false;
    backButton.scale = 1.0f;
    SDL_Surface* backSurface = TTF_RenderText_Solid(font, backButton.text.c_str(), white);
    if (backSurface) {
        backButton.rect.w = backSurface->w;
        backButton.rect.h = backSurface->h;
        backButton.rect.x = SCREEN_WIDTH - backButton.rect.w - 20; // Góc phải dưới
        backButton.rect.y = SCREEN_HEIGHT - backButton.rect.h - 20;
        SDL_FreeSurface(backSurface);
    }

    // Tải và thiết lập nút hình ảnh "Home"
    SDL_Surface* homeSurface = IMG_Load("img/Menu/home.png");
    if (homeSurface) {
        homeButton.texture = SDL_CreateTextureFromSurface(renderer, homeSurface);
        homeButton.rect = {525, 20, 50, 50}; // Vị trí và kích thước nút
        homeButton.pauseTexture = nullptr;
        homeButton.playTexture = nullptr;
        SDL_FreeSurface(homeSurface);
    }

    // Tải và thiết lập nút hình ảnh "Pause/Play"
    SDL_Surface* pauseSurface = IMG_Load("img/Menu/pause.png");
    SDL_Surface* playSurface = IMG_Load("img/Menu/play.png");
    if (pauseSurface && playSurface) {
        pauseButton.pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
        pauseButton.playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
        pauseButton.texture = pauseButton.pauseTexture; // Mặc định là pause
        pauseButton.rect = {585, 20, 50, 50}; // Vị trí và kích thước nút
        SDL_FreeSurface(pauseSurface);
        SDL_FreeSurface(playSurface);
    }

}

// Hàm thiết lập các texture cho nền, hướng dẫn và game over
void Menu::setTextures(SDL_Texture* bgTexture, SDL_Texture* tutorialTexture, SDL_Texture* gameOverTexture) {
    this->menuBackgroundTexture = bgTexture;
    this->tutorialTexture = tutorialTexture;
    this->gameOverTexture = gameOverTexture;
}

// Hàm hủy Menu, giải phóng tài nguyên
Menu::~Menu() {
    TTF_CloseFont(font); // Giải phóng font
    TTF_CloseFont(scoreFont); // Giải phóng font điểm số
    SDL_DestroyTexture(homeButton.texture); // Giải phóng texture nút home
    SDL_DestroyTexture(pauseButton.pauseTexture); // Giải phóng texture pause
    SDL_DestroyTexture(pauseButton.playTexture); // Giải phóng texture play
}

// Hàm vẽ văn bản lên màn hình với font và tỷ lệ tùy chọn
void Menu::renderText(const std::string& text, int x, int y, SDL_Color color, float scale, bool isScore) {
    TTF_Font* targetFont = isScore ? scoreFont : font; // Chọn font phù hợp
    SDL_Surface* surface = TTF_RenderText_Solid(targetFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, int(surface->w * scale), int(surface->h * scale)};
    dst.x -= (dst.w - surface->w) / 2; // Căn giữa theo trục x
    dst.y -= (dst.h - surface->h) / 2; // Căn giữa theo trục y
    SDL_RenderCopy(renderer, texture, nullptr, &dst); // Vẽ texture
    SDL_FreeSurface(surface); // Giải phóng surface
    SDL_DestroyTexture(texture); // Giải phóng texture
}

// Hàm kiểm tra chuột có đang ở trên nút văn bản
bool Menu::isMouseOverButton(const Button& btn, int mouseX, int mouseY) {
    return mouseX >= btn.rect.x && mouseX <= btn.rect.x + btn.rect.w &&
           mouseY >= btn.rect.y && mouseY <= btn.rect.y + btn.rect.h;
}

// Hàm xử lý các sự kiện chuột và bàn phím
void Menu::handleEvents(SDL_Event& event, bool& running) {
    if (event.type == SDL_QUIT) {
        running = false; // Thoát game khi đóng cửa sổ
        shouldExit = true;
        return;
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY); // Lấy vị trí chuột

    if (event.type == SDL_MOUSEMOTION) {
        // Cập nhật trạng thái hover và tỷ lệ phóng to cho nút
        auto updateHover = [&](Button& btn) {
            btn.hover = isMouseOverButton(btn, mouseX, mouseY);
            btn.scale = btn.hover ? 1.2f : 1.0f; // Phóng to khi hover
        };

        // Cập nhật trạng thái hover dựa trên màn hình hiện tại
        if (!inTutorial && !inGame && !isGameOver)
            for (auto& btn : buttons) updateHover(btn);
        else if (inTutorial)         updateHover(backButton);
        else if (isGameOver)         updateHover(playAgainButton);
    }

    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        // Xử lý khi nhấn chuột trái
        if (isGameOver && isMouseOverButton(playAgainButton, mouseX, mouseY)) {
            shouldRestartGame = true; // Khởi động lại game
        }
        else if (!inTutorial && !inGame) {
            for (const auto& btn : buttons) {
                if (!isMouseOverButton(btn, mouseX, mouseY)) continue;

                if      (btn.text == "Play")     shouldStartGame = true; // Bắt đầu game
                else if (btn.text == "Tutorial") inTutorial = true; // Vào hướng dẫn
                else if (btn.text == "Exit") {
                    running = false; // Thoát game
                    shouldExit = true;
                }
            }
        }
        else if (inTutorial && isMouseOverButton(backButton, mouseX, mouseY)) {
            inTutorial = false; // Quay lại menu
        }
        else if (inGame) {
            if (isMouseOverImageButton(homeButton, mouseX, mouseY)) {
                inGame = false; // Quay về menu chính
                shouldStartGame = false;
                isPaused = false;
                pauseButton.texture = pauseButton.pauseTexture; // Đặt lại texture pause
            }
            if (isMouseOverImageButton(pauseButton, mouseX, mouseY)) {
                isPaused = !isPaused; // Chuyển đổi trạng thái tạm dừng
                pauseButton.texture = isPaused ? pauseButton.playTexture : pauseButton.pauseTexture;
            }
            }
    }
}

bool Menu::isMouseOverImageButton(const ImageButton& btn, int mouseX, int mouseY) {
    return mouseX >= btn.rect.x && mouseX <= btn.rect.x + btn.rect.w &&
           mouseY >= btn.rect.y && mouseY <= btn.rect.y + btn.rect.h;
}

// Hàm vẽ giao diện menu dựa trên trạng thái hiện tại
void Menu::render() {
    SDL_Color orange = {255, 165, 0, 255}; // Màu cam cho điểm số
    if (!inTutorial && !inGame && !isGameOver) {
        // Vẽ menu chính
        if (menuBackgroundTexture) {
            SDL_Rect dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, &dst); // Vẽ nền
        }
        for (const auto& btn : buttons) {
            renderText(btn.text, btn.rect.x, btn.rect.y, btn.hover ? yellow : white, btn.scale, false); // Vẽ các nút
        }
    } else if (inTutorial) {
        // Vẽ màn hình hướng dẫn
        if (tutorialTexture) {
            SDL_Rect dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, tutorialTexture, nullptr, &dst); // Vẽ texture hướng dẫn
        }
        renderText(backButton.text, backButton.rect.x, backButton.rect.y, backButton.hover ? yellow : white, backButton.scale, false); // Vẽ nút back
    } else if (inGame && !isGameOver) {
        // Vẽ giao diện trong game
        SDL_RenderCopy(renderer, homeButton.texture, nullptr, &homeButton.rect); // Vẽ nút home
        SDL_RenderCopy(renderer, pauseButton.texture, nullptr, &pauseButton.rect); // Vẽ nút pause/play
        std::string scoreText = "Score: " + std::to_string(score);
        renderText(scoreText, SCREEN_WIDTH - 200, 20, orange, 1.0f, true); // Vẽ điểm số
    } else if (isGameOver) {
        // Vẽ màn hình game over
        if (gameOverTexture) {
            SDL_Rect dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, gameOverTexture, nullptr, &dst); // Vẽ texture game over
        }
        std::string scoreText = "SCORE " + std::to_string(score);
        renderText(scoreText, SCREEN_WIDTH / 2 - 96, SCREEN_HEIGHT / 2 - 85, orange, 1.8f, true); // Vẽ điểm số
        renderText(playAgainButton.text, playAgainButton.rect.x, playAgainButton.rect.y,
                   playAgainButton.hover ? yellow : white, playAgainButton.scale, false); // Vẽ nút play again
    }
}
