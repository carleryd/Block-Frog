#ifndef __Block_Frog__Player__
#define __Block_Frog__Player__

#include <iostream>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include "Rectangle.h"
#include "Circle.h"
#include "ContactListener.h"
#include "Hook.h"
#include "PacketParser.h"
class Game;
class Hook;
class Item;

using namespace std;

class Player
{
public:
	Player(Game* game_, int localID_, string color);
    ~Player();
    void init();

    void setPosition(b2Vec2* newPos);
    void setLocalID(int localID_) { localID = localID_; }
    
    void resetPlayer(b2Vec2* newPos);
    
    b2Body* getBody() { return box->getBody(); }
	b2Vec2* getPosition() const{return box->getPosition();};
    ContactListener* getContactListener() { return contactListener; }
	Shape* getBox() {return box;};
	Circle* getHookTip() {return hook->getHookTip();};
	Rectangle* getHookBase() {return hook->getHookBase();};
    Hook* getHook();
    int getLocalID();
    string getAlias() { return alias; }
    
	enum { LEFT, RIGHT, JUMP, LEFT_STOP, RIGHT_STOP };
	//set bool to true when moving a remote player
    void useHook(sf::Vector2i mousePos, bool localPlayer = true);
    void aimHook(sf::Vector2i mousePos, bool localPlayer = true);
    void releaseHook(bool localPlayer = true);
    void increaseHook();
    void decreaseHook();
    void destroyHook();
    
	void move(int dir, bool localPlayer=true, bool is_jumping = true);
	void push(b2Vec2&& direction);
    bool isJumping();
    
    void draw();
	void update();
    
	void setDeath(bool d) {dead = d;};
	bool isDead() {return dead;};

private:
    Game* game;
    sf::RenderWindow* window;
    Hook* hook;
    Rectangle* box;
    ContactListener* contactListener;
    Circle* hookTip;
    sf::Sprite frogSprite;
    sf::Texture frogTexture;
    b2Fixture* footSensorFixture;
	//check for item collision
	void itemCheck();
    
    int localID;
	float leftSpeed, rightSpeed;
	float jumpHeight, downWardPull, linearDamping;
    float movementSpeed;
    bool jumping;
	string alias;
	bool dead;
	friend Item;

	//small networking
	void send(sf::Packet& p);
};

#endif /* defined(__Block_Frog__Player__) */
