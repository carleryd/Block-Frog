#include "ShapeFactory.h"
#include "Rectangle.h"

ShapeFactory::ShapeFactory(b2World* world, sf::RenderWindow* window):
	world(world), window(window)
{
}


ShapeFactory::~ShapeFactory(void)
{

}

Shape* ShapeFactory::createRectangle(b2Vec2* size, b2Vec2* position, bool dynamic)
{
	return new Rectangle(world, size, position, window, dynamic);
}