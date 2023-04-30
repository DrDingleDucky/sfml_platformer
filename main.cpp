#include <SFML/Graphics.hpp>
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
    float playerJumpVelocity;
    float fallMultiplayer;
    float playerGravity;

    bool isGounded;
    std::vector<Tile> tileGroup;

   public:
    sf::Vector2f playerDirection;
    sf::RectangleShape playerRect;
    Player(sf::Color playerColor, float playerSpeed, float playerJumpVelocity, float fallMultiplayer, float playerGravity, sf::Vector2f playerSize, sf::Vector2f playerPos, std::vector<Tile>& vector)
        : playerSpeed(playerSpeed), playerJumpVelocity(playerJumpVelocity), fallMultiplayer(fallMultiplayer), playerGravity(playerGravity), playerDirection(0.0f, 0.0f) {
        tileGroup = vector;
        isGounded = false;
        playerRect.setFillColor(playerColor);
        playerRect.setSize(playerSize);
        playerRect.setPosition(playerPos);
    }

    void horizontalMovement(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
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
                    playerRect.setPosition(sf::Vector2f(tile.tileRect.getGlobalBounds().left - playerRect.getSize().x, playerRect.getPosition().y));
                } else if (playerDirection.x < 0) {
                    playerRect.setPosition(sf::Vector2f(tile.tileRect.getGlobalBounds().left + tile.tileRect.getSize().x, playerRect.getPosition().y));
                }
            }
        }
    }

    void verticalMovement(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && isGounded) {
            playerDirection.y = playerJumpVelocity;
            isGounded = false;
        }

        if (playerDirection.y > 0 || !sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && playerDirection.y < 0) {
            playerDirection.y += playerGravity * fallMultiplayer;
        } else {
            playerDirection.y += playerGravity;
        }
        playerRect.move(sf::Vector2f(0, playerDirection.y * deltaTime));
    }

    void verticalCollisions() {
        for (auto& tile : tileGroup) {
            if (playerRect.getGlobalBounds().intersects(tile.tileRect.getGlobalBounds())) {
                if (playerDirection.y > 0) {
                    isGounded = true;
                    playerDirection.y = 0;
                    playerRect.setPosition(sf::Vector2f(playerRect.getPosition().x, tile.tileRect.getGlobalBounds().top - playerRect.getSize().y));
                } else if (playerDirection.y < 0) {
                    playerDirection.y = 0;
                    playerRect.setPosition(sf::Vector2f(playerRect.getPosition().x, tile.tileRect.getGlobalBounds().top + tile.tileRect.getSize().y));
                }
            }
        }
    }

    void update(float deltaTime) {
        horizontalMovement(deltaTime);
        horizontalCollisions();

        verticalMovement(deltaTime);
        verticalCollisions();
    }

    void draw(sf::RenderWindow& window) {
        window.draw(playerRect);
    }
};

int main() {
    std::string windowTitle = "Game";
    int windowWidth = 1200;
    int windowHeight = 816;

    int windowPositionX = sf::VideoMode::getDesktopMode().width / 2 - windowWidth / 2;
    int windowPositionY = sf::VideoMode::getDesktopMode().height / 2 - windowHeight / 2;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle, sf::Style::Close);
    window.setPosition(sf::Vector2i(windowPositionX, windowPositionY));
    sf::Event event;

    sf::Clock clock;
    float deltaTime;

    std::vector<Tile> tileGroup;

    tileGroup.push_back(Tile(sf::Color::Black, sf::Vector2f(1008, 96), sf::Vector2f(96, 624)));
    tileGroup.push_back(Tile(sf::Color::Black, sf::Vector2f(288, 96), sf::Vector2f(336, 528)));
    tileGroup.push_back(Tile(sf::Color::Black, sf::Vector2f(288, 48), sf::Vector2f(672, 336)));

    Player player(sf::Color::White, 380.0f, -1200.0f, 2.0f, 0.25f, sf::Vector2f(48.0f, 96.0f), sf::Vector2f(144.0f, 96.0f), tileGroup);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            }
        }
        deltaTime = clock.restart().asSeconds();
        player.update(deltaTime);

        window.clear(sf::Color(64, 64, 64, 255));

        player.draw(window);

        for (auto& tile : tileGroup) {
            tile.draw(window);
        }

        window.display();
    }
    return 0;
}