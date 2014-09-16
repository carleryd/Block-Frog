#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Box2D\Box2D.h>
#include "Shape.h"



class ShapeFactory
{
public:
	ShapeFactory(b2World* world, sf::RenderWindow* window);
	~ShapeFactory(void);

	Shape* createRectangle(b2Vec2* size, b2Vec2* position, bool dynamic);

private:
	b2World* world;
	sf::RenderWindow* window;
};

#endif