//
//  Square.h
//  Block Frog
//
//  Created by roflmao on 2014-09-07.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#ifndef __Block_Frog__Square__
#define __Block_Frog__Square__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

class Rectangle
{
public:
    Rectangle(b2World* world, b2Vec2* recSize, b2Vec2* recPosition, bool recDynamic, float recDensity = 1.0f, float recFriction = 0.3f);
    void update();
    sf::RectangleShape* getShape();
    b2Body* getBody();
    
private:
    sf::RectangleShape* shape;
    b2Body* body;
    float pixelToMeter, meterToPixel, offSetX, offSetY;
    float density, friction;
    bool dynamic;
    b2Vec2* size;
    b2Vec2* position;
    float angle;
};

#endif /* defined(__Block_Frog__Square__) */
