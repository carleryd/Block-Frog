//
//  Square.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-07.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Rectangle.h"

Rectangle::Rectangle(b2World* world, b2Vec2* recSize, b2Vec2* recPosition, sf::RenderWindow* w,
					 bool recDynamic, float recDensity, float recFriction):
	Shape(world, recSize, recPosition, recDynamic, recDensity, recFriction, w)
{
}

Rectangle::~Rectangle()
{
	delete Shape::shape;
}