#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Tile {
public:
    sf::RectangleShape tileRectangle;
    Tile(sf::Color color,
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
           float leftX,
           float leftY,
           float rightX,
           float rightY,
           float topX,
           float topY,
           float bottomX,
           float bottomY,
           std::vector<Tile> &tileGroup)
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
          cameraLeftX(leftX),
          cameraLeftY(leftY),
          cameraRightX(rightX),
          cameraRightY(rightY),
          cameraTopX(topX),
          cameraTopY(topY),
          cameraBottomX(bottomX),
          cameraBottomY(bottomY),
          playerTileGroup(tileGroup),
          playerIsGrounded(false),
          playerCoyoteTimeTimer(0.0f),
          playerDirection(sf::Vector2f(0.0f, 0.0f)) {
        playerRectangle.setFillColor(playerColor);
        playerRectangle.setSize(playerSize);
        playerRectangle.setPosition(playerStartPosition);
    }

    void update(sf::RenderWindow &window, float deltaTime) {
        horizontalMovement(deltaTime);
        horizontalCollisions();

        verticalMovement(deltaTime);
        verticalCollisions();

        camera(window);
    }

    void draw(sf::RenderWindow &window) {
        window.draw(playerRectangle);

        for (auto &tile : playerTileGroup) {
            tile.draw(window);
        }
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
    float cameraLeftX;
    float cameraLeftY;
    float cameraRightX;
    float cameraRightY;
    float cameraTopX;
    float cameraTopY;
    float cameraBottomX;
    float cameraBottomY;
    std::vector<Tile> playerTileGroup;

    bool playerIsGrounded;
    float playerCoyoteTimeTimer;
    sf::Vector2f playerDirection;
    sf::RectangleShape playerRectangle;

    void load(std::string map,
              float &leftX,
              float &leftY,
              float &rightX,
              float &rightY,
              float &topX,
              float &topY,
              float &bottomX,
              float &bottomY,
              std::vector<Tile> &tileGroup) {
        std::ifstream file(map);
        std::string line;

        float x;
        float y;

        tileGroup.clear();

        if (file.is_open()) {
            float row_index = 0.0f;
            while (std::getline(file, line)) {
                for (float collom_index = 0.0f; collom_index < line.length(); collom_index++) {
                    x = collom_index * 36.0f;
                    y = row_index * 36.0f;
                    if (line[collom_index] == 'p') { // p - player
                        playerRectangle.setPosition(sf::Vector2f(x, y));
                    } else if (line[collom_index] == 'l') { // l - left
                        leftX = x;
                        leftY = y;
                        tileGroup.push_back(Tile(
                            sf::Color::Black,
                            sf::Vector2f(36.0f, 36.0f),
                            sf::Vector2f(x, y)));
                    } else if (line[collom_index] == 'r') { // r - right
                        rightX = x;
                        rightY = y;
                        tileGroup.push_back(Tile(
                            sf::Color::Black,
                            sf::Vector2f(36.0f, 36.0f),
                            sf::Vector2f(x, y)));
                    } else if (line[collom_index] == 't') { // t - top
                        topX = x;
                        topY = y;
                        tileGroup.push_back(Tile(
                            sf::Color::Black,
                            sf::Vector2f(36.0f, 36.0f),
                            sf::Vector2f(x, y)));
                    } else if (line[collom_index] == 'b') { // b - bottom
                        bottomX = x;
                        bottomY = y;
                        tileGroup.push_back(Tile(
                            sf::Color::Black,
                            sf::Vector2f(36.0f, 36.0f),
                            sf::Vector2f(x, y)));
                    } else if (line[collom_index] == '1') { // 1 - tile
                        tileGroup.push_back(Tile(
                            sf::Color::Black,
                            sf::Vector2f(36.0f, 36.0f),
                            sf::Vector2f(x, y)));
                    }
                }
                row_index++;
            }
        } else {
            std::cout << "error: can't open 'map.txt'\n";
            exit(1);
        }
    }

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
            } else if (playerDirection.x < 0.0f) {
                playerDirection.x += playerAcceleration * deltaTime;
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
        for (auto &tile : playerTileGroup) {
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
        for (auto &tile : playerTileGroup) {
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
    }

    void camera(sf::RenderWindow &window) {
        if (playerRectangle.getPosition().x < cameraLeftX) {
            load(std::string("map1.txt"),
                 cameraLeftX,
                 cameraLeftY,
                 cameraRightX,
                 cameraRightY,
                 cameraTopX,
                 cameraTopY,
                 cameraBottomX,
                 cameraBottomY,
                 playerTileGroup);
        } else if (playerRectangle.getPosition().x > cameraRightX) {
            load(std::string("map2.txt"),
                 cameraLeftX,
                 cameraLeftY,
                 cameraRightX,
                 cameraRightY,
                 cameraTopX,
                 cameraTopY,
                 cameraBottomX,
                 cameraBottomY,
                 playerTileGroup);
        }

        float cameraX = playerRectangle.getPosition().x + playerRectangle.getSize().x / 2.0f - window.getSize().x / 2.0f;
        float cameraY = playerRectangle.getPosition().y + playerRectangle.getSize().y / 2.0f - window.getSize().y / 2.0f;

        if (cameraX < cameraLeftX) {
            cameraX = cameraLeftX;
        } else if (cameraX + window.getSize().x > cameraRightX + 36.0f) {
            cameraX = cameraRightX - window.getSize().x + 36.0f;
        }
        if (cameraY < cameraTopY) {
            cameraY = cameraTopY;
        } else if (cameraY + window.getSize().y > cameraBottomY + 36.0f) {
            cameraY = cameraBottomY - window.getSize().y + 36.0f;
        }

        window.setView(sf::View(sf::FloatRect(
            cameraX,
            cameraY,
            window.getSize().x,
            window.getSize().y)));
    }
};

void initLoad(std::string map,
              float &playerPositionX,
              float &playerPositionY,
              float &leftX,
              float &leftY,
              float &rightX,
              float &rightY,
              float &topX,
              float &topY,
              float &bottomX,
              float &bottomY,
              std::vector<Tile> &tileGroup) {
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
                if (line[collom_index] == 'p') { // p - player
                    playerPositionX = x;
                    playerPositionY = y;
                } else if (line[collom_index] == 'l') { // l - left
                    leftX = x;
                    leftY = y;
                    tileGroup.push_back(Tile(
                        sf::Color::Black,
                        sf::Vector2f(36.0f, 36.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == 'r') { // r - right
                    rightX = x;
                    rightY = y;
                    tileGroup.push_back(Tile(
                        sf::Color::Black,
                        sf::Vector2f(36.0f, 36.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == 't') { // t - top
                    topX = x;
                    topY = y;
                    tileGroup.push_back(Tile(
                        sf::Color::Black,
                        sf::Vector2f(36.0f, 36.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == 'b') { // b - bottom
                    bottomX = x;
                    bottomY = y;
                    tileGroup.push_back(Tile(
                        sf::Color::Black,
                        sf::Vector2f(36.0f, 36.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == '1') { // 1 - tile
                    tileGroup.push_back(Tile(
                        sf::Color::Black,
                        sf::Vector2f(36.0f, 36.0f),
                        sf::Vector2f(x, y)));
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

    std::vector<Tile> tileGroup;

    float playerPositionX;
    float playerPositionY;

    float leftX;
    float leftY;
    float rightX;
    float rightY;
    float topX;
    float topY;
    float bottomX;
    float bottomY;

    initLoad(std::string("map1.txt"),
             playerPositionX,
             playerPositionY,
             leftX,
             leftY,
             rightX,
             rightY,
             topX,
             topY,
             bottomX,
             bottomY,
             tileGroup);

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
        leftX,
        leftY,
        rightX,
        rightY,
        topX,
        topY,
        bottomX,
        bottomY,
        tileGroup);

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

        player.draw(window);

        window.display();
    }
    return 0;
}
