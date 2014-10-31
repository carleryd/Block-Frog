#include "Controller.h"
#include "Director.h"
#include "StartMenu.h"
#include "Lobby.h"

Controller::Controller(Game* game_, Director* director_)
{
    game = game_;
    director = director_;
	game->window->setKeyRepeatEnabled(false);
}

void Controller::checkInput() {
	sf::Event event;
    sf::Vector2i position;
    while (game->getWindow()->pollEvent(event))
    {
        // Resize view
        if(event.type == sf::Event::Resized)
            game->getWindow()->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
        
        // Close window : exit
        if (event.type == sf::Event::Closed) {
            game->getWindow()->close();
			game->exitGame();
        }

        // Escape pressed : exit
        if (event.type == sf::Event::KeyPressed && game->getPlayer() != nullptr) {
            switch(event.key.code) {
                case sf::Keyboard::Escape:
                    game->getWindow()->close();
                    break;
             	case sf::Keyboard::A:
                    director->getStartMenu()->setAPressed();
                    game->getPlayer()->move(game->getPlayer()->LEFT);
                    break;
             	case sf::Keyboard::D:
                    director->getStartMenu()->setDPressed();
                    game->getPlayer()->move(game->getPlayer()->RIGHT);
                    break;
                case sf::Keyboard::Space:
                    director->getStartMenu()->setSpacePressed();
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
        	else if(event.key.code == sf::Keyboard::E) {
	            position = sf::Mouse::getPosition(*game->getWindow());
    	        game->getPlayer()->useHook(position);
            }
			else if(event.key.code == sf::Keyboard::T) {
                // Use this for testing! :D
            }
        }
        else if(event.type == sf::Event::KeyReleased && game->getPlayer() != nullptr) {
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
        
        // Checks for Director.cpp
        if (event.type == sf::Event::TextEntered && director->joinAs == director->CLIENT) {
            // 8 is backspace | 10 is enter
            if(event.text.unicode == 8) {
                if(director->joinState == director->IP)
                    director->ipAddress = director->ipAddress.substr(0, director->ipAddress.size()-1);
                else if(director->joinState == director->PORT)
                    director->portAddress = director->portAddress.substr(0, director->portAddress.size()-1);
            }
			else if(event.text.unicode == 10 || event.text.unicode == 13) {
                if(director->joinState == director->IP)
                    director->joinState = director->PORT;
                else if(director->joinState == director->PORT)
//                    director->state = director->GAME;
                    director->state = LOBBY;
            }
            else {
                if(director->joinState == director->IP)
                    director->ipAddress += static_cast<char>(event.text.unicode);
                else if(director->joinState == director->PORT)
                    director->portAddress += static_cast<char>(event.text.unicode);
            }
        }
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            position = sf::Mouse::getPosition(*game->getWindow());
            for(Button button : director->getButtons()) {
                if(position.x > button.position.x - button.size.x/2 &&
                   position.x < button.position.x + button.size.x/2 &&
                   position.y > button.position.y - button.size.y/2 &&
                   position.y < button.position.y + button.size.y/2) {
                    if(button.function == "STARTBUTTON") {
                        cout << "CLICKED STARTBUTTON" << endl;
                        UDPNetwork* localHost = director->getLobby()->getLocalHost();
                    	if(localHost != nullptr && localHost->isServer()) {
                            cout << "SERVER CHANGING STATE" << endl;
                         	director->state = GAME;
                        }
                    }
                }
            }
        }
        
        if(game->getPlayer() != nullptr) {
            position = sf::Mouse::getPosition(*game->getWindow());
            game->getPlayer()->aimHook(position + game->viewOffset);
            
            if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                director->getStartMenu()->setLeftClickPressed();
                game->getPlayer()->useHook(position);
            }
            else if(sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                game->getPlayer()->releaseHook();
            }
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