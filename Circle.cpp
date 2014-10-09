#include "Circle.h"
#include "Game.h"
#include "Utility.h"

Circle::Circle(Game* game, float radius_, b2Vec2* position,
					 bool dynamic, int id, float density, float friction, int groupIndex):
Shape(game, position, dynamic, density, friction, groupIndex, id)
{
    radius = radius_;
    
    // Define another box shape for our dynamic body.
    b2CircleShape circleShape; //dynamicBox;
    circleShape.m_radius = radius * game->getUtility()->getPTM();

//    boxShape.SetAsBox((size->x/2) * pixelToMeter, (size->y/2) * pixelToMeter);
//    circleS
    // Define the dynamic body fixture.
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circleShape;
    
    // Set collision group. Will not collide with objects of same positive group
    fixtureDef.filter.groupIndex = groupIndex;
    
    // Set the box density to be non-zero, so it will be dynamic.
    fixtureDef.density = density;
    
    // Override the default friction.
    fixtureDef.friction = friction;
    
    // Add the shape to the body.
    body->CreateFixture(&fixtureDef);
    
    shape = new sf::CircleShape(radius);
    
    // This makes SFML use the same origin for shapes as Box2D(middle, middle)
    shape->setOrigin(radius, radius);
    shape->setPosition(position->x, position->y);
}

Circle::~Circle()
{
	delete Shape::shape;
}

float Circle::getRadius() {
    return radius;
}