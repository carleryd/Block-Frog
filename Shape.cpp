#include "Shape.h"
#include "Game.h"
#include <math.h>
#include "Utility.h"

const float PI = 3.14f;

#include <iostream>

Shape::Shape(Game* game_, b2Vec2* position_, bool dynamic_, float density_, float friction_, int groupIndex)
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
}

Shape::~Shape(void)
{
    cout << "Shape::~Shape()" << endl;
    
    game->getWorld()->DestroyBody(body);
    body = NULL;
	delete position;
    position = NULL;
    game = NULL;
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