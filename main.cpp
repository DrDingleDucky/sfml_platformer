#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Tile1 {
public:
    sf::RectangleShape tileRectangle;
    Tile1(sf::Color color,
          sf::Vector2f size,
          sf::Vector2f position)
        : tileColor(color),
          tileSize(size),
          tileStartPosition(position) {
        tileRectangle.setFillColor(tileColor);
        tileRectangle.setSize(tileSize);
        tileRectangle.setPosition(tileStartPosition);
    }

    void draw(sf::RenderWindow &window) {
        window.draw(tileRectangle);
    }

private:
    sf::Color tileColor;
    sf::Vector2f tileSize;
    sf::Vector2f tileStartPosition;
};

class Tile2 {
public:
    sf::RectangleShape tileRectangle;
    sf::RectangleShape tileRectangleTop;
    Tile2(sf::Color color,
          sf::Vector2f size,
          sf::Vector2f position)
        : tileColor(color),
          tileSize(size),
          tileStartPosition(position) {
        tileRectangle.setFillColor(tileColor);
        tileRectangle.setSize(tileSize);
        tileRectangle.setPosition(tileStartPosition);
        tileRectangleTop.setSize(sf::Vector2f(tileRectangle.getSize().x, 1.0f));
        tileRectangleTop.setPosition(sf::Vector2f(tileRectangle.getPosition().x, tileRectangle.getPosition().y));
    }

    void draw(sf::RenderWindow &window) {
        window.draw(tileRectangle);
    }

private:
    sf::Color tileColor;
    sf::Vector2f tileSize;
    sf::Vector2f tileStartPosition;
};

class Player {
public:
    Player(sf::Color color,
           float acceleration,
           float maxSpeed,
           float gravity,
           float jumpVelocity,
           float fallMultiplier,
           float jumpFallMultiplier,
           float MaxFallSpeed,
           float CoyoteTime,
           sf::Vector2f size,
           sf::Vector2f position,
           std::vector<Tile1> &tileGroup1,
           std::vector<Tile2> &tileGroup2)
        : playerColor(color),
          playerAcceleration(acceleration),
          playerMaxSpeed(maxSpeed),
          playerGravity(gravity),
          playerJumpVelocity(jumpVelocity),
          playerFallMultiplier(fallMultiplier),
          playerJumpFallMultiplier(jumpFallMultiplier),
          playerMaxFallSpeed(MaxFallSpeed),
          playerCoyoteTime(CoyoteTime),
          playerSize(size),
          playerStartPosition(position),
          playerTileGroup1(tileGroup1),
          playerTileGroup2(tileGroup2),
          playerIsGrounded(false),
          playerCoyoteTimeTimer(0.0f),
          playerDirection(sf::Vector2f(0.0f, 0.0f)) {
        playerRectangle.setFillColor(playerColor);
        playerRectangle.setSize(playerSize);
        playerRectangle.setPosition(playerStartPosition);
        playerRectangleBottom.setSize(sf::Vector2f(playerRectangle.getSize().x, 1.0f));
        playerRectangleBottom.setPosition(sf::Vector2f(playerRectangle.getPosition().x,
                                                       playerRectangle.getPosition().y + playerRectangle.getSize().y - 1.0f));
    }

    void update(sf::RenderWindow &window, float deltaTime) {
        playerRectangleBottom.setPosition(sf::Vector2f(playerRectangle.getPosition().x,
                                                       playerRectangle.getPosition().y + playerRectangle.getSize().y - 1.0f));

        horizontalMovement(deltaTime);
        horizontalCollisions();

        verticalMovement(deltaTime);
        verticalCollisions();

        camera(window);
    }

    void draw(sf::RenderWindow &window) {
        window.draw(playerRectangle);
    }

private:
    sf::Color playerColor;
    float playerAcceleration;
    float playerMaxSpeed;
    float playerGravity;
    float playerJumpVelocity;
    float playerFallMultiplier;
    float playerJumpFallMultiplier;
    float playerMaxFallSpeed;
    float playerCoyoteTime;
    sf::Vector2f playerSize;
    sf::Vector2f playerStartPosition;
    std::vector<Tile1> playerTileGroup1;
    std::vector<Tile2> playerTileGroup2;

    bool playerIsGrounded;
    float playerCoyoteTimeTimer;
    sf::Vector2f playerDirection;
    sf::RectangleShape playerRectangle;
    sf::RectangleShape playerRectangleBottom;

