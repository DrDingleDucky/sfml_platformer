#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class MainTile {
public:
    sf::RectangleShape m_rectangle;
    MainTile(sf::Color color,
          sf::Vector2f size,
          sf::Vector2f position)
        : m_color(color),
          m_size(size),
          m_startPosition(position) {
        m_rectangle.setFillColor(m_color);
        m_rectangle.setSize(m_size);
        m_rectangle.setPosition(m_startPosition);
    }

    void draw(sf::RenderWindow &window) {
        window.draw(m_rectangle);
    }

private:
    sf::Color m_color;
    sf::Vector2f m_size;
    sf::Vector2f m_startPosition;
};

class OneWayTile {
public:
    sf::RectangleShape m_rectangle;
    sf::RectangleShape m_rectangleTop;
    OneWayTile(sf::Color color,
          sf::Vector2f size,
          sf::Vector2f position)
        : m_color(color),
          m_size(size),
          m_startPosition(position) {
        m_rectangle.setFillColor(m_color);
        m_rectangle.setSize(m_size);
        m_rectangle.setPosition(m_startPosition);
        m_rectangleTop.setSize(sf::Vector2f(m_rectangle.getSize().x, 1.0f));
        m_rectangleTop.setPosition(sf::Vector2f(m_rectangle.getPosition().x,
                                                m_rectangle.getPosition().y));
    }

    void draw(sf::RenderWindow &window) {
        window.draw(m_rectangle);
    }

private:
    sf::Color m_color;
    sf::Vector2f m_size;
    sf::Vector2f m_startPosition;
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
           float JumpBufferTime,
           sf::Vector2f size,
           sf::Vector2f position,
           std::vector<MainTile> &tileGroup1,
           std::vector<OneWayTile> &tileGroup2)
        : m_color(color),
          m_acceleration(acceleration),
          m_maxSpeed(maxSpeed),
          m_gravity(gravity),
          m_jumpVelocity(jumpVelocity),
          m_fallMultiplier(fallMultiplier),
          m_jumpFallMultiplier(jumpFallMultiplier),
          m_maxFallSpeed(MaxFallSpeed),
          m_coyoteTime(CoyoteTime),
          m_jumpBufferTime(JumpBufferTime),
          m_size(size),
          m_startPosition(position),
          m_tileGroup1(tileGroup1),
          m_tileGroup2(tileGroup2),
          m_isGrounded(false),
          m_holdingSpace(false),
          m_coyoteTimeTimer(0.0f),
          m_jumpBufferTimer(0.0f),
          m_direction(sf::Vector2f(0.0f, 0.0f)) {
        m_rectangle.setFillColor(m_color);
        m_rectangle.setSize(m_size);
        m_rectangle.setPosition(m_startPosition);
        m_rectangleBottom.setSize(sf::Vector2f(m_rectangle.getSize().x, 1.0f));
        m_rectangleBottom.setPosition(sf::Vector2f(m_rectangle.getPosition().x,
                                                   m_rectangle.getPosition().y +
                                                       m_rectangle.getSize().y - 1.0f));
    }

    void update(sf::RenderWindow &window, float deltaTime) {
        m_rectangleBottom.setPosition(sf::Vector2f(m_rectangle.getPosition().x,
                                                   m_rectangle.getPosition().y +
                                                       m_rectangle.getSize().y - 1.0f));
        collisionTileGroup2();

        horizontalMovement(deltaTime);
        horizontalCollisionsTileGroup1();

        verticalMovement(deltaTime);
        verticalCollisionsTileGroup1();

        camera(window);
    }

    void draw(sf::RenderWindow &window) {
        window.draw(m_rectangle);
    }

