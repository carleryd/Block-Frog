#include "Shape.h"
#include <math.h>
//#ifdef OS_WINDOWS
const float PI = 3.14f;
//#endif

#include <iostream>

Shape::Shape(b2World* world, b2Vec2* recSize, b2Vec2* recPosition, bool recDynamic, float recDensity, float recFriction, sf::RenderWindow* w)
{
	pixelToMeter = 1.0f/30.0f; // Box2D meter equals 50 pixels?
    meterToPixel = 30.0;
    // Window: 800x600
    // Box2D coordinate system is in the middle of screen, SFML is top-left. These offsets will make SFML start off drawing in the middle
    offSetX = 800 / meterToPixel / 2;
    offSetY = 600 / meterToPixel / 2;
    //std::cout << w->getSize().x << std::endl;
    
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
    
    body = world->CreateBody(&bodyDef);
    
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
    //std::cout << "Position: " << body->GetPosition().x << " " << body->GetPosition().y << std::endl;
    //std::cout << "Size    : " << size->x * pixelToMeter << " " << size->y * pixelToMeter << std::endl;
    
    shape = new sf::RectangleShape(sf::Vector2f(size->x, size->y));
    // This makes SFML use the same origin for shapes as Box2D(middle, middle)
    shape->setOrigin(size->x/2, size->y/2);
    shape->setPosition(position->x, position->y);	
	//clock = std::clock();
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

	//just a try to set a non-moving body to static and vice versa when moving
	//not functional at the moment
	/*double duration = (std::clock() - clock) / (double) CLOCKS_PER_SEC;
	if(body->GetLinearVelocity().LengthSquared() < 0.5f && duration > 3)
	{
		body->SetActive(false);
	}
	/*else if(body->GetWorld()->GetContactCount() > 2)
	{
		//b2Contact* contact =  body->GetWorld()->GetContactList();
		//contact->GetTangentSpeed();
		body->SetActive(true);
	}*/
//    std::cout << "shape angle: " << shape->getRotation() << std::endl;
//    std::cout << "body angle: " << body->GetAngle() / PI * 180 << std::endl;
}

sf::Shape* Shape::getShape()
{
	return shape;
}

b2Body* Shape::getBody()
{
	return body;
}