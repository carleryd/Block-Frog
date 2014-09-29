#ifndef __Block_Frog__Player__
#define __Block_Frog__Player__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Rectangle.h"
#include "Circle.h"
#include "ContactListener.h"
class Game;
using namespace std;

class Player
{
public:
	Player(Game* game);

	void setName(string n);    
	string& getName(){return name;};
    void setPosition(b2Vec2* newPos);
	b2Vec2* getPosition() const{return box->getPosition();};
	enum { LEFT, RIGHT, JUMP, LEFT_STOP, RIGHT_STOP };
	void move(int dir);
	/*
	*	Pushes player in the direction
	*/
	void push(b2Vec2&& direction);
    void draw();
	void updatePlayer();
    void increaseHook();
    void decreaseHook();
    bool isJumping();
    //Rectangle* box;
    //Circle* hookTip;
private:
    Game* game;
    ContactListener* contactListener;
    sf::Sprite frogSprite;
    sf::Texture frogTexture;
	Rectangle* box;
    Circle* hookTip;
    
    b2Joint* hook;
    
	float leftSpeed, rightSpeed;
	float jumpHeight;
    bool jumping;
	string name;
};

#endif /* defined(__Block_Frog__Player__) */
