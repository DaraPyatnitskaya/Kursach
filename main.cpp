#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <sqlite3.h>

int coins = 0;
int clickValue = 1;
int feedPrice = 10;
sf::Font font;

std::string getRandomPhrase(sqlite3* db) {
    std::string phrase = "Ошибка чтения";
    sqlite3_stmt* stmt;
    const char* sql = "SELECT phrase FROM phrases ORDER BY RANDOM() LIMIT 1";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            phrase = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
        sqlite3_finalize(stmt);
    }
    return phrase;
}

struct FloatingText {
    sf::Text text;
    sf::Clock lifeClock;
    float startAlpha = 255.f;
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(700, 500), "Cat Clicker with Andrey");

    if (!font.loadFromFile("assets/font.ttf")) {
        std::cerr << "Font not found!\n";
        return 1;
    }

    // Звук
    sf::SoundBuffer coinBuffer, purrBuffer, bgBuffer;
    if (!coinBuffer.loadFromFile("assets/coin.wav") ||
        !purrBuffer.loadFromFile("assets/purr.wav") ||
        !bgBuffer.loadFromFile("assets/background_music.wav")) {
        std::cerr << "Sound files not found!\n";
        return 1;
    }

    sf::Sound coinSound(coinBuffer);
    sf::Sound purrSound(purrBuffer);
    sf::Sound backgroundSound(bgBuffer);
    backgroundSound.setLoop(true);
    backgroundSound.setVolume(10.f);
    backgroundSound.play();

    // Фон
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("assets/background.png")) {
        std::cerr << "Background not found!\n";
        return 1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);
    backgroundSprite.setScale(700.f / backgroundTexture.getSize().x, 500.f / backgroundTexture.getSize().y);

    // Картина (рандомная)
    sf::Texture paintingTexture;
    int paintingIndex = 1 + (std::rand() % 10);
    std::string paintingPath = "assets/painting" + std::to_string(paintingIndex) + ".png";
    if (!paintingTexture.loadFromFile(paintingPath)) {
        std::cerr << "Painting not found: " << paintingPath << "\n";
        return 1;
    }
    sf::Sprite paintingSprite(paintingTexture);
    paintingSprite.setScale(0.15f, 0.15f);
    paintingSprite.setPosition(250, 130);

    // Монеты
    sf::Texture coinsTexture;
    coinsTexture.loadFromFile("assets/coins.png");
    sf::Sprite coinsSprite(coinsTexture);
    coinsSprite.setScale(0.03f, 0.03f);
    coinsSprite.setPosition(0, 0);

    sf::Text coinText;
    coinText.setFont(font);
    coinText.setCharacterSize(24);
    coinText.setPosition(60, 28);
    coinText.setFillColor(sf::Color::Yellow);

    // Кот
    sf::Texture catTexture;
    catTexture.loadFromFile("assets/cat.png");
    sf::Sprite catSprite(catTexture);
    catSprite.setScale(0.1f, 0.1f);
    catSprite.setPosition(330, 127);

    // Кнопка корма
    sf::Texture feedTexture;
    feedTexture.loadFromFile("assets/feed_button.png");
    sf::Sprite feedButton(feedTexture);
    feedButton.setScale(0.06f, 0.06f);
    feedButton.setPosition(480, 35);

    sf::Text feedPriceText;
    feedPriceText.setFont(font);
    feedPriceText.setCharacterSize(18);
    feedPriceText.setFillColor(sf::Color::Yellow);
    feedPriceText.setPosition(494, 131);
    feedPriceText.setString("Price: " + std::to_string(feedPrice));

    bool isFeedButtonPressed = false;

    // Андрей
    sf::Texture andreyTexture1, andreyTexture2;
    andreyTexture1.loadFromFile("assets/andrey.png");
    andreyTexture2.loadFromFile("assets/andrey_talk.png");

    sf::Sprite andreySprite(andreyTexture1);
    andreySprite.setScale(0.25f, 0.25f);
    andreySprite.setPosition(0, 60);

    sf::Text andreyText;
    andreyText.setFont(font);
    andreyText.setCharacterSize(18);
    andreyText.setFillColor(sf::Color::White);
    andreyText.setPosition(20, 440);

    std::string currentPhrase;
    std::string displayedPhrase;
    sf::Clock typingClock;
    sf::Clock animationClock;
    float typingSpeed = 0.02f;
    bool isTyping = false;
    bool showFirstTexture = true;
    float animationSpeed = 0.15f;

    sqlite3* db;
    if (sqlite3_open("phrases.db", &db)) {
        std::cerr << "Can't open DB: " << sqlite3_errmsg(db) << "\n";
        return 1;
    }

    std::vector<FloatingText> floatingTexts;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                auto mousePos = sf::Mouse::getPosition(window);

                if (feedButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    isFeedButtonPressed = true;
                    feedButton.setScale(0.055f, 0.055f);
                }
                else if (catSprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    coins += clickValue;
                    coinSound.play();
                    purrSound.play();

                    FloatingText ft;
                    ft.text.setFont(font);
                    ft.text.setCharacterSize(20);
                    ft.text.setFillColor(sf::Color::Yellow);
                    ft.text.setString("+" + std::to_string(clickValue));
                    ft.text.setPosition(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                    floatingTexts.push_back(ft);
                }
                else if (andreySprite.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                    if (coins >= 100) {
                        coins -= 100;
                        currentPhrase = getRandomPhrase(db);
                        displayedPhrase.clear();
                        isTyping = true;
                        typingClock.restart();
                        animationClock.restart();
                        showFirstTexture = true;
                    } else {
                        currentPhrase = "Не хватает монет (100)";
                        displayedPhrase = currentPhrase;
                        isTyping = false;
                        andreySprite.setTexture(andreyTexture1);
                    }
                    andreyText.setString(sf::String::fromUtf8(displayedPhrase.begin(), displayedPhrase.end()));
                }
            } else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                auto mousePos = sf::Mouse::getPosition(window);

                if (isFeedButtonPressed) {
                    isFeedButtonPressed = false;
                    feedButton.setScale(0.06f, 0.06f);

                    if (feedButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (coins >= feedPrice) {
                            coins -= feedPrice;
                            clickValue *= 2;
                            feedPrice *= 3;
                            feedPriceText.setString("Price: " + std::to_string(feedPrice));
                            coinSound.play();
                        }
                    }
                }
            }
        }

        // Печать текста
        if (isTyping) {
            float elapsed = typingClock.getElapsedTime().asSeconds();
            size_t charsToShow = static_cast<size_t>(elapsed / typingSpeed);

            if (charsToShow >= currentPhrase.size()) {
                displayedPhrase = currentPhrase;
                isTyping = false;
                andreySprite.setTexture(andreyTexture1);
            } else {
                displayedPhrase = currentPhrase.substr(0, charsToShow);
            }

            andreyText.setString(sf::String::fromUtf8(displayedPhrase.begin(), displayedPhrase.end()));

            if (animationClock.getElapsedTime().asSeconds() > animationSpeed) {
                showFirstTexture = !showFirstTexture;
                andreySprite.setTexture(showFirstTexture ? andreyTexture1 : andreyTexture2);
                animationClock.restart();
            }
        }

        for (auto& ft : floatingTexts) {
            float elapsed = ft.lifeClock.getElapsedTime().asSeconds();
            ft.text.move(0, -0.1f);
            float alpha = ft.startAlpha * (1.0f - elapsed);
            if (alpha < 0) alpha = 0;
            sf::Color color = ft.text.getFillColor();
            color.a = static_cast<sf::Uint8>(alpha);
            ft.text.setFillColor(color);
        }

        floatingTexts.erase(
            std::remove_if(floatingTexts.begin(), floatingTexts.end(),
                [](const FloatingText& ft) {
                    return ft.lifeClock.getElapsedTime().asSeconds() > 1.0f;
                }),
            floatingTexts.end()
        );

        coinText.setString(std::to_string(coins));

        window.clear();
        window.draw(backgroundSprite);
        window.draw(paintingSprite); // <- картина на стене
        window.draw(catSprite);
        window.draw(feedButton);
        window.draw(feedPriceText);
        window.draw(andreySprite);
        window.draw(coinText);
        window.draw(coinsSprite);
        for (auto& ft : floatingTexts) window.draw(ft.text);
        window.draw(andreyText);
        window.display();
    }

    sqlite3_close(db);
    return 0;
}
