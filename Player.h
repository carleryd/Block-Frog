#ifndef __Block_Frog__Player__
#define __Block_Frog__Player__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Rectangle.h"
#include "ContactListener.h"
class Game;

class Player
{
public:
	Player(Game* game);
    
    void setPosition(b2Vec2* newPos);
	enum { LEFT, RIGHT, JUMP, LEFT_STOP, RIGHT_STOP };
	void move(int dir);
	/*
	*	Pushes player in the direction
	*/
	void push(b2Vec2&& direction);
    void draw();
	void updatePlayer();
    bool isJumping();
    Rectangle* box;
    Rectangle* hookTip;
private:
    Game* game;
    ContactListener* contactListener;
    sf::Sprite frogSprite;
    sf::Texture frogTexture;
    
	float leftSpeed, rightSpeed;
	float jumpHeight;
    bool jumping;
};

#endif /* defined(__Block_Frog__Player__) */
