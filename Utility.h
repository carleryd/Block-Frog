#ifndef __Block_Frog__Utility__
#define __Block_Frog__Utility__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

class Game;

class Utility
{
public:
    Utility(Game* game);
    
    b2Vec2 mouseToBox2D(sf::Vector2i mousePos);
    float mouseAngle(sf::Vector2i mousePixelPos, b2Vec2 playerMeterPos);
    float degToRad(float degrees);
    float radToDeg(float radians);
    
    double getPTM();
    float getMTP();
    float getOffSetX();
    float getOffSetY();
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
