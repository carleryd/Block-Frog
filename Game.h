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
#include <Box2D/Box2D.h>
#include "Player.h"
#include "ShapeFactory.h"
#include <list>
class Shape;
class Controller;

class Game
{
public:
    Game(sf::RenderWindow* window);
	~Game();
    void run();

    void spawnBox(sf::Vector2i position);
    
    sf::RenderWindow* getWindow();
    b2World* getWorld();
    Player* getPlayer();
    
private:
	void removeFallenBoxes(std::list<Shape*>& todelete);
    sf::RenderWindow* window;
    b2World* world;
    Player* player;
	sf::View* view;
    std::vector<Shape*> boxes;
	ShapeFactory* shapeFactory;
	float riseSpeed;
};

#endif /* defined(__Block_Frog__Game__) */
