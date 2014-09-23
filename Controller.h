#ifndef __Block_Frog__Controller__
#define __Block_Frog__Controller__

#include <iostream>
#include <SFML/Graphics.hpp>
#include "Game.h"

class Controller
{
public:
    Controller(Game* game);
    void checkInput();
private:
    void spawnBox(sf::Vector2i position, b2World* world, std::vector<Rectangle*>& boxes);
	void devInput(sf::Event& e);
    
    Game* game;
	bool pressedLastFrame;
};

#endif /* defined(__Block_Frog__Controller__) */