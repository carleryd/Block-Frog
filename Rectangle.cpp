#include "Rectangle.h"
#include "Game.h"
#include "Utility.h"

Rectangle::Rectangle(Game* game, b2Vec2* size_, b2Vec2* position,
					 bool dynamic, float density, float friction):
	Shape(game, position, dynamic, density, friction)
{
    size = size_;
    
    // Define another box shape for our dynamic body.
    b2PolygonShape boxShape; //dynamicBox;
    boxShape.SetAsBox((size->x/2) * game->getUtility()->getPTM(), (size->y/2) * game->getUtility()->getPTM());
    
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &boxShape;
    
    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = density;
    
    // Override the default friction.
    fixtureDef.friction = friction;
    
    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);
    
    shape = new sf::RectangleShape(sf::Vector2f(size->x, size->y));
    
    // This makes SFML use the same origin for shapes as Box2D(middle, middle)
    shape->setOrigin(size->x/2, size->y/2);
    shape->setPosition(position->x, position->y);
}

Rectangle::~Rectangle()
{
   	delete size;
	delete Shape::shape;
}

b2Vec2* Rectangle::getSize() const {
    return size;
}