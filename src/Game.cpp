// game.cpp
#include "Game.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>

Game::Game() : mWindow(sf::VideoMode(800, 600), "Кликер с котом"), mCoins(0), mDb(nullptr) {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    loadFont();
    loadAssets();
    loadPhrases();

    mCoinText.setFont(mFont);
    mCoinText.setCharacterSize(20);
    mCoinText.setFillColor(sf::Color::White);
    mCoinText.setPosition(10, 10);

    mPhraseText.setFont(mFont);
    mPhraseText.setCharacterSize(16);
    mPhraseText.setFillColor(sf::Color::Yellow);
    mPhraseText.setPosition(10, 550);

    mCatSprite.setPosition(550, 300);
    mAndreySprite.setPosition(0, 0);
    mCoinsSprite.setPosition(0, 0);
}

void Game::run() {
    while (mWindow.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            mWindow.close();

        handleInput(event);
    }
}

void Game::update() {
    mCoinText.setString("Монеты: " + std::to_string(mCoins));
}

void Game::render() {
    mWindow.clear();
    mWindow.draw(mCatSprite);
    mWindow.draw(mAndreySprite);
    mWindow.draw(mCoinsSprite);
    mWindow.draw(mCoinText);
    mWindow.draw(mPhraseText);
    mWindow.display();
}

void Game::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        auto pos = sf::Mouse::getPosition(mWindow);
        if (mCatSprite.getGlobalBounds().contains(static_cast<float>(pos.x), static_cast<float>(pos.y))) {
            if (event.mouseButton.button == sf::Mouse::Left) petCat();
        }
        if (mAndreySprite.getGlobalBounds().contains(static_cast<float>(pos.x), static_cast<float>(pos.y))) {
            if (event.mouseButton.button == sf::Mouse::Left) interactWithAndrey();
        }
    }
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        feedCat();
    }
}

void Game::loadFont() {
    if (!mFont.loadFromFile("assets/font.ttf")) {
        std::cerr << "Не удалось загрузить шрифт!" << std::endl;
    }
}

void Game::loadAssets() {
    mCatTexture.loadFromFile("cat.png");
    mCatSprite.setTexture(mCatTexture);

    mAndreyTexture.loadFromFile("andrey.png");
    mAndreySprite.setTexture(mAndreyTexture);

    mCoinsTexture.loadFromFile("coins.png");
    mCoinsSprite.setTexture(mCoinsTexture);
}

void Game::loadPhrases() {
    if (sqlite3_open("phrases.db", &mDb) != SQLITE_OK) {
        std::cerr << "Не удалось открыть БД: " << sqlite3_errmsg(mDb) << std::endl;
        return;
    }

    sqlite3_stmt* stmt;
    const char* sql = "SELECT phrase FROM phrases;";

    if (sqlite3_prepare_v2(mDb, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const unsigned char* text = sqlite3_column_text(stmt, 0);
            mPhrases.emplace_back(reinterpret_cast<const char*>(text));
        }
        sqlite3_finalize(stmt);
    }
}

std::string Game::getRandomPhrase() {
    if (mPhrases.empty()) return "Нет фраз :(";
    int index = std::rand() % mPhrases.size();
    return mPhrases[index];
}

void Game::feedCat() {
    mCoins += 5;
}

void Game::petCat() {
    mCoins += 1;
}

void Game::interactWithAndrey() {
    if (mCoins >= 100) {
        mCoins -= 100;
        mPhraseText.setString(getRandomPhrase());
    } else {
        mPhraseText.setString("Недостаточно монет!");
    }
}
