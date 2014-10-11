#ifndef __Block_Frog__Hook__
#define __Block_Frog__Hook__

#include <iostream>
#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>

class Circle;
class Rectangle;
class Game;
class Utility;
class ContactListener;
class Player;

class Hook
{
public:
	Hook(Game* game, Player* player);
//    ~Hook();
    
    enum Action
    {
        SHOOTING,
        PASSIVE
    };
    
    void draw();
    void update();
    void shoot(sf::Vector2i mousePixelPos);
    void aim(sf::Vector2i mousePixelPos);
    void release();
    b2RevoluteJoint* grab(b2Body* box);
    
    float getLength();
    void changeLength(float delta);
    
private:
    Game* game;
    Circle* hookTip;
    Rectangle* hookBase;
    Utility* utility;
	ContactListener* contactListener;
    
    Action ACTION;
    
    b2PrismaticJoint* prismaticJoint;
    b2RevoluteJoint* revoluteJoint;
    b2RevoluteJoint* grabJoint;
    // The most recent box we have hit with hook
    b2Body* recentBoxContact;
    b2Vec2 playerMeterPos;
    
    float newMouseAngle;
    float oldMouseAngle;
    // This is the length the hook is striving towards having
    float passiveLength;
    // This is the length the hook should have when holding a box
    float grabLength;
    // This is the length we want to strive towards when using hook
    float reachLength;
    // This is needed so as to not, for example, go into the if case
    // of hook length > x several times because it has yet to adjust
    float currentLength;
};

#endif /* defined(__Block_Frog__Hook__) */
