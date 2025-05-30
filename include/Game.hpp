// game.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <sqlite3.h>
#include <vector>
#include <string>

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update();
    void render();
    void handleInput(const sf::Event& event);

    void loadFont();
    void loadAssets();
    void loadPhrases();
    std::string getRandomPhrase();

    void feedCat();
    void petCat();
    void interactWithAndrey();

private:
    sf::RenderWindow mWindow;
    sf::Font mFont;
    sf::Text mCoinText;
    sf::Text mPhraseText;

    sf::Texture mCoinsTexture;
    sf::Sprite mCoinsSprite;

    sf::Texture mCatTexture;
    sf::Sprite mCatSprite;

    sf::Texture mAndreyTexture;
    sf::Sprite mAndreySprite;

    int mCoins;

    sqlite3* mDb;
    std::vector<std::string> mPhrases;
};
