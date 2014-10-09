#include "Utility.h"
#include "Game.h"
#include <math.h>

Utility::Utility(Game* game_) {
    game = game_;
    cycles = 0;
    currDegrees = 0;
    prevDegrees = 0;
    pixelToMeter = 1.0/30.0;
    meterToPixel = 30.0;
    // Window: 800x600(access this form game->getWorld()->getSize())
    // Box2D coordinate system is in the middle of screen, SFML is top-left.
    // These offsets will make SFML start off drawing in the middle
    offSetX = game->getWindow()->getSize().x / meterToPixel / 2.0;
    offSetY = game->getWindow()->getSize().y / meterToPixel / 2.0;
}

b2Vec2 Utility::mouseToBox2D(sf::Vector2i mousePos) {
    sf::Vector2i adjPixelPos = sf::Vector2i(mousePos.x - game->getWindow()->getSize().x/2,
                                            -mousePos.y + game->getWindow()->getSize().y/2);
    
    b2Vec2 adjMeterPos = b2Vec2(adjPixelPos.x * pixelToMeter,
                                adjPixelPos.y * pixelToMeter);
    
    return adjMeterPos;
}

float Utility::angleBetweenPoints(sf::Vector2i pixelPosA, sf::Vector2i pixelPosB) {
    b2Vec2 convertedA = b2Vec2(pixelPosA.x * pixelToMeter, pixelPosA.y * pixelToMeter);
    b2Vec2 convertedB = b2Vec2(pixelPosB.x * pixelToMeter, pixelPosB.y * pixelToMeter);
    
    return angleBetweenPoints(convertedA, convertedB);
}

float Utility::angleBetweenPoints(b2Vec2 meterPosA, b2Vec2 meterPosB) {
    b2Vec2 vector = b2Vec2(meterPosA.x - meterPosB.x, meterPosA.y - meterPosB.y);
    
    currDegrees = (atan2(vector.y, -vector.x) * 180 / M_PI + 180);
    
    // There is a problem where the revoluteJoint goes from 360 -> 1 degrees.
    // This makes the joint go back counter-clockwise
	if(currDegrees - 270 > prevDegrees)
        cycles--;
    else if(currDegrees + 270 < prevDegrees)
        cycles++;
    
    prevDegrees = currDegrees;
    
    cout << "cycles: " << cycles << endl;
    
    // The reason I have y and -x is to get 0/360 degrees to be to the right of player
    return currDegrees + 360 * cycles;
}

float Utility::mouseAngle(sf::Vector2i mousePixelPos, b2Vec2 playerMeterPos) {
    b2Vec2 mouseMeterPos = game->getUtility()->mouseToBox2D(mousePixelPos);
    
    return angleBetweenPoints(mouseMeterPos, playerMeterPos);
}

float Utility::degToRad(float degrees) {
    return degrees / 57.2957795;
}

float Utility::radToDeg(float radians) {
    return radians * 57.2957795;
}

double Utility::getPTM() { return pixelToMeter; }
float Utility::getMTP() { return meterToPixel; }
float Utility::getOffSetX() { return offSetX; }
float Utility::getOffSetY() { return offSetY; }