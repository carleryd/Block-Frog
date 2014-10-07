#ifndef SHAPE_H
#define SHAPE_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <ctime>
#include <memory>

class Game;
struct syncStruct;

class Shape
{
public:
	virtual ~Shape(void) = 0;
	void update();

	sf::Shape* getShape();
	b2Body* getBody();
	b2Vec2* getPosition() const {return position;};
	const b2Vec2& getVelocity() const {return body->GetLinearVelocity();};
	float getAngle() const {return body->GetAngle();};
	float getAngularVelocity() const {return body->GetAngularVelocity();};
	void setPosition(b2Vec2* pos, float angle){ position = pos; body->SetTransform(*position,angle);};
	void setVelocity(b2Vec2& velocity) {body->SetLinearVelocity(velocity);};
	void setAngularVelocity(float v) {body->SetAngularVelocity(v);};
	//will return true if the Shapes are "close enough"
	//i.e. if shapes are some what synched according to some value
	//if false, it will synch with the given syncstruct
	bool operator==(const syncStruct& serverShape);

protected:
	Shape(Game* game, b2Vec2* position,
		bool dynamic, float density, float friction, int groupIndex);

	sf::Shape* shape;
	//sf::Shape* shape;
	b2Body* body;
    b2Vec2* position;
    
	std::clock_t clock;
    float density, friction;
    float angle;
    bool dynamic;
private:
    Game* game;
};

struct syncStruct
{
	b2Vec2 velocity;
	b2Vec2 position;
	float angle;
	float angularVelocity;
};

#endif