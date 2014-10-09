#include "Utility.h"
#include "Game.h"
#include <math.h>
const float pi = 3.14;

Utility::Utility(Game* game_) {
    game = game_;
    pixelToMeter = 1.0/30.0;
    meterToPixel = 30.0;
    // Window: 800x600(access this form game->getWorld()->getSize())
    // Box2D coordinate system is in the middle of screen, SFML is top-left. These offsets will make SFML start off drawing in the middle
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

float Utility::mouseAngle(sf::Vector2i mousePixelPos, b2Vec2 playerMeterPos) {
    b2Vec2 mouseMeterPos = game->getUtility()->mouseToBox2D(mousePixelPos);
    b2Vec2 relativeToPlayer = b2Vec2(mouseMeterPos.x - playerMeterPos.x,
                                     mouseMeterPos.y - playerMeterPos.y);
	
    // The reason I have y and -x is to get 0/360 degrees to be to the right of player
    return atan2(relativeToPlayer.y, -relativeToPlayer.x) * 180 / pi + 180;
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