#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class SolidTile
{
public:
    sf::RectangleShape rect;
    SolidTile(
        sf::Color color,
        sf::Vector2f size,
        sf::Vector2f pos)
    {
        this->color = color;
        this->pos = pos;
        this->size = size;
        rect.setFillColor(color);
        rect.setSize(size);
        rect.setPosition(pos);
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(rect);
    }

private:
    sf::Color color;
    sf::Vector2f size;
    sf::Vector2f pos;
};

class OneWayTile
{
public:
    sf::RectangleShape rect;
    sf::RectangleShape rectTop;
    OneWayTile(
        sf::Color color,
        sf::Vector2f size,
        sf::Vector2f pos)
    {
        this->color = color;
        this->pos = pos;
        this->size = size;
        rect.setFillColor(color);
        rect.setSize(size);
        rect.setPosition(pos);
        rectTop.setSize(sf::Vector2f(rect.getSize().x, 1.0f));
        rectTop.setPosition(sf::Vector2f(
            rect.getPosition().x,
            rect.getPosition().y));
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(rect);
    }

private:
    sf::Color color;
    sf::Vector2f size;
    sf::Vector2f pos;
};

class Player
{
public:
    Player(
        float acceleration,
        float maxSpeed,
        float gravity,
        float jumpVelocity,
        float fallMultiplier,
        float jumpFallMultiplier,
        float maxFallSpeed,
        float coyoteTime,
        float jumpBufferTime,
        sf::Color color,
        sf::Vector2f size,
        sf::Vector2f pos)
    {
        this->acceleration = acceleration;
        this->maxSpeed = maxSpeed;
        this->gravity = gravity;
        this->jumpVelocity = jumpVelocity;
        this->fallMultiplier = fallMultiplier;
        this->jumpFallMultiplier = jumpFallMultiplier;
        this->maxFallSpeed = maxFallSpeed;
        this->coyoteTime = coyoteTime;
        this->jumpBufferTime = jumpBufferTime;
        this->color = color;
        this->size = size;
        this->pos = pos;
        isGrounded = false;
        holdingSpace = false;
        coyoteTimeTimer = 0.0f;
        jumpBufferTimer = 0.0f;
        direction = sf::Vector2f(0.0f, 0.0f);
        rect.setFillColor(color);
        rect.setSize(size);
        rect.setPosition(pos);
        rectBottom.setSize(sf::Vector2f(rect.getSize().x, 1.0f));
        rectBottom.setPosition(sf::Vector2f(
            rect.getPosition().x,
            rect.getPosition().y + rect.getSize().y - 1.0f));
    }

    void update(
        sf::RenderWindow &window,
        float deltaTime,
        std::vector<SolidTile> &solidTileGroup,
        std::vector<OneWayTile> &oneWayTileGroup)
    {
        rectBottom.setPosition(sf::Vector2f(
            rect.getPosition().x,
            rect.getPosition().y + rect.getSize().y - 1.0f));

        CollisionOneWayTile(oneWayTileGroup);
        horizontalMovement(deltaTime);
        horizontalCollisionsSolidTile(solidTileGroup);
        verticalMovement(deltaTime);
        verticalCollisionsSolidTile(solidTileGroup);
        camera(window);
    }

    void draw(sf::RenderWindow &window)
    {
        window.draw(rect);
    }

private:
    float acceleration;
    float maxSpeed;
    float gravity;
    float jumpVelocity;
    float fallMultiplier;
    float jumpFallMultiplier;
    float maxFallSpeed;
    float coyoteTime;
    float jumpBufferTime;
    sf::Color color;
    sf::Vector2f size;
    sf::Vector2f pos;
    bool isGrounded;
    bool holdingSpace;
    float coyoteTimeTimer;
    float jumpBufferTimer;
    sf::Vector2f direction;
    sf::RectangleShape rect;
    sf::RectangleShape rectBottom;