    void horizontalMovement(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (playerDirection.x > 0.0f) {
                playerDirection.x -= playerAcceleration * deltaTime;
            } else if (playerDirection.x < 0.0f) {
                playerDirection.x += playerAcceleration * deltaTime;
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            playerDirection.x -= playerAcceleration * deltaTime;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            playerDirection.x += playerAcceleration * deltaTime;
        } else {
            if (playerDirection.x > 0.0f) {
                playerDirection.x -= playerAcceleration * deltaTime;
                if (playerDirection.x < 0.0f) {
                    playerDirection.x = 0.0f;
                }
            } else if (playerDirection.x < 0.0f) {
                playerDirection.x += playerAcceleration * deltaTime;
                if (playerDirection.x > 0.0f) {
                    playerDirection.x = 0.0f;
                }
            }
        }

        if (playerDirection.x > playerMaxSpeed) {
            playerDirection.x = playerMaxSpeed;
        } else if (playerDirection.x < -playerMaxSpeed) {
            playerDirection.x = -playerMaxSpeed;
        }

        playerRectangle.move(sf::Vector2f(playerDirection.x * deltaTime, 0.0f));
    }

    void horizontalCollisions() {
        for (auto &tile : playerTileGroup1) {
            if (playerRectangle.getGlobalBounds().intersects(tile.tileRectangle.getGlobalBounds())) {
                if (playerDirection.x > 0.0f) {
                    playerDirection.x = 0.0f;
                    playerRectangle.setPosition(sf::Vector2f(
                        tile.tileRectangle.getGlobalBounds().left - playerRectangle.getSize().x,
                        playerRectangle.getPosition().y));
                } else if (playerDirection.x < 0.0f) {
                    playerDirection.x = 0.0f;
                    playerRectangle.setPosition(sf::Vector2f(
                        tile.tileRectangle.getGlobalBounds().left + tile.tileRectangle.getSize().x,
                        playerRectangle.getPosition().y));
                }
            }
        }
    }

    void verticalMovement(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && playerCoyoteTimeTimer > 0.0f) {
            playerDirection.y = playerJumpVelocity;
            playerCoyoteTimeTimer = 0.0f;
            playerIsGrounded = false;
        } else if (playerDirection.y > 0.0f && playerIsGrounded) {
            playerIsGrounded = false;
        }

        if (playerIsGrounded) {
            playerCoyoteTimeTimer = playerCoyoteTime;
        } else {
            playerCoyoteTimeTimer -= deltaTime;
        }

