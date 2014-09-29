#ifndef SHAPE_H
#define SHAPE_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <ctime>

class Game;

class Shape
{
public:
	virtual ~Shape(void) = 0;
	void update();
    
	sf::Shape* getShape();
	b2Body* getBody();
//	b2Vec2* getSize() const {return size;};
	b2Vec2* getPosition() const {return position;};
protected:
	Shape(Game* game, b2Vec2* position,
		bool dynamic, float density, float friction);
	sf::Shape* shape;
    b2Body* body;
//	b2Vec2* size;
    b2Vec2* position;
    
	std::clock_t clock;
	float pixelToMeter, meterToPixel, offSetX, offSetY;
    float density, friction;
    float angle;
    bool dynamic;
private:
    Game* game;
};

#endif