    void CollisionOneWayTile(
        std::vector<OneWayTile> &oneWayTileGroup)
    {
        for (auto &tile : oneWayTileGroup)
        {
            if (rectBottom.getGlobalBounds().intersects(tile.rectTop.getGlobalBounds()))
            {
                if (direction.y > 0.0f &&
                    !sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                {
                    isGrounded = true;
                    direction.y = 0.0f;
                    rect.setPosition(sf::Vector2f(
                        rect.getPosition().x,
                        tile.rect.getGlobalBounds().top -
                            rect.getSize().y));
                }
            }
        }
    }

    void horizontalMovement(float deltaTime)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) &&
            sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            if (direction.x > 0.0f)
            {
                direction.x -= acceleration * deltaTime;
            }
            else if (direction.x < 0.0f)
            {
                direction.x += acceleration * deltaTime;
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            direction.x -= acceleration * deltaTime;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            direction.x += acceleration * deltaTime;
        }
        else
        {
            if (direction.x > 0.0f)
            {
                direction.x -= acceleration * deltaTime;
                if (direction.x < 0.0f)
                {
                    direction.x = 0.0f;
                }
            }
            else if (direction.x < 0.0f)
            {
                direction.x += acceleration * deltaTime;
                if (direction.x > 0.0f)
                {
                    direction.x = 0.0f;
                }
            }
        }

        if (direction.x > maxSpeed)
        {
            direction.x = maxSpeed;
        }
        else if (direction.x < -maxSpeed)
        {
            direction.x = -maxSpeed;
        }

