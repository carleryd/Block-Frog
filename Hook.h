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

class Hook
{
public:
    Hook(Game* game);
//    ~Hook();
    
    void draw();
    void update();
    void use(sf::Vector2i mousePixelPos);
    void aim(sf::Vector2i mousePixelPos);
    
    float getLength();
    void changeLength(float delta);
    
private:
    Game* game;
    Circle* hookTip;
    Rectangle* hookBase;
    Utility* utility;
    b2PrismaticJoint* prismaticJoint;
    b2RevoluteJoint* revoluteJoint;
    b2Vec2 playerMeterPos;
    float newMouseAngle;
    float oldMouseAngle;
};

#endif /* defined(__Block_Frog__Hook__) */
