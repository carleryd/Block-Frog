//
//  Controller.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-09.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Controller.h"

void Controller::checkInput(Game* game) {
	sf::Event event;
    while (game->getWindow()->pollEvent(event))
    {
        // Close window : exit
        if (event.type == sf::Event::Closed) {
            game->getWindow()->close();
        }
        // Escape pressed : exit
        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
                case sf::Keyboard::Escape:
                    game->getWindow()->close();
                    break;
             	case sf::Keyboard::Left:
                    // gå vänster
                    break;
             	case sf::Keyboard::Right:
                    // gå höger
                    break;
                case sf::Keyboard::Space:
                    // JUMP AROUND
                    break;
                default:
                    std::cout << "AAAH OF COURSE" << std::endl;
                    break;
            }
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
	        sf::Vector2i position = sf::Mouse::getPosition(*game->getWindow());
            game->spawnBox(position);
//            spawnBox(sf::Vector2i(position.x - game->getWindow()->getSize().x/2, -position.y + game->getWindow()->getSize().y/2), game->getWorld(), boxes);
        }
    }
}