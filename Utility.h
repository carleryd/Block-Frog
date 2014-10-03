#ifndef __Block_Frog__Utility__
#define __Block_Frog__Utility__

#include <iostream>

class Game;

class Utility
{
public:
    Utility(Game* game);
    
    float getPTM();
    float getMTP();
    float getOffSetX();
    float getOffSetY();
private:
    float pixelToMeter;
    float meterToPixel;
    float offSetX;
    float offSetY;
};

#endif /* defined(__Block_Frog__Utility__) */
