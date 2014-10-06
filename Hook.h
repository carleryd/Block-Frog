//
//  Hook.h
//  Block Frog
//
//  Created by roflmao on 2014-09-29.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#ifndef __Block_Frog__Hook__
#define __Block_Frog__Hook__

#include <iostream>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

class Circle;
class Rectangle;
class Game;
class Utility;
class ContactListener;

class Hook
{
public:
    Hook(Game* game);
//    ~Hook();
    
    void draw();
    void update();
    void use(sf::Vector2i mousePixelPos);
    void aim(sf::Vector2i mousePixelPos);
    void release();
    b2WeldJoint* grab(b2Body* box);
    
    float getLength();
    void changeLength(float delta);
    
private:
    Game* game;
    Circle* hookTip;
    Rectangle* hookBase;
    Utility* utility;
	ContactListener* contactListener;
    b2PrismaticJoint* prismaticJoint;
    b2RevoluteJoint* revoluteJoint;
    b2WeldJoint* weldJoint;
    b2Vec2 playerMeterPos;
    float newMouseAngle;
    float oldMouseAngle;
    // This is the length the hook is striving towards having
    float passiveLength;
    // This is the length we want to strive towards when using hook
    float useLength;
};

#endif /* defined(__Block_Frog__Hook__) */
