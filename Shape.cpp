#include "Shape.h"
#include "Game.h"
#include <math.h>

const float PI = 3.14f;

#include <iostream>

Shape::Shape(Game* game, b2Vec2* position_, bool dynamic_, float density_, float friction_)
{
	pixelToMeter = 1.0f/30.0f; // Box2D meter equals 50 pixels?
    meterToPixel = 30.0;
    // Window: 800x600(access this form game->getWorld()->getSize())
    // Box2D coordinate system is in the middle of screen, SFML is top-left. These offsets will make SFML start off drawing in the middle
    offSetX = game->getWindow()->getSize().x / meterToPixel / 2;
    offSetY = game->getWindow()->getSize().y / meterToPixel / 2;
    
//    size = size_;
    position = position_;
    dynamic = dynamic_;
    density = density_;
    friction = friction_;
    
    // Define the dynamic body. We set its position and call the body factory.
    b2BodyDef bodyDef;
    bodyDef.position.Set(position->x * pixelToMeter, position->y * pixelToMeter);
    
    if(dynamic)
        bodyDef.type = b2_dynamicBody;
    
    body = game->getWorld()->CreateBody(&bodyDef);
}

Shape::~Shape(void)
{
	//delete body; not access?
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