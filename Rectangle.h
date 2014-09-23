#ifndef __Block_Frog__Square__
#define __Block_Frog__Square__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Shape.h"

class Rectangle: public Shape
{
public:
	Rectangle(Game* game, b2Vec2* recSize, b2Vec2* recPosition,
		bool recDynamic, float recDensity = 1.0f, float recFriction = 0.3f);
	~Rectangle();
};

#endif /* defined(__Block_Frog__Square__) */
