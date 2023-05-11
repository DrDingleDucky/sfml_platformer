#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Tile {
public:
    sf::RectangleShape tileRect;
    Tile(sf::Color tileColor, sf::Vector2f tileSize, sf::Vector2f tilePos) {
        tileRect.setFillColor(tileColor);
        tileRect.setSize(tileSize);
        tileRect.setPosition(tilePos);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(tileRect);
    }
};

class Player {
private:
    float playerSpeed;
    float playerGravity;
    float playerJumpVelocity;
    float fallMultiplier;
    float jumpFallMultiplier;
    float jumpBufferDelay;

    float jumpBufferTimer;
    bool isGrounded;
    bool jumpReady;
    sf::Vector2f playerDirection;
    sf::RectangleShape playerRect;
    std::vector<Tile> tileGroup;

public:
    Player(
        sf::Color playerColor,
        float playerSpeed,
        float playerGravity,
        float playerJumpVelocity,
        float fallMultiplier,
        float jumpFallMultiplier,
        float jumpBufferDelay,
        sf::Vector2f playerSize,
        sf::Vector2f playerPosition,
        std::vector<Tile>& tileGroup)
        : playerSpeed(playerSpeed)
        , playerGravity(playerGravity)
        , playerJumpVelocity(playerJumpVelocity)
        , fallMultiplier(fallMultiplier)
        , jumpFallMultiplier(jumpFallMultiplier)
        , jumpBufferDelay(jumpBufferDelay)
        , playerDirection(0.0f, 0.0f)
        , playerRect(playerSize)
        , tileGroup(tileGroup) {
        playerRect.setFillColor(playerColor);
        playerRect.setPosition(playerPosition);
        jumpBufferTimer = jumpBufferDelay;
    }

    void horizontalMovement(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)
            && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            playerDirection.x = 0;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            playerDirection.x = -1;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            playerDirection.x = 1;
        } else {
            playerDirection.x = 0;
        }

        playerRect.move(sf::Vector2f(playerDirection.x * deltaTime * playerSpeed, 0.0f));
    }

    void horizontalCollisions() {
        for (auto& tile : tileGroup) {
            if (playerRect.getGlobalBounds().intersects(tile.tileRect.getGlobalBounds())) {
                if (playerDirection.x > 0) {
                    playerRect.setPosition(sf::Vector2f(
                        tile.tileRect.getGlobalBounds().left - playerRect.getSize().x,
                        playerRect.getPosition().y));
                } else if (playerDirection.x < 0) {
                    playerRect.setPosition(sf::Vector2f(
                        tile.tileRect.getGlobalBounds().left + tile.tileRect.getSize().x,
                        playerRect.getPosition().y));
                }
            }
        }
    }

    void verticalMovement(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (isGrounded) {
                playerDirection.y = playerJumpVelocity;
                isGrounded = false;
                jumpReady = false;
            } else if (!jumpReady) {
                jumpReady = true;
            }
        } else if (playerDirection.y > 0 && isGrounded) {
            isGrounded = false;
        }

        if (jumpReady) {
            if (jumpBufferTimer < 0) {
                jumpReady = false;
                jumpBufferTimer = jumpBufferDelay;
            } else if (isGrounded) {
                playerDirection.y = playerJumpVelocity;
                isGrounded = false;
                jumpReady = false;
            }

            jumpBufferTimer -= deltaTime;
        }

        if (playerDirection.y > 0) {
            playerDirection.y += playerGravity * fallMultiplier * deltaTime;
        } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
            && playerDirection.y < 0) {
            playerDirection.y += playerGravity * jumpFallMultiplier * deltaTime;
        } else {
            playerDirection.y += playerGravity * deltaTime;
        }

        playerRect.move(sf::Vector2f(0, playerDirection.y * deltaTime));
    }

    void verticalCollisions() {
        for (auto& tile : tileGroup) {
            if (playerRect.getGlobalBounds().intersects(tile.tileRect.getGlobalBounds())) {
                if (playerDirection.y > 0) {
                    isGrounded = true;
                    playerDirection.y = 0;
                    playerRect.setPosition(sf::Vector2f(
                        playerRect.getPosition().x,
                        tile.tileRect.getGlobalBounds().top - playerRect.getSize().y));
                } else if (playerDirection.y < 0) {
                    playerDirection.y = 0;
                    playerRect.setPosition(sf::Vector2f(
                        playerRect.getPosition().x,
                        tile.tileRect.getGlobalBounds().top + tile.tileRect.getSize().y));
                }
            }
        }
    }

    void camera(sf::RenderWindow& window) {
        if (playerRect.getPosition().x + playerRect.getSize().x / 2.0f
            < window.getView().getCenter().x - window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRect.getPosition().x + playerRect.getSize().x / 2.0f
            > window.getView().getCenter().x + window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRect.getPosition().y + playerRect.getSize().y / 2.0f
            < window.getView().getCenter().y - window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRect.getPosition().y + playerRect.getSize().y / 2.0f
            > window.getView().getCenter().y + window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void update(sf::RenderWindow& window, float deltaTime) {
        horizontalMovement(deltaTime);
        horizontalCollisions();

        verticalMovement(deltaTime);
        verticalCollisions();

        camera(window);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(playerRect);
    }
};

void loadLevel(float &playerPositionX, float &playerPositionY, std::vector<Tile>& tileGroup) {
    std::ifstream file("map.txt");
    std::string line;

    float x;
    float y;

    if (file.is_open()) {
        float row_index = 0;
        while (std::getline(file, line)) {
            for (float collom_index = 0; collom_index < line.length(); collom_index++) {
                x = collom_index * 48.0f;
                y = row_index * 48.0f;
                if (line[collom_index] == '1') {
                    tileGroup.push_back(Tile(
                        sf::Color::Black,
                        sf::Vector2f(48.0f, 48.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == '2') {
                    playerPositionX = x;
                    playerPositionY = y;
                }
            }
            row_index++;
        }
    } else {
        std::cout << "error: can't open file"
                  << "\n";
    }
}

int main() {
    std::string windowTitle = "Game";
    int windowWidth = 1200;
    int windowHeight = 816;

    int windowPositionX = sf::VideoMode::getDesktopMode().width / 2 - windowWidth / 2;
    int windowPositionY = sf::VideoMode::getDesktopMode().height / 2 - windowHeight / 2;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
        windowTitle,
        sf::Style::Close);
    window.setPosition(sf::Vector2i(windowPositionX, windowPositionY));

    sf::Event event;

    sf::Clock clock;
    float deltaTime;

    std::vector<Tile> tileGroup;

    float playerPositionX;
    float playerPositionY;

    loadLevel(playerPositionX, playerPositionY, tileGroup);

    Player player(
        sf::Color::White, // player color
        450.0f, // player speed
        2175.0f, // player gravity
        -1000.0f, // player jump velocity
        3.0f, // fall multiplier
        5.0f, // jump fall multiplier
        0.1f, // jump buffer timer
        sf::Vector2f(48.0f, 96.0f), // player size
        sf::Vector2f(playerPositionX, playerPositionY), // player position
        tileGroup);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            }
        }

        deltaTime = clock.restart().asSeconds();

        player.update(window, deltaTime);

        window.clear(sf::Color(64, 64, 64, 255));

        player.draw(window);

        for (auto& tile : tileGroup) {
            tile.draw(window);
        }

        window.display();
    }
    return 0;
}