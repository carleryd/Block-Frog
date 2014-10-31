#ifndef __Block_Frog__Square__
#define __Block_Frog__Square__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Shape.h"

class Rectangle: public Shape
{
public:
	Rectangle(Game* game, b2Vec2* size, b2Vec2* position,
		bool dynamic, int id, float density = 1.0f, float friction = 1.0f, int groupIndex = 1);
	~Rectangle();

  	b2Vec2* getSize() const;
private:
    b2Vec2* size;
    
    sf::Texture rockTexture;
};

#endif /* defined(__Block_Frog__Square__) */