        if (playerDirection.y > playerMaxFallSpeed) {
            playerDirection.y = playerMaxFallSpeed;
        } else if (playerDirection.y > 0.0f) {
            playerDirection.y += playerGravity * playerFallMultiplier * deltaTime;
        } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && playerDirection.y < 0.0f) {
            playerDirection.y += playerGravity * playerJumpFallMultiplier * deltaTime;
        } else {
            playerDirection.y += playerGravity * deltaTime;
        }

        playerRectangle.move(sf::Vector2f(0, playerDirection.y * deltaTime));
    }

    void verticalCollisions() {
        for (auto &tile : playerTileGroup1) {
            if (playerRectangle.getGlobalBounds().intersects(tile.tileRectangle.getGlobalBounds())) {
                if (playerDirection.y > 0.0f) {
                    playerIsGrounded = true;
                    playerDirection.y = 0.0f;
                    playerRectangle.setPosition(sf::Vector2f(
                        playerRectangle.getPosition().x,
                        tile.tileRectangle.getGlobalBounds().top - playerRectangle.getSize().y));
                } else if (playerDirection.y < 0.0f) {
                    playerDirection.y = 0.0f;
                    playerRectangle.setPosition(sf::Vector2f(
                        playerRectangle.getPosition().x,
                        tile.tileRectangle.getGlobalBounds().top + tile.tileRectangle.getSize().y));
                }
            }
        }

        for (auto &tile : playerTileGroup2) {
            if (playerRectangleBottom.getGlobalBounds().intersects(tile.tileRectangleTop.getGlobalBounds())) {
                if (playerDirection.y > 0.0f && !sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    playerIsGrounded = true;
                    playerDirection.y = 0.0f;
                    playerRectangle.setPosition(sf::Vector2f(
                        playerRectangle.getPosition().x,
                        tile.tileRectangle.getGlobalBounds().top - playerRectangle.getSize().y));
                }
            }
        }
    }

    void followCamera(float deadZone, sf::RenderWindow &window) {
        if (playerRectangle.getPosition().x + playerRectangle.getSize().x > window.getView().getCenter().x + deadZone) {
            window.setView(sf::View(sf::FloatRect(
                playerRectangle.getPosition().x + playerRectangle.getSize().x - window.getSize().x / 2.0f - deadZone,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRectangle.getPosition().x < window.getView().getCenter().x - deadZone) {
            window.setView(sf::View(sf::FloatRect(
                playerRectangle.getPosition().x - window.getSize().x / 2.0f + deadZone,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }

        if (playerRectangle.getPosition().y + playerRectangle.getSize().y > window.getView().getCenter().y + deadZone) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                playerRectangle.getPosition().y + playerRectangle.getSize().y - window.getSize().y / 2.0f - deadZone,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRectangle.getPosition().y < window.getView().getCenter().y - deadZone) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                playerRectangle.getPosition().y - window.getSize().y / 2.0f + deadZone,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void stationaryCamera(sf::RenderWindow &window) {
        if (playerRectangle.getPosition().x + playerRectangle.getSize().x / 2.0f <
            window.getView().getCenter().x - window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRectangle.getPosition().x + playerRectangle.getSize().x / 2.0f >
                   window.getView().getCenter().x + window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRectangle.getPosition().y + playerRectangle.getSize().y / 2.0f <
                   window.getView().getCenter().y - window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRectangle.getPosition().y + playerRectangle.getSize().y / 2.0f >
                   window.getView().getCenter().y + window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void horizontalCamera(sf::RenderWindow &window) {
        if (playerRectangle.getPosition().y + playerRectangle.getSize().y / 2.0f <
            window.getView().getCenter().y - window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRectangle.getPosition().y + playerRectangle.getSize().y / 2.0f >
                   window.getView().getCenter().y + window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        } else {
            window.setView(sf::View(sf::FloatRect(
                playerRectangle.getPosition().x + playerRectangle.getSize().x / 2.0f - window.getSize().x / 2.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void verticalCamera(sf::RenderWindow &window) {
        if (playerRectangle.getPosition().x + playerRectangle.getSize().x / 2.0f <
            window.getView().getCenter().x - window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (playerRectangle.getPosition().x + playerRectangle.getSize().x / 2.0f >
                   window.getView().getCenter().x + window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                playerRectangle.getPosition().y + playerRectangle.getSize().y / 2.0f - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void camera(sf::RenderWindow &window) {
        horizontalCamera(window);
    }
};

void loadLevel(std::string map, float &playerPositionX, float &playerPositionY, std::vector<Tile1> &tileGroup1, std::vector<Tile2> &tileGroup2) {
    std::ifstream file(map);
    std::string line;

    float x;
    float y;

    if (file.is_open()) {
        float row_index = 0.0f;
        while (std::getline(file, line)) {
            for (float collom_index = 0.0f; collom_index < line.length(); collom_index++) {
                x = collom_index * 36.0f;
                y = row_index * 36.0f;
                if (line[collom_index] == '1') { // 1 - normal tile
                    tileGroup1.push_back(Tile1(
                        sf::Color(0, 0, 0),
                        sf::Vector2f(36.0f, 36.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == '2') { // 2 - small tile
                    tileGroup1.push_back(Tile1(
                        sf::Color(0, 0, 0),
                        sf::Vector2f(36.0f, 9.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == '3') { // 3 - one way tile
                    tileGroup2.push_back(Tile2(
                        sf::Color(139,69,19),
                        sf::Vector2f(36.0f, 9.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == 'p') { // p - player
                    playerPositionX = x;
                    playerPositionY = y;
                }
            }
            row_index++;
        }
    } else {
        std::cout << "error: can't open 'map.txt'\n";
        exit(1);
    }
}

int main() {
    std::string windowTitle = "2d platformer";
    int windowWidth = 1260;
    int windowHeight = 900;

    int windowPositionX = sf::VideoMode::getDesktopMode().width / 2 -
                          windowWidth / 2;
    int windowPositionY = sf::VideoMode::getDesktopMode().height / 2 -
                          windowHeight / 2;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight),
                            windowTitle,
                            sf::Style::Close);
    window.setPosition(sf::Vector2i(windowPositionX, windowPositionY));

    sf::Event event;

    sf::Clock clock;
    float deltaTime;

    std::vector<Tile1> tileGroup1;
    std::vector<Tile2> tileGroup2;

    float playerPositionX;
    float playerPositionY;

    loadLevel(std::string("map.txt"), playerPositionX, playerPositionY, tileGroup1, tileGroup2);

    Player player(
        sf::Color::White,                               // player color
        7675.0f,                                        // player acceleration
        405.0f,                                         // player max speed
        2175.0f,                                        // player gravity
        -850.0f,                                        // player jump velocity
        3.0f,                                           // player fall multiplier
        5.0f,                                           // player jump fall multiplier
        1085.0f,                                        // player max fall speed
        0.1f,                                           // player coyote time
        sf::Vector2f(36.0f, 72.0f),                     // player size
        sf::Vector2f(playerPositionX, playerPositionY), // player start position
        tileGroup1,
        tileGroup2);

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            }
        }

        deltaTime = clock.restart().asSeconds();

        player.update(window, deltaTime);

        window.clear(sf::Color(64, 64, 64, 255));

        for (auto &tile : tileGroup1) {
            tile.draw(window);
        }

        for (auto &tile : tileGroup2) {
            tile.draw(window);
        }

        player.draw(window);

        window.display();
    }
    return 0;
}
