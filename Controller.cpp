//
//  Controller.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-09.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "Controller.h"

Controller::Controller(Game* game_) 
{
    game = game_;
	game->window->setKeyRepeatEnabled(false); 
}

void Controller::checkInput() {
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
                    moveLeft();
                    break;
             	case sf::Keyboard::Right:
                    // gå höger
                    break;
                case sf::Keyboard::Space:
                    // JUMP AROUND
                    break;
				
					//dev ctrls
				case sf::Keyboard::R:
					if(game->riseSpeed == 0)
					{
						game->riseSpeed = -0.5f; 
						cout << "Water rising turned ON." << endl;
					}
					else
					{
						game->riseSpeed = 0; 
						cout << "Water rising turned OFF." << endl;
					}
					break;


                default:
                    //std::cout << "AAAH OF COURSE" << std::endl;
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

void Controller::moveLeft() {
    //game->getPlayer()->setPosition(new b2Vec2(50, 50));
	game->getPlayer()->move(b2Vec2(-10, 0));
}
