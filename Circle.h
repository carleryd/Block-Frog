#ifndef __Block_Frog__Circle__
#define __Block_Frog__Circle__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Shape.h"

class Circle: public Shape
{
public:
	Circle(Game* game, float radius, b2Vec2* position,
              bool dynamic, int id, float density = 1.0f, float friction = 0.3f, int groupIndex = 1);
	~Circle();
    
    float getRadius();
private:
    float radius;
};

#endif /* defined(__Block_Frog__Circle__) */
