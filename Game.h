#ifndef __Block_Frog__Game__
#define __Block_Frog__Game__

#include <iostream>
#include <SFML/Graphics.hpp>
#include "UDPNetwork.h"
#include <Box2D/Box2D.h>
#include "Player.h"
#include <list>
#include <thread>
#include <typeinfo>
class Shape;
class ShapeFactory;
class Controller;
class OSHandler;
class UDPNetwork;

class Game
{
public:
	Game(sf::RenderWindow* window, OSHandler* osHandler,bool server, sf::IpAddress* serverip = nullptr, unsigned short serverPort = 0);
	~Game();
    void run();

    void spawnBox(sf::Vector2i position);
    
    sf::RenderWindow* getWindow();
    b2World* getWorld();
    Player* getPlayer();
    OSHandler* getOSHandler();
    
private:
	void removeFallenBoxes(std::list<Shape*>& todelete);
	void calcViewOffset();
	Shape* boxHandling(); //create and destroy boxes. Returns pointer to last created box
	
    b2World* world;
    Player* player;
    OSHandler* osHandler;
	ShapeFactory* shapeFactory;
    
    sf::RenderWindow* window;
	sf::View* view;
	sf::Vector2i viewOffset;
    sf::Clock clock;
    sf::Time timer;

	//network
	UDPNetwork* localHost;
    
	std::thread* network;
    std::vector<Shape*> boxes;
	double duration;
	float riseSpeed;
	int secPerDrops; //time before a new block is dropped
	int killOffset; //how far under the screen blocks will be killed
	friend Controller;
	friend UDPNetwork;
};

#endif /* defined(__Block_Frog__Game__) */