private:
    sf::Color m_color;
    float m_acceleration;
    float m_maxSpeed;
    float m_gravity;
    float m_jumpVelocity;
    float m_fallMultiplier;
    float m_jumpFallMultiplier;
    float m_maxFallSpeed;
    float m_coyoteTime;
    float m_jumpBufferTime;
    sf::Vector2f m_size;
    sf::Vector2f m_startPosition;
    std::vector<MainTile> m_tileGroup1;
    std::vector<OneWayTile> m_tileGroup2;

    bool m_isGrounded;
    bool m_holdingSpace;
    float m_coyoteTimeTimer;
    float m_jumpBufferTimer;
    sf::Vector2f m_direction;
    sf::RectangleShape m_rectangle;
    sf::RectangleShape m_rectangleBottom;

    void collisionTileGroup2() {
        for (auto &tile : m_tileGroup2) {
            if (m_rectangleBottom.getGlobalBounds().intersects(tile.m_rectangleTop.getGlobalBounds())) {
                if (m_direction.y > 0.0f && !sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    m_isGrounded = true;
                    m_direction.y = 0.0f;
                    m_rectangle.setPosition(sf::Vector2f(
                        m_rectangle.getPosition().x,
                        tile.m_rectangle.getGlobalBounds().top - m_rectangle.getSize().y));
                }
            }
        }
    }

    void horizontalMovement(float deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (m_direction.x > 0.0f) {
                m_direction.x -= m_acceleration * deltaTime;
            } else if (m_direction.x < 0.0f) {
                m_direction.x += m_acceleration * deltaTime;
            }
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            m_direction.x -= m_acceleration * deltaTime;
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            m_direction.x += m_acceleration * deltaTime;
        } else {
            if (m_direction.x > 0.0f) {
                m_direction.x -= m_acceleration * deltaTime;
                if (m_direction.x < 0.0f) {
                    m_direction.x = 0.0f;
                }
            } else if (m_direction.x < 0.0f) {
                m_direction.x += m_acceleration * deltaTime;
                if (m_direction.x > 0.0f) {
                    m_direction.x = 0.0f;
                }
            }
        }

        if (m_direction.x > m_maxSpeed) {
            m_direction.x = m_maxSpeed;
        } else if (m_direction.x < -m_maxSpeed) {
            m_direction.x = -m_maxSpeed;
        }

        m_rectangle.move(sf::Vector2f(m_direction.x * deltaTime, 0.0f));
    }

    void horizontalCollisionsTileGroup1() {
        for (auto &tile : m_tileGroup1) {
            if (m_rectangle.getGlobalBounds().intersects(tile.m_rectangle.getGlobalBounds())) {
                if (m_direction.x > 0.0f) {
                    m_direction.x = 0.0f;
                    m_rectangle.setPosition(sf::Vector2f(
                        tile.m_rectangle.getGlobalBounds().left - m_rectangle.getSize().x,
                        m_rectangle.getPosition().y));
                } else if (m_direction.x < 0.0f) {
                    m_direction.x = 0.0f;
                    m_rectangle.setPosition(sf::Vector2f(
                        tile.m_rectangle.getGlobalBounds().left + tile.m_rectangle.getSize().x,
                        m_rectangle.getPosition().y));
                }
            }
        }
    }

    void verticalMovement(float deltaTime) {
        if (m_jumpBufferTimer > 0.0f && m_coyoteTimeTimer > 0.0f) {
            m_direction.y = m_jumpVelocity;
            m_coyoteTimeTimer = 0.0f;
            m_isGrounded = false;
        } else if (m_direction.y > 0.0f && m_isGrounded) {
            m_isGrounded = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !m_holdingSpace) {
            m_jumpBufferTimer = m_jumpBufferTime;
            m_holdingSpace = true;
        } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            m_holdingSpace = false;
        }

        m_jumpBufferTimer -= deltaTime;

        if (m_isGrounded) {
            m_coyoteTimeTimer = m_coyoteTime;
        }

        m_coyoteTimeTimer -= deltaTime;

        if (m_direction.y > m_maxFallSpeed) {
            m_direction.y = m_maxFallSpeed;
        } else if (m_direction.y > 0.0f) {
            m_direction.y += m_gravity * m_fallMultiplier * deltaTime;
        } else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_direction.y < 0.0f) {
            m_direction.y += m_gravity * m_jumpFallMultiplier * deltaTime;
        } else {
            m_direction.y += m_gravity * deltaTime;
        }

        m_rectangle.move(sf::Vector2f(0, m_direction.y * deltaTime));
    }

    void verticalCollisionsTileGroup1() {
        for (auto &tile : m_tileGroup1) {
            if (m_rectangle.getGlobalBounds().intersects(tile.m_rectangle.getGlobalBounds())) {
                if (m_direction.y > 0.0f) {
                    m_isGrounded = true;
                    m_direction.y = 0.0f;
                    m_rectangle.setPosition(sf::Vector2f(
                        m_rectangle.getPosition().x,
                        tile.m_rectangle.getGlobalBounds().top - m_rectangle.getSize().y));
                } else if (m_direction.y < 0.0f) {
                    m_direction.y = 0.0f;
                    m_rectangle.setPosition(sf::Vector2f(
                        m_rectangle.getPosition().x,
                        tile.m_rectangle.getGlobalBounds().top + tile.m_rectangle.getSize().y));
                }
            }
        }
    }

    void followCamera(float deadZone, sf::RenderWindow &window) {
        if (m_rectangle.getPosition().x + m_rectangle.getSize().x >
            window.getView().getCenter().x + deadZone) {
            window.setView(sf::View(sf::FloatRect(
                m_rectangle.getPosition().x + m_rectangle.getSize().x - window.getSize().x / 2.0f - deadZone,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (m_rectangle.getPosition().x < window.getView().getCenter().x - deadZone) {
            window.setView(sf::View(sf::FloatRect(
                m_rectangle.getPosition().x - window.getSize().x / 2.0f + deadZone,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }

        if (m_rectangle.getPosition().y + m_rectangle.getSize().y >
            window.getView().getCenter().y + deadZone) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                m_rectangle.getPosition().y + m_rectangle.getSize().y - window.getSize().y / 2.0f - deadZone,
                window.getSize().x,
                window.getSize().y)));
        } else if (m_rectangle.getPosition().y < window.getView().getCenter().y - deadZone) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                m_rectangle.getPosition().y - window.getSize().y / 2.0f + deadZone,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void stationaryCamera(sf::RenderWindow &window) {
        if (m_rectangle.getPosition().x + m_rectangle.getSize().x / 2.0f <
            window.getView().getCenter().x - window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (m_rectangle.getPosition().x + m_rectangle.getSize().x / 2.0f >
                   window.getView().getCenter().x + window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (m_rectangle.getPosition().y + m_rectangle.getSize().y / 2.0f <
                   window.getView().getCenter().y - window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (m_rectangle.getPosition().y + m_rectangle.getSize().y / 2.0f >
                   window.getView().getCenter().y + window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void horizontalCamera(sf::RenderWindow &window) {
        if (m_rectangle.getPosition().y + m_rectangle.getSize().y / 2.0f <
            window.getView().getCenter().y - window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (m_rectangle.getPosition().y + m_rectangle.getSize().y / 2.0f >
                   window.getView().getCenter().y + window.getSize().y / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        } else {
            window.setView(sf::View(sf::FloatRect(
                m_rectangle.getPosition().x + m_rectangle.getSize().x / 2.0f - window.getSize().x / 2.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void verticalCamera(sf::RenderWindow &window) {
        if (m_rectangle.getPosition().x + m_rectangle.getSize().x / 2.0f <
            window.getView().getCenter().x - window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else if (m_rectangle.getPosition().x + m_rectangle.getSize().x / 2.0f >
                   window.getView().getCenter().x + window.getSize().x / 2.0f) {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x + window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y + window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        } else {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x - window.getSize().x / 2.0f,
                m_rectangle.getPosition().y + m_rectangle.getSize().y / 2.0f - window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void camera(sf::RenderWindow &window) {
        horizontalCamera(window);
    }
};

void loadLevel(std::string map,
               float &playerPositionX,
               float &playerPositionY,
               std::vector<MainTile> &tileGroup1,
               std::vector<OneWayTile> &tileGroup2) {
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
                    tileGroup1.push_back(MainTile(
                        sf::Color(0.0f, 0.0f, 0.0f),
                        sf::Vector2f(36.0f, 36.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == '2') { // 2 - small tile
                    tileGroup1.push_back(MainTile(
                        sf::Color(0, 0, 0),
                        sf::Vector2f(36.0f, 9.0f),
                        sf::Vector2f(x, y)));
                } else if (line[collom_index] == '3') { // 3 - one way tile
                    tileGroup2.push_back(OneWayTile(
                        sf::Color(139.0f, 69.0f, 19.0f),
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

    std::vector<MainTile> tileGroup1;
    std::vector<OneWayTile> tileGroup2;

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
        0.12f,                                          // player jump buffer time
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
