#ifndef __Block_Frog__Player__
#define __Block_Frog__Player__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Rectangle.h"
#include "Circle.h"
#include "ContactListener.h"

class Game;
class Hook;

using namespace std;

class Player
{
public:
	Player(Game* game);
    void init();
    
    b2Body* getBody();

	void setName(string n);
    void setPosition(b2Vec2* newPos);
    
	string& getName(){return name;};
	b2Vec2* getPosition() const{return box->getPosition();};
    
	enum { LEFT, RIGHT, JUMP, LEFT_STOP, RIGHT_STOP };
	//set bool to true when moving a remote player
	void move(int dir, bool localPlayer=true);
	/*
	*	Pushes player in the direction
	*/
	void push(b2Vec2&& direction);
    void draw();
	void update();
    void useHook(sf::Vector2i mousePos);
    void aimHook(sf::Vector2i mousePos);
    void increaseHook();
    void decreaseHook();
    bool isJumping();
    
private:
    Game* game;
    Hook* hook;
    Rectangle* box;
    ContactListener* contactListener;
    sf::Sprite frogSprite;
    sf::Texture frogTexture;
    Circle* hookTip;
    
	float leftSpeed, rightSpeed;
	float jumpHeight;
    float movementSpeed;
    bool jumping;
	string name;
};

#endif /* defined(__Block_Frog__Player__) */
