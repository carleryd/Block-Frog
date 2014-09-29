//
//  Hook.h
//  Block Frog
//
//  Created by roflmao on 2014-09-29.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#ifndef __Block_Frog__Hook__
#define __Block_Frog__Hook__

#include <iostream>
#include <Box2D/Box2D.h>
class Circle;
class Game;

class Hook
{
public:
    Hook(Game* game);
    void draw();
    void update();
    
    float getLength();
    void changeLength(float delta);
    
private:
    Game* game;
    Circle* hookTip;
    b2DistanceJoint* distanceJoint;
};

#endif /* defined(__Block_Frog__Hook__) */