        rect.move(sf::Vector2f(direction.x * deltaTime, 0.0f));
    }

    void horizontalCollisionsSolidTile(
        std::vector<SolidTile> &solidTileGroup)
    {
        for (auto &tile : solidTileGroup)
        {
            if (rect.getGlobalBounds().intersects(tile.rect.getGlobalBounds()))
            {
                if (direction.x > 0.0f)
                {
                    direction.x = 0.0f;
                    rect.setPosition(sf::Vector2f(
                        tile.rect.getGlobalBounds().left -
                            rect.getSize().x,
                        rect.getPosition().y));
                }
                else if (direction.x < 0.0f)
                {
                    direction.x = 0.0f;
                    rect.setPosition(sf::Vector2f(
                        tile.rect.getGlobalBounds().left +
                            tile.rect.getSize().x,
                        rect.getPosition().y));
                }
            }
        }
    }

    void verticalMovement(float deltaTime)
    {
        if (jumpBufferTimer > 0.0f && coyoteTimeTimer > 0.0f)
        {
            direction.y = jumpVelocity;
            coyoteTimeTimer = 0.0f;
            isGrounded = false;
        }
        else if (direction.y > 0.0f && isGrounded)
        {
            isGrounded = false;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) &&
            !holdingSpace)
        {
            jumpBufferTimer = jumpBufferTime;
            holdingSpace = true;
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            holdingSpace = false;
        }

        jumpBufferTimer -= deltaTime;

        if (isGrounded)
        {
            coyoteTimeTimer = coyoteTime;
        }

        coyoteTimeTimer -= deltaTime;

        if (direction.y > maxFallSpeed)
        {
            direction.y = maxFallSpeed;
        }
        else if (direction.y > 0.0f)
        {
            direction.y += gravity * fallMultiplier * deltaTime;
        }
        else if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space) &&
                 direction.y < 0.0f)
        {
            direction.y += gravity * jumpFallMultiplier * deltaTime;
        }
        else
        {
            direction.y += gravity * deltaTime;
        }

        rect.move(sf::Vector2f(0, direction.y * deltaTime));
    }

    void verticalCollisionsSolidTile(
        std::vector<SolidTile> &solidTileGroup)
    {
        for (auto &tile : solidTileGroup)
        {
            if (rect.getGlobalBounds().intersects(tile.rect.getGlobalBounds()))
            {
                if (direction.y > 0.0f)
                {
                    isGrounded = true;
                    direction.y = 0.0f;
                    rect.setPosition(sf::Vector2f(
                        rect.getPosition().x,
                        tile.rect.getGlobalBounds().top -
                            rect.getSize().y));
                }
                else if (direction.y < 0.0f)
                {
                    direction.y = 0.0f;
                    rect.setPosition(sf::Vector2f(
                        rect.getPosition().x,
                        tile.rect.getGlobalBounds().top +
                            tile.rect.getSize().y));
                }
            }
        }
    }

    void followCamera(
        float horizontalDeadZone,
        float verticalDeadZone,
        sf::RenderWindow &window)
    {
        if (rect.getPosition().x + rect.getSize().x >
            window.getView().getCenter().x + horizontalDeadZone)
        {
            window.setView(sf::View(sf::FloatRect(
                rect.getPosition().x + rect.getSize().x -
                    window.getSize().x / 2.0f -
                    horizontalDeadZone,
                window.getView().getCenter().y -
                    window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
        else if (rect.getPosition().x <
                 window.getView().getCenter().x - horizontalDeadZone)
        {
            window.setView(sf::View(sf::FloatRect(
                rect.getPosition().x -
                    window.getSize().x / 2.0f +
                    horizontalDeadZone,
                window.getView().getCenter().y -
                    window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }

        if (rect.getPosition().y + rect.getSize().y >
            window.getView().getCenter().y + verticalDeadZone)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x -
                    window.getSize().x / 2.0f,
                rect.getPosition().y +
                    rect.getSize().y -
                    window.getSize().y / 2.0f -
                    verticalDeadZone,
                window.getSize().x,
                window.getSize().y)));
        }
        else if (rect.getPosition().y <
                 window.getView().getCenter().y - verticalDeadZone)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x -
                    window.getSize().x / 2.0f,
                rect.getPosition().y -
                    window.getSize().y / 2.0f +
                    verticalDeadZone,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void stationaryCamera(sf::RenderWindow &window)
    {
        if (rect.getPosition().x + rect.getSize().x / 2.0f <
            window.getView().getCenter().x -
                window.getSize().x / 2.0f)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x -
                    window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y -
                    window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
        else if (rect.getPosition().x + rect.getSize().x / 2.0f >
                 window.getView().getCenter().x +
                     window.getSize().x / 2.0f)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x +
                    window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y +
                    window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
        else if (rect.getPosition().y + rect.getSize().y / 2.0f <
                 window.getView().getCenter().y -
                     window.getSize().y / 2.0f)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x +
                    window.getSize().x / 2.0f,
                window.getView().getCenter().y -
                    window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        }
        else if (rect.getPosition().y + rect.getSize().y / 2.0f >
                 window.getView().getCenter().y +
                     window.getSize().y / 2.0f)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x -
                    window.getSize().x / 2.0f,
                window.getView().getCenter().y +
                    window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void horizontalCamera(sf::RenderWindow &window)
    {
        if (rect.getPosition().y + rect.getSize().y / 2.0f <
            window.getView().getCenter().y -
                window.getSize().y / 2.0f)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x +
                    window.getSize().x / 2.0f,
                window.getView().getCenter().y -
                    window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        }
        else if (rect.getPosition().y + rect.getSize().y / 2.0f >
                 window.getView().getCenter().y +
                     window.getSize().y / 2.0f)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x -
                    window.getSize().x / 2.0f,
                window.getView().getCenter().y +
                    window.getSize().y / 2.0f * 3.0f,
                window.getSize().x,
                window.getSize().y)));
        }
        else
        {
            window.setView(sf::View(sf::FloatRect(
                rect.getPosition().x +
                    rect.getSize().x / 2.0f -
                    window.getSize().x / 2.0f,
                window.getView().getCenter().y -
                    window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void verticalCamera(sf::RenderWindow &window)
    {
        if (rect.getPosition().x + rect.getSize().x / 2.0f <
            window.getView().getCenter().x -
                window.getSize().x / 2.0f)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x -
                    window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y -
                    window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
        else if (rect.getPosition().x + rect.getSize().x / 2.0f >
                 window.getView().getCenter().x +
                     window.getSize().x / 2.0f)
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x +
                    window.getSize().x / 2.0f * 3.0f,
                window.getView().getCenter().y +
                    window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
        else
        {
            window.setView(sf::View(sf::FloatRect(
                window.getView().getCenter().x -
                    window.getSize().x / 2.0f,
                rect.getPosition().y +
                    rect.getSize().y / 2.0f -
                    window.getSize().y / 2.0f,
                window.getSize().x,
                window.getSize().y)));
        }
    }

    void camera(sf::RenderWindow &window)
    {
        horizontalCamera(window);
    }
};

void loadLevel(
    std::string map,
    std::vector<Player> &playerGroup,
    std::vector<SolidTile> &solidTileGroup,
    std::vector<OneWayTile> &oneWayTileGroup)
{
    std::ifstream file(map);
    std::string line;

    float x;
    float y;

    float collom_index = 0.0f;

    while (std::getline(file, line))
    {
        for (float row_index = 0.0f; row_index < line.length(); row_index++)
        {
            x = row_index * 36.0f;
            y = collom_index * 36.0f;

            if (line[row_index] == 't') // t - tile
            {
                solidTileGroup.push_back(SolidTile(
                    sf::Color(0.0f, 0.0f, 0.0f),
                    sf::Vector2f(36.0f, 36.0f),
                    sf::Vector2f(x, y)));
            }
            else if (line[row_index] == 's') // s - small tile
            {
                solidTileGroup.push_back(SolidTile(
                    sf::Color(0, 0, 0),
                    sf::Vector2f(36.0f, 9.0f),
                    sf::Vector2f(x, y)));
            }
            else if (line[row_index] == 'o') // o - one way tile
            {
                oneWayTileGroup.push_back(OneWayTile(
                    sf::Color(139.0f, 69.0f, 19.0f),
                    sf::Vector2f(36.0f, 9.0f),
                    sf::Vector2f(x, y)));
            }
            else if (line[row_index] == 'p') // p - player
            {
                playerGroup.push_back(Player(
                    7675.0f,                    // player acceleration
                    405.0f,                     // player max speed
                    2175.0f,                    // player gravity
                    -850.0f,                    // player jump velocity
                    3.0f,                       // player fall multiplier
                    5.0f,                       // player jump fall multiplier
                    1085.0f,                    // player max fall speed
                    0.1f,                       // player coyote time
                    0.12f,                      // player jump buffer time
                    sf::Color::White,           // player color
                    sf::Vector2f(36.0f, 72.0f), // player size
                    sf::Vector2f(x, y)));       // player pos
            }
        }
        collom_index++;
    }
}

int main()
{
    std::string winTitle = "sfml platformer";
    int winWidth = 1260;
    int winHeight = 900;

    sf::RenderWindow window(
        sf::VideoMode(winWidth, winHeight),
        winTitle,
        sf::Style::Close);
    window.setPosition(sf::Vector2i(
        sf::VideoMode::getDesktopMode().width / 2 - winWidth / 2,
        sf::VideoMode::getDesktopMode().height / 2 - winHeight / 2));

    std::vector<Player> playerGroup;
    std::vector<SolidTile> solidTileGroup;
    std::vector<OneWayTile> oneWayTileGroup;

    loadLevel(
        std::string("map.txt"),
        playerGroup,
        solidTileGroup,
        oneWayTileGroup);

    sf::Clock clock;
    float deltaTime;

    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                window.close();
            }
        }

        // update
        deltaTime = clock.restart().asSeconds();

        for (auto &player : playerGroup)
        {
            player.update(
                window,
                deltaTime,
                solidTileGroup,
                oneWayTileGroup);
        }

        // draw
        window.clear(sf::Color(64, 64, 64));

        for (auto &tile : solidTileGroup)
        {
            tile.draw(window);
        }

        for (auto &tile : oneWayTileGroup)
        {
            tile.draw(window);
        }

        for (auto &player : playerGroup)
        {
            player.draw(window);
        }

        window.display();
    }
    return 0;
}
