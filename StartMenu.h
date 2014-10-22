#ifndef __Block_Frog__StartMenu__
#define __Block_Frog__StartMenu__

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

class Shape;
class Game;
class Player;
class Textor;
class ShapeFactory;
class OSHandler;
class Utility;

class StartMenu
{
public:
    StartMenu(Game* game);
    ~StartMenu();
    
    // Setters
    void setAPressed() { APressed = true; }
    void setDPressed() { DPressed = true; }
    void setSpacePressed() { spacePressed = true; }
    void setLeftClickPressed() { leftClickPressed = true; }
    
    void run();
    
private:
    Game* game;
    b2World* world;
    sf::RenderWindow* window;
    ShapeFactory* shapeFactory;
    Player* player;
    Textor* textor;
    OSHandler* osHandler;
    Utility* utility;
    
    std::vector<Shape*> boxes;
    // These are the two boxes shown in start menu
    Shape* hostRectangle;
    Shape* joinRectangle;
    Shape* floor;
    
    // Suggested buttons
    sf::Texture textureA;
    sf::Texture textureD;
    sf::Texture textureSpace;
    sf::Texture textureLeftClick;
    sf::Sprite spriteA;
    sf::Sprite spriteD;
    sf::Sprite spriteSpace;
    sf::Sprite spriteLeftClick;
    sf::Vector2f playerPos;
    
    bool APressed;
    bool DPressed;
    bool spacePressed;
    bool leftClickPressed;
    
    enum PressState { WALK, JUMP, HOOK, WAITING };
    PressState learnState;
    
    sf::Clock clock;
    sf::Time timer;
    float secondsPassed;
};

#endif /* defined(__Block_Frog__StartMenu__) */
