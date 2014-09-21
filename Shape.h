#ifndef SHAPE_H
#define SHAPE_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <ctime>

class Shape
{
public:
	//Shape(void);
	virtual ~Shape(void) = 0;
	void update();
	sf::Shape* getShape();
	b2Body* getBody();
	b2Vec2* getSize() const {return size;};
	b2Vec2* getPosition() const {return position;};
protected:
	Shape(b2World* world, b2Vec2* recSize, b2Vec2* recPosition, 
		bool recDynamic, float recDensity, float recFriction, sf::RenderWindow* w);
	sf::Shape* shape;
    b2Body* body;
	b2Vec2* size;
    b2Vec2* position;
	float pixelToMeter, meterToPixel, offSetX, offSetY;
    float density, friction;
    bool dynamic;
    float angle;
	std::clock_t clock;
private:

};

#endif