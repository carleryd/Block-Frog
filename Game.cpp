//
//  Game.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-05.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Game.h"
using namespace std;

Game::Game(sf::RenderWindow* window_)
{
    window = window_;
	view = new sf::View();
	view->setCenter(window->getSize().x/2, window->getSize().y/2);
	view->setSize(sf::Vector2f(window->getSize()));
	window->setView(*view);
    
    // Setting up Box2D physics
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -0.1f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    world = new b2World(gravity);
	//shapefactory for creating shapes easily
	shapeFactory = new ShapeFactory(world, window);
    
	//boxes.push_back(new Rectangle(world, new b2Vec2(750.0f, 50.0f), new b2Vec2(0.0f, -windowHeight/2), false, window));
	//boxes.push_back(new Rectangle(world, new b2Vec2(100.0f, 100.0f), new b2Vec2(0.0f, 200.0f), true, window));
    //boxes.push_back(new Rectangle(world, new b2Vec2(50.0f, 50.0f), new b2Vec2(-50.0f, 100.0f),  true, window));
    boxes.push_back(shapeFactory->createRectangle(new b2Vec2(750.0f, 50.0f), new b2Vec2(0.0f, -float(window->getSize().y)/2), false));
    
    player = new Player(world, window);

	//TEMP
	riseSpeed = -0.01f;
}

Game::~Game()
{
	while(!boxes.empty())
	{
		delete boxes.back();
		boxes.pop_back();
	}
	delete world;
	delete player;
	delete view;
}

sf::RenderWindow* Game::getWindow() {
    return window;
}

b2World* Game::getWorld() {
    return world;
}

Player* Game::getPlayer() {
    return player;
}

void Game::run() {

	//handle boxes
	list<Shape*> deletion;
    for(Shape* box : boxes) 
	{
		//check if box has fallen "outside"
		if(box->getShape()->getPosition().y > window->getSize().y)
			deletion.push_back(box);
        box->update();
        window->draw(*box->getShape());
    }
	removeFallenBoxes(deletion);

	//draw
    player->draw(window);

	//move view up/raise water level
	view->move(0, riseSpeed);
	window->setView(*view);
    
    // timeStep, velocityIterations, positionIterations
    world->Step(1.0f/60.0f, 10, 10);
}

void Game::spawnBox(sf::Vector2i position) {
    // adjPos is a position adjusted to window position, also adjusted according to Box2D positions, y upwards
    sf::Vector2i adjPos = sf::Vector2i(position.x - window->getSize().x/2,
                                                  -position.y + window->getSize().y/2);
    std::cout << "Spawning box at position " << position.x << " " << position.y << std::endl;
	boxes.push_back(
		shapeFactory->createRectangle(
		new b2Vec2(20.0f, 20.0f), 
		new b2Vec2(float(adjPos.x), float(adjPos.y)), 
		true));
    std::cout << boxes.size() << std::endl;
}

void Game::removeFallenBoxes(list<Shape*>& deletion)
{
	if(!deletion.empty())
		cout << deletion.size() << " shapes have fallen out of the screen and been deleted this frame." << endl;
	while(!deletion.empty())
	{
		Shape* del = deletion.front();
		vector<Shape*>::iterator todelete = std::remove_if(boxes.begin(), boxes.end(), [&deletion](Shape* b)
		{
			return deletion.front() == b;
		}
		);
		boxes.erase(todelete, boxes.end());
		delete deletion.front();
		deletion.pop_front();
	}
		
	deletion.clear();
}