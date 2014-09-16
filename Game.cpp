//
//  Game.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-05.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Game.h"

Game::Game(sf::RenderWindow* window_) {
    window = window_;
    
    // Setting up Box2D physics
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -0.1f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    world = new b2World(gravity);

    std::cout << "Window:" << std::endl << "width: " << window->getSize().x << std::endl << "height: " << window->getSize().y << std::endl;
    
    boxes.push_back(new Rectangle(world, new b2Vec2(750.0f, 50.0f), new b2Vec2(0.0f, -float(window->getSize().y)/2), false));
//	boxes.push_back(new Rectangle(world, new b2Vec2(100.0f, 100.0f), new b2Vec2(0.0f, 200.0f), true));
//    boxes.push_back(new Rectangle(world, new b2Vec2(50.0f, 50.0f), new b2Vec2(-50.0f, 100.0f),  true));
    
    player = new Player(world);
}

sf::RenderWindow* Game::getWindow() {
    return window;
}

b2World* Game::getWorld() {
    return world;
}


void Game::run() {
    for(Rectangle* box : boxes) {
        box->update();
        window->draw(*box->getShape());
    }
    player->draw(window);
    
    // timeStep, velocityIterations, positionIterations
    world->Step(1.0/60.0, 10, 10);
}

void Game::spawnBox(sf::Vector2i position) {
    // adjPos is a position adjusted to window position, also adjusted according to Box2D positions, y upwards
    sf::Vector2i adjPos = sf::Vector2i(position.x - window->getSize().x/2,
                                                  -position.y + window->getSize().y/2);
    std::cout << "Spawning box at position " << position.x << " " << position.y << std::endl;
    boxes.push_back(new Rectangle(world, new b2Vec2(20.0f, 20.0f), new b2Vec2(float(adjPos.x), float(adjPos.y)), true));
    std::cout << boxes.size() << std::endl;
}