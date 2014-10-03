#include "Utility.h"
#include "Game.h"

Utility::Utility(Game* game) {
    pixelToMeter = 1.0f/30.0f; // Box2D meter equals 50 pixels?
    meterToPixel = 30.0;
    // Window: 800x600(access this form game->getWorld()->getSize())
    // Box2D coordinate system is in the middle of screen, SFML is top-left. These offsets will make SFML start off drawing in the middle
    offSetX = game->getWindow()->getSize().x / meterToPixel / 2;
    offSetY = game->getWindow()->getSize().y / meterToPixel / 2;
}

float Utility::getPTM() { return pixelToMeter; }
float Utility::getMTP() { return meterToPixel; }
float Utility::getOffSetX() { return offSetX; }
float Utility::getOffSetY() { return offSetY; }