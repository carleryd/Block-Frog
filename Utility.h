#ifndef __Block_Frog__Utility__
#define __Block_Frog__Utility__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
using namespace std;



class Game;

class Utility
{
public:
    Utility(Game* game);
    
    b2Vec2 mouseToBox2D(sf::Vector2i mousePos);
    float angleBetweenPoints(b2Vec2 meterPosA, b2Vec2 meterPosB);
    float angleBetweenPoints(sf::Vector2i meterPosA, sf::Vector2i meterPosB);
    float mouseAngle(sf::Vector2i mousePixelPos, b2Vec2 playerMeterPos, float hookDegrees);
    
    float degToRad(float degrees);
    float radToDeg(float radians);
    
    double getPTM();
    float getMTP();
    float getOffSetX();
    float getOffSetY();

	friend ostream& operator<<(ostream& o, b2Vec2* v)
	{
		return o << "position (" << v->x << ", " << v->y << ")";
	}
private:
    Game* game;
    
    double pixelToMeter;
    float meterToPixel;
    float offSetX;
    float offSetY;
    short int cycles;
    float currDegrees, prevDegrees;


};



#endif /* defined(__Block_Frog__Utility__) */
