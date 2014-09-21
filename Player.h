//
//  Player.h
//  Block Frog
//
//  Created by roflmao on 2014-09-05.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

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
    void draw(sf::RenderWindow* window);
	void updatePlayer();
    bool isJumping();
    Rectangle* box;
private:
    b2World* world;
    sf::RenderWindow* window;
    Game* game;
    ContactListener* contactListener;
    sf::Sprite frogSprite;
    sf::Texture frogTexture;
    
	float leftSpeed, rightSpeed;
	float jumpHeight;
    bool jumping;
};

#endif /* defined(__Block_Frog__Player__) */
