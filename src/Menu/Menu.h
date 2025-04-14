#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <string>
#include <vector>
#include "RigidBody.h"
#include "TextureManager.h"
using namespace std;
struct Button {
    std::string text;
    SDL_Rect rect;
    bool hover;
    float scale;
};

struct ImageButton {
    SDL_Texture* texture;
    SDL_Texture* pauseTexture;
    SDL_Texture* playTexture;
    SDL_Rect rect;
};

class Menu {
private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    TTF_Font* scoreFont;
    vector<Button> buttons;
    bool inTutorial;
    Button backButton;
    ImageButton homeButton;
    ImageButton pauseButton;
    Button playAgainButton;
    bool shouldExit;
    bool shouldStartGame;
    bool isPaused;
    bool inGame;
    bool isGameOver;
    bool shouldRestartGame;
    SDL_Texture* tutorialTexture;
    SDL_Texture* menuBackgroundTexture;
    SDL_Texture* gameOverTexture;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color gray = {128, 128, 128, 255};
    int score = 0;

public:
    Menu(SDL_Renderer* renderer);
    ~Menu();
    void render();
    void setTextures(SDL_Texture* bgTexture, SDL_Texture* tutorialTexture, SDL_Texture* gameOverTexture);
    void handleEvents(SDL_Event& event, bool& running);
    void renderText(const string& text, int x, int y, SDL_Color color, float scale, bool isScore = false);
    bool isMouseOverButton(const Button& btn, int mouseX, int mouseY);
    bool isMouseOverImageButton(const ImageButton& btn, int mouseX, int mouseY);
    bool shouldStart() const { return shouldStartGame;}
    bool shouldQuit() const { return shouldExit;}
    bool isInTutorial() const { return inTutorial;}
    bool isGamePaused() const { return isPaused;}
    bool isInGame() const { return inGame;}
    void setInGame(bool state) {inGame = state;}
    void setScore(int score) { this->score = score; }
    void setGameOver(bool isOver) {isGameOver = isOver;}
    bool shouldRestart() const { return shouldRestartGame;}
    void resetRestartState() { shouldRestartGame = false;}
};
