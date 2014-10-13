#include "Menu.h"
#include "Textor.h"
#include "Game.h"
#include "Player.h"
#include "Controller.h"

Menu::Menu(sf::RenderWindow* window_, OSHandler* osHandler_) {
    window = window_;
    osHandler = osHandler_;
    
	textor = new Textor(osHandler);
    window->setKeyRepeatEnabled(false);
	gameStarted = false;
    ipAddress = "";
    portAddress = "";
    
    game = new Game(window, osHandler);
    menuState = START;
    joinState = IP;
    
	a = nullptr;
	p = 0;
    
    game->init();
    game->initStartMenu();
    controller = new Controller(game);
}


void Menu::run() {
    if(gameStarted) {
        if(menuState != GAME) {
            if(joinAs == SERVER) {
                game->init(SERVER);
//                controller = new Controller(game);
                menuState = GAME;
            }
            else if(joinAs == CLIENT) {
                game->init(CLIENT, new sf::IpAddress(ipAddress), atoi(portAddress.c_str()));
//                controller = new Controller(game);
                menuState = GAME;
            }
        }
        else {
	        game->run();
    	    controller->checkInput();
        }
    }
    else {
        draw();
        update();
    }
}

void Menu::draw() {
    switch(menuState) {
        case START:
            game->runStartMenu();
            controller->checkInput();
//            window->draw(textor->write("1. Host game", sf::Vector2f(window->getSize().y/2 + 100, 250)));
//            window->draw(textor->write("2. Join game", sf::Vector2f(window->getSize().y/2 + 100, 350)));
            break;
        case LOBBY:
            // Not yet implemented
            break;
        case JOIN:
            window->draw(textor->write("ENTER HOST'S IP ADDRESS",
                                       sf::Vector2f(window->getSize().y/2 + 100, 250)));
            if(joinState == IP)
	            window->draw(textor->write(ipAddress,
                                       sf::Vector2f(window->getSize().y/2 + 100, 350)));
            else if(joinState == PORT)
                window->draw(textor->write(portAddress,
                                           sf::Vector2f(window->getSize().y/2 + 100, 350)));
            break;
        default: ;
    }
}

void Menu::update() {
    while (game->getWindow()->pollEvent(event))
    {
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
		{
			window->close();
		}
        switch(menuState) {
            case START:
                if(game->getContactListener()->getHostGame()) {
                    game->removeStartMenu();
                    joinAs = SERVER;
                    gameStarted = true;
                }
                if(game->getContactListener()->getJoinGame()) {
                    game->removeStartMenu();
                    joinAs = CLIENT;
                    menuState = JOIN;
                    window->pollEvent(event);
                }
                
                if (event.type == sf::Event::KeyPressed) {
                    switch(event.key.code) {
                        case sf::Keyboard::Num1:
                            joinAs = SERVER;
                            gameStarted = true;
                            break;
                        case sf::Keyboard::Num2:
                            joinAs = CLIENT;
                            menuState = JOIN;
                            window->pollEvent(event);
                            break;
                        default: ;
                    }
                }
                break;
            case LOBBY:
                // Not yet implemented
                break;
            case JOIN:
                if (event.type == sf::Event::TextEntered) {
                    // 8 is backspace | 10 is enter
                    if(event.text.unicode == 8) {
                        if(joinState == IP)
                                ipAddress = ipAddress.substr(0, ipAddress.size()-1);
                        else if(joinState == PORT)
                                portAddress = portAddress.substr(0, portAddress.size()-1);
                    }
                    else if(event.text.unicode > 47 && event.text.unicode < 123) {
                        if(joinState == IP)
                            ipAddress += static_cast<char>(event.text.unicode);
                        else if(joinState == PORT)
                            portAddress += static_cast<char>(event.text.unicode);
                    }
                    else if(event.text.unicode == 10) {
                        if(joinState == IP)
                            joinState = PORT;
                        else if(joinState == PORT)
							gameStarted = true;
                    }
                    else
                        cout << "Invalid input: " << event.text.unicode << " Only checking for numbers and backspace" << endl;
                }
                break;
            default: ;
    	}
    }
}


