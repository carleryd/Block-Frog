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
                    game->getPlayer()->move(game->getPlayer()->LEFT);
                    break;
             	case sf::Keyboard::Right:
                    game->getPlayer()->move(game->getPlayer()->RIGHT);
                    break;
                case sf::Keyboard::Space:
                    game->getPlayer()->move(game->getPlayer()->JUMP);
                    break;

                default:
					if(event.key.alt)
						devInput(event);
                    break;
            }
        }
        else if(event.type == sf::Event::KeyReleased) {
            switch(event.key.code) {
                case sf::Keyboard::Left:
                    game->getPlayer()->move(game->getPlayer()->LEFT_STOP);
                    break;
                case sf::Keyboard::Right:
                    game->getPlayer()->move(game->getPlayer()->RIGHT_STOP);
                    break;
                default:
                    // Default action
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

void Controller::devInput(sf::Event& e)
{
	switch (e.key.code)
	{
		//dev ctrls
	case sf::Keyboard::R: //water rising
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
	case sf::Keyboard::S: //reset player
		game->getPlayer()->setPosition(new b2Vec2(0,0));
		break;
	case sf::Keyboard::G: //gravity
		if(game->world->GetGravity().y == 0)
		{
			game->world->SetGravity(b2Vec2(0, -9.8)); 
			cout << "Gravity is turned ON." << endl;
		}
		else
		{
			game->world->SetGravity(b2Vec2(0, 0)); 
			cout << "Gravity is turned OFF." << endl;
		}
	default:
		break;
	}
}