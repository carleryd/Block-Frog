#include "Shape.h"
#include "Game.h"
#include <math.h>
#include "Utility.h"
#include "ParticleSystem.h"

const float PI = 3.14f;

#include <iostream>

Shape::Shape(Game* game_, b2Vec2* position_, bool dynamic_, int id, float density_, float friction_, int groupIndex)
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
	userData.toBeRemoved = false;
	body->SetUserData((void*)&userData);
	updateClock.restart();
	particleSystem = nullptr;
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

	//particles
	if(particleSystem != nullptr)
	{
		particleSystem->setEmitter( shape->getPosition() );
		particleSystem->update();
		game->getWindow()->draw(*particleSystem);
		
		if(!particleSystem->isRunning())
		{
			delete particleSystem;
			particleSystem = nullptr;
		}
	}
	else if(userData.id > -1)
		checkForContacts();
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

void Shape::makeStatic()
{
	if(body->GetType() != b2BodyType::b2_staticBody)
	{
		body->SetType(b2BodyType::b2_staticBody);
		startParticles();
	}
}

void Shape::checkForContacts()
{
	b2ContactEdge* edge = body->GetContactList();
	for(;edge; edge = edge->next)
	{
		if(edge->contact->IsTouching() && body->GetLinearVelocity().Length() > 1)
		{
			startParticles();
		}
	}
}

void Shape::startParticles()
{
	if(particleSystem == nullptr)
		particleSystem = new ParticleSystem(1000, shape->getPosition());
}