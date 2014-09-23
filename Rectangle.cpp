#include "Rectangle.h"

Rectangle::Rectangle(Game* game, b2Vec2* recSize, b2Vec2* recPosition,
					 bool recDynamic, float recDensity, float recFriction):
	Shape(game, recSize, recPosition, recDynamic, recDensity, recFriction)
{
}

Rectangle::~Rectangle()
{
	delete Shape::shape;
}