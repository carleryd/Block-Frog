//
//  Game.h
//  Block Frog
//
//  Created by roflmao on 2014-09-05.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#ifndef __Block_Frog__Game__
#define __Block_Frog__Game__

#include <iostream>
#include <SFML/Graphics.hpp>
#include "UDPNetwork.h"
#include <Box2D/Box2D.h>
#include "Player.h"
#include "ShapeFactory.h"
#include <list>
#include <ctime>
class Shape;
class Controller;
class OSHandler;
class UDPNetwork;

class Game
{
public:
	Game(sf::RenderWindow* window, bool server, sf::IpAddress* serverip=nullptr, unsigned short serverPort = 0);
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
	
    b2World* world;
    Player* player;
    OSHandler* osHandler;
	ShapeFactory* shapeFactory;
    sf::RenderWindow* window;
	sf::View* view;
    std::vector<Shape*> boxes;
	sf::Vector2i viewOffset;

	//network
	UDPNetwork* localHost;
    
    sf::Clock clock;
    sf::Time timer;
    
	double duration;
	float riseSpeed;
	int secPerDrops; //time before a new block is dropped
	int killOffset; //how far under the screen blocks will be killed
	friend Controller;
};

#endif /* defined(__Block_Frog__Game__) */
