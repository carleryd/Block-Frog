#include "Shape.h"
#include "Game.h"
#include <math.h>
#include "Utility.h"

const float PI = 3.14f;

#include <iostream>

Shape::Shape(Game* game_, b2Vec2* position_, bool dynamic_, float density_, float friction_, int groupIndex, int id)
{
    game = game_;
    position = position_;
    dynamic = dynamic_;
    density = density_;
    friction = friction_;
    
    // Define the dynamic body. We set its position and call the body factory.
    b2BodyDef bodyDef;
    bodyDef.position.Set(position->x * game->getUtility()->getPTM(),
                         position->y * game->getUtility()->getPTM());
    
    if(dynamic)
        bodyDef.type = b2_dynamicBody;
    
    body = game->getWorld()->CreateBody(&bodyDef);

	userData.id = id;
	userData.parent = this;
	body->SetUserData((void*)&userData);
	updateClock.restart();
	ajour = true;
}

Shape::~Shape(void)
{
    game->getWorld()->DestroyBody(body);
	//delete position; //<- causes client to crash
}

void Shape::update()
{
    shape->setPosition((body->GetPosition().x + game->getUtility()->getOffSetX()) * game->getUtility()->getMTP(),
                       (-body->GetPosition().y + game->getUtility()->getOffSetY()) * game->getUtility()->getMTP());
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

sf::Time Shape::timeSinceUpdate() const
{
	return updateClock.getElapsedTime();
}

void Shape::resetUpdateClock()
{
	updateClock.restart();
}