#include "Menu.h"
#include <iostream>

Menu::Menu(SDL_Renderer* renderer): renderer(renderer), font(nullptr), scoreFont(nullptr), inTutorial(false), shouldExit(false), shouldStartGame(false), isPaused(false),
    inGame(false), isGameOver(false), shouldRestartGame(false), tutorialTexture(nullptr), menuBackgroundTexture(nullptr), gameOverTexture(nullptr) {
    font = TTF_OpenFont("text/The Bomb Sound.ttf", 75);
    scoreFont = TTF_OpenFont("text/The Bomb Sound.ttf", 50);
    if (!font || !scoreFont) {return;}
    vector<string> buttonTexts = {"Play", "Tutorial", "Exit"};
    int yOffset = SCREEN_HEIGHT / 4 + 38;
    for (const auto& text : buttonTexts) {
        Button btn;
        btn.text = text;
        btn.hover = false;
        btn.scale = 1.0f;
        SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
        if (!surface) continue;

        btn.rect.w = surface->w;
        btn.rect.h = surface->h;
        btn.rect.x = (SCREEN_WIDTH - btn.rect.w) / 2;
        btn.rect.y = yOffset;
        yOffset += 100;
        buttons.push_back(btn);
        SDL_FreeSurface(surface);
    }

    playAgainButton.text = "Play Again";
    playAgainButton.hover = false;
    playAgainButton.scale = 1.0f;
    SDL_Surface* playAgainSurface = TTF_RenderText_Solid(font, playAgainButton.text.c_str(), white);
    if (playAgainSurface) {
        playAgainButton.rect.w = playAgainSurface->w;
        playAgainButton.rect.h = playAgainSurface->h;
        playAgainButton.rect.x = (SCREEN_WIDTH - playAgainSurface->w) / 2;
        playAgainButton.rect.y = SCREEN_HEIGHT / 2 + 50;
        SDL_FreeSurface(playAgainSurface);
    }
    backButton.text = "Back to Menu";
    backButton.hover = false;
    backButton.scale = 1.0f;
    SDL_Surface* backSurface = TTF_RenderText_Solid(font, backButton.text.c_str(), white);
    if (backSurface) {
        backButton.rect.w = backSurface->w;
        backButton.rect.h = backSurface->h;
        backButton.rect.x = SCREEN_WIDTH - backButton.rect.w - 20;
        backButton.rect.y = SCREEN_HEIGHT - backButton.rect.h - 20;
        SDL_FreeSurface(backSurface);
    }
    SDL_Surface* homeSurface = IMG_Load("img/Menu/home.png");
    if (homeSurface) {
        homeButton.texture = SDL_CreateTextureFromSurface(renderer, homeSurface);
        homeButton.rect = {525, 20, 50, 50};
        homeButton.pauseTexture = nullptr;
        homeButton.playTexture = nullptr;
        SDL_FreeSurface(homeSurface);
    }
    SDL_Surface* pauseSurface = IMG_Load("img/Menu/pause.png");
    SDL_Surface* playSurface = IMG_Load("img/Menu/play.png");
    if (pauseSurface && playSurface) {
        pauseButton.pauseTexture = SDL_CreateTextureFromSurface(renderer, pauseSurface);
        pauseButton.playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
        pauseButton.texture = pauseButton.pauseTexture;
        pauseButton.rect = {585, 20, 50, 50};
        SDL_FreeSurface(pauseSurface);
        SDL_FreeSurface(playSurface);
    }
}

void Menu::setTextures(SDL_Texture* bgTexture, SDL_Texture* tutorialTexture, SDL_Texture* gameOverTexture) {
    this->menuBackgroundTexture = bgTexture;
    this->tutorialTexture = tutorialTexture;
    this->gameOverTexture = gameOverTexture;
}

Menu::~Menu() {
    TTF_CloseFont(font);
    SDL_DestroyTexture(homeButton.texture);
    SDL_DestroyTexture(pauseButton.pauseTexture);
    SDL_DestroyTexture(pauseButton.playTexture);
}

void Menu::renderText(const std::string& text, int x, int y, SDL_Color color, float scale, bool isScore) {
    TTF_Font* targetFont = isScore ? scoreFont : font;
    SDL_Surface* surface = TTF_RenderText_Solid(targetFont, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dst = {x, y, int(surface->w * scale), int(surface->h * scale)};
    dst.x -= (dst.w - surface->w) / 2;
    dst.y -= (dst.h - surface->h) / 2;
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

bool Menu::isMouseOverButton(const Button& btn, int mouseX, int mouseY) {
    return mouseX >= btn.rect.x && mouseX <= btn.rect.x + btn.rect.w &&
           mouseY >= btn.rect.y && mouseY <= btn.rect.y + btn.rect.h;
}

void Menu::handleEvents(SDL_Event& event, bool& running) {
    if (event.type == SDL_QUIT) {
        running = false;
        shouldExit = true;
        return;
    }

    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    if (event.type == SDL_MOUSEMOTION) {
        auto updateHover = [&](Button& btn) {
            btn.hover = isMouseOverButton(btn, mouseX, mouseY);
            btn.scale = btn.hover ? 1.2f : 1.0f;
        };

        if (!inTutorial && !inGame && !isGameOver)
            for (auto& btn : buttons) updateHover(btn);
        else if (inTutorial)         updateHover(backButton);
        else if (isGameOver)         updateHover(playAgainButton);
    }

    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (isGameOver && isMouseOverButton(playAgainButton, mouseX, mouseY)) {
            shouldRestartGame = true;
        }
        else if (!inTutorial && !inGame) {
            for (const auto& btn : buttons) {
                if (!isMouseOverButton(btn, mouseX, mouseY)) continue;

                if      (btn.text == "Play")     shouldStartGame = true;
                else if (btn.text == "Tutorial") inTutorial = true;
                else if (btn.text == "Exit") {
                    running = false;
                    shouldExit = true;
                }
            }
        }
        else if (inTutorial && isMouseOverButton(backButton, mouseX, mouseY)) {
            inTutorial = false;
        }
        else if (inGame) {
            if (isMouseOverImageButton(homeButton, mouseX, mouseY)) {
                inGame = false;
                shouldStartGame = false;
                isPaused = false;
                pauseButton.texture = pauseButton.pauseTexture;
            }
            if (isMouseOverImageButton(pauseButton, mouseX, mouseY)) {
                isPaused = !isPaused;
                pauseButton.texture = isPaused ? pauseButton.playTexture : pauseButton.pauseTexture;
            }
        }
    }
}

bool Menu::isMouseOverImageButton(const ImageButton& btn, int mouseX, int mouseY) {
    return mouseX >= btn.rect.x && mouseX <= btn.rect.x + btn.rect.w &&
           mouseY >= btn.rect.y && mouseY <= btn.rect.y + btn.rect.h;
}

void Menu::render() {
    SDL_Color orange = {255, 165, 0, 255};
    if (!inTutorial && !inGame && !isGameOver) {
        if (menuBackgroundTexture) {
            SDL_Rect dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, menuBackgroundTexture, nullptr, &dst);
        }
        for (const auto& btn : buttons) {
            renderText(btn.text, btn.rect.x, btn.rect.y, btn.hover ? yellow : white, btn.scale, false);
        }
    } else if (inTutorial) {
        if (tutorialTexture) {
            SDL_Rect dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, tutorialTexture, nullptr, &dst);
        }
        renderText(backButton.text, backButton.rect.x, backButton.rect.y, backButton.hover ? yellow : white, backButton.scale, false);
    } else if (inGame && !isGameOver) {
        SDL_RenderCopy(renderer, homeButton.texture, nullptr, &homeButton.rect);
        SDL_RenderCopy(renderer, pauseButton.texture, nullptr, &pauseButton.rect);
        std::string scoreText = "Score: " + std::to_string(score);
        renderText(scoreText, SCREEN_WIDTH - 200, 20, orange, 1.0f, true);
    } else if (isGameOver) {
        if (gameOverTexture) {
            SDL_Rect dst = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
            SDL_RenderCopy(renderer, gameOverTexture, nullptr, &dst);
        }
        std::string scoreText = "SCORE " + std::to_string(score);
        renderText(scoreText, SCREEN_WIDTH / 2 - 96, SCREEN_HEIGHT / 2 - 85, orange, 1.8f, true);
        renderText(playAgainButton.text, playAgainButton.rect.x, playAgainButton.rect.y,
                   playAgainButton.hover ? yellow : white, playAgainButton.scale, false);
    }
}
