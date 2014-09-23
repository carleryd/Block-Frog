#include "Shape.h"
#include "Game.h"
#include <math.h>

const float PI = 3.14f;

#include <iostream>

Shape::Shape(Game* game, b2Vec2* recSize, b2Vec2* recPosition, bool recDynamic, float recDensity, float recFriction)
{
	pixelToMeter = 1.0f/30.0f; // Box2D meter equals 50 pixels?
    meterToPixel = 30.0;
    // Window: 800x600(access this form game->getWorld()->getSize())
    // Box2D coordinate system is in the middle of screen, SFML is top-left. These offsets will make SFML start off drawing in the middle
    offSetX = game->getWindow()->getSize().x / meterToPixel / 2;
    offSetY = game->getWindow()->getSize().y / meterToPixel / 2;
    
    size = recSize;
    position = recPosition;
    dynamic = recDynamic;
    density = recDensity;
    friction = recFriction;
    
    // Define the dynamic body. We set its position and call the body factory.
    b2BodyDef bodyDef;
    bodyDef.position.Set(position->x * pixelToMeter, position->y * pixelToMeter);
    
    if(dynamic)
        bodyDef.type = b2_dynamicBody;
    
    body = game->getWorld()->CreateBody(&bodyDef);
    
    // Define another box shape for our dynamic body.
    b2PolygonShape boxShape; //dynamicBox;
    boxShape.SetAsBox((size->x/2) * pixelToMeter, (size->y/2) * pixelToMeter);
    
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

Shape::~Shape(void)
{
	//delete body; not access?
	delete size;
	delete position;
}

void Shape::update()
{
    shape->setPosition((body->GetPosition().x + offSetX) * meterToPixel, (-body->GetPosition().y + offSetY) * meterToPixel);
    shape->setRotation((-body->GetAngle() / PI) * 180);
}

sf::Shape* Shape::getShape()
{
	return shape;
}

b2Body* Shape::getBody()
{
	return body;
}