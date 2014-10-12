#ifndef SHAPE_H
#define SHAPE_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <ctime>
#include <iostream>
using namespace std;

class Game;
class Shape;
struct b2BodyUserData
{
	int id;
	Shape* parent;
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
	void setPosition(b2Vec2* pos, float angle) 
	{
		position = pos;
		body->SetTransform(*pos, angle);
	};
	void setId(int id) { userData.id = id; };
	int getId() const {return userData.id;};
	sf::Time timeSinceUpdate() const;
	//reset the update clock
	void resetUpdateClock();
	
	bool ajour; 

protected:
	Shape(Game* game, b2Vec2* position,
		bool dynamic, int id, float density, float friction, int groupIndex);
    
	sf::Shape* shape;
    b2Body* body;
    b2Vec2* position;
	b2BodyUserData userData;
	sf::Clock updateClock;
    
	std::clock_t clock;
    float density, friction;
    float angle;
    bool dynamic;
private:
    Game* game;
};



#endif