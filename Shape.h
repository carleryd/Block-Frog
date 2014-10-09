#ifndef SHAPE_H
#define SHAPE_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <ctime>

class Game;
struct syncStruct;
struct b2BodyUserData
{
	int id;
};

class Shape
{
public:
	virtual ~Shape(void) = 0;
	void update();
    
	sf::Shape* getShape();
	b2Body* getBody();
	b2Vec2* getPosition() const {return position;};
	void setPosition(b2Vec2* pos) 
	{
		position = pos;
		body->SetTransform(*pos, body->GetAngle());
	};
	int getId() const {return userData.id;};

protected:
	Shape(Game* game, b2Vec2* position,
		bool dynamic, float density, float friction, int groupIndex, int id);
    
	sf::Shape* shape;
    b2Body* body;
    b2Vec2* position;
	b2BodyUserData userData;
    
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