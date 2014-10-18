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
	Player(Game* game);
    void init(Player* player);

	void setName(string n);
    void setPosition(b2Vec2* newPos);
    void resetPlayer(b2Vec2* newPos);
    void setBirthNumber(int number);
    
    b2Body* getBody();
	string& getName(){return name;};
	b2Vec2* getPosition() const{return box->getPosition();};
    ContactListener* getContactListener() { return contactListener; }
	Shape* getBox() {return box;};
	Circle* getHookTip() {return hook->getHookTip();};
	Rectangle* getHookBase() {return hook->getHookBase();};
    int getBirthNumber() { return birthNumber; }
	//Hook* getHook() {return hook;};

    
	enum { LEFT, RIGHT, JUMP, LEFT_STOP, RIGHT_STOP };
	//set bool to true when moving a remote player
	void move(int dir, bool localPlayer=true, bool is_jumping = true);
	/*
	*	Pushes player in the direction
	*/
	void push(b2Vec2&& direction);
    void draw();
	void update();
    void useHook(sf::Vector2i mousePos, bool localPlayer = true);
    void aimHook(sf::Vector2i mousePos, bool localPlayer = true);
    void releaseHook(bool localPlayer = true);
    void increaseHook();
    void decreaseHook();
    bool isJumping();
	void setDeath(bool d) {dead = d;};
	bool isDead() {return dead;};

private:
    Game* game;
    Hook* hook;
    Rectangle* box;
    ContactListener* contactListener;
    Circle* hookTip;
    sf::Sprite frogSprite;
    sf::Texture frogTexture;
    b2Fixture* footSensorFixture;
	//check for item collision
	void itemCheck();
    
	float leftSpeed, rightSpeed;
	float jumpHeight;
    float movementSpeed;
    bool jumping;
	string name;
    int birthNumber;
	bool dead;
	friend Item;

	//small networking
	void send(sf::Packet& p);
};

#endif /* defined(__Block_Frog__Player__) */
