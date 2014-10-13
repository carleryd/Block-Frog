#include "Controller.h"
#include "Menu.h"

Controller::Controller(Game* game_, Menu* menu_)
{
    game = game_;
    menu = menu_;
	game->window->setKeyRepeatEnabled(false);
}

void Controller::checkInput() {
	sf::Event event;
    sf::Vector2i position;
    while (game->getWindow()->pollEvent(event))
    {
        // Close window : exit
        if (event.type == sf::Event::Closed) {
            game->getWindow()->close();
			/*if(!game->exitCalled)
				game->exitGame();*/
        }

        // Escape pressed : exit
        if (event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
                case sf::Keyboard::Escape:
                    game->getWindow()->close();
                    break;
             	case sf::Keyboard::A:
                    game->getPlayer()->move(game->getPlayer()->LEFT);
                    break;
             	case sf::Keyboard::D:
                    game->getPlayer()->move(game->getPlayer()->RIGHT);
                    break;
                case sf::Keyboard::Space:
                    game->getPlayer()->move(game->getPlayer()->JUMP);
                    break;
				case sf::Keyboard::J:
					game->allowJoin = false;
					cout << "player join false" << endl;
					break;
                default:
					if(event.key.alt)
						devInput(event);
                    break;
            }
        	if(event.key.code == sf::Keyboard::Q)
 	           game->getPlayer()->releaseHook();
        	if(event.key.code == sf::Keyboard::E) {
	            position = sf::Mouse::getPosition(*game->getWindow());
    	        game->getPlayer()->useHook(position);
            }
        }
        else if(event.type == sf::Event::KeyReleased) {
            switch(event.key.code) {
                case sf::Keyboard::A:
                    game->getPlayer()->move(game->getPlayer()->LEFT_STOP);
                    break;
                case sf::Keyboard::D:
                    game->getPlayer()->move(game->getPlayer()->RIGHT_STOP);
                    break;
                default:
                    // Default action
                    break;
            }
        }
        
        // Checks for Menu.cpp
        if (event.type == sf::Event::TextEntered && !menu->gameStarted) {
            // 8 is backspace | 10 is enter
            if(event.text.unicode == 8) {
                if(menu->joinState == menu->IP)
                    menu->ipAddress = menu->ipAddress.substr(0, menu->ipAddress.size()-1);
                else if(menu->joinState == menu->PORT)
                    menu->portAddress = menu->portAddress.substr(0, menu->portAddress.size()-1);
            }
			else if(event.text.unicode == 10 || event.text.unicode == 13) {
                if(menu->joinState == menu->IP)
                    menu->joinState = menu->PORT;
                else if(menu->joinState == menu->PORT)
                    menu->gameStarted = true;
            }
            else {
                if(menu->joinState == menu->IP)
                    menu->ipAddress += static_cast<char>(event.text.unicode);
                else if(menu->joinState == menu->PORT)
                    menu->portAddress += static_cast<char>(event.text.unicode);
            }
        }
        
        position = sf::Mouse::getPosition(*game->getWindow());
		game->getPlayer()->aimHook(position + game->viewOffset);
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
	        position = sf::Mouse::getPosition(*game->getWindow());
            game->getPlayer()->useHook(position);
        }
        else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            game->getPlayer()->releaseHook();
        }
    }
}

void Controller::devInput(sf::Event& e)
{
	switch (e.key.code)
	{
		//dev ctrls
	case sf::Keyboard::R: //water rising
		//game->rise = !game->rise;
		game->startRise();
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