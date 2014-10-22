#include "Menu.h"
#include "Textor.h"
#include "Game.h"
#include "Player.h"
#include "Controller.h"
#include "StartMenu.h"

Menu::Menu(sf::RenderWindow* window_, OSHandler* osHandler_) {
    window = window_;
    osHandler = osHandler_;
    
	textor = new Textor(osHandler);
    window->setKeyRepeatEnabled(false);
	gameStarted = false;
    ipAddress = "";
    portAddress = "";
    
    game = new Game(window_, osHandler_);
    menuState = START;
    joinState = IP;
    
	a = nullptr;
	p = 0;
    
//    game->basicInit();
//    game->initStartMenu();
    startMenu = new StartMenu(game);
    controller = new Controller(game, this);
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
//            window->draw(textor->write("1. Host game", sf::Vector2f(window->getSize().y/2 + 100, 250)));
//            window->draw(textor->write("2. Join game", sf::Vector2f(window->getSize().y/2 + 100, 350)));
            break;
        case LOBBY:
            // Not yet implemented
            break;
        case JOIN:
            if(joinState == IP) {
                window->draw(textor->write("ENTER HOST'S IP ADDRESS",
                                           sf::Vector2f(window->getSize().x/2, 250)));
	            window->draw(textor->write(ipAddress,
                                           sf::Vector2f(window->getSize().x/2, 350)));
                
            }
            else if(joinState == PORT) {
                window->draw(textor->write("ENTER HOST'S PORT",
                                           sf::Vector2f(window->getSize().x/2, 250)));
                window->draw(textor->write(portAddress,
                                           sf::Vector2f(window->getSize().x/2, 350)));
            }
            break;
        default: ;
    }
}

void Menu::update() {
    switch(menuState) {
        case START:
//            game->runStartMenu();
            startMenu->run();
            controller->checkInput();
            
            if(game->getContactListener()->getHostGame()) {
//                game->removeStartMenu();
                delete startMenu;
                joinAs = SERVER;
                gameStarted = true;
                cout << "HOOKED HOST" << endl;
            }
            else if(game->getContactListener()->getJoinGame()) {
//                game->removeStartMenu();
                delete startMenu;
                joinAs = CLIENT;
                menuState = JOIN;
                cout << "HOOKED JOIN" << endl;
            }
            break;
        case LOBBY:
            // Not yet implemented
            break;
        case JOIN:
            controller->checkInput();
            // ipAddress& portAddress& joinstate
//                if (event.type == sf::Event::TextEntered) {
//                    // 8 is backspace | 10 is enter
//                    if(event.text.unicode == 8) {
//                        if(joinState == IP)
//                                ipAddress = ipAddress.substr(0, ipAddress.size()-1);
//                        else if(joinState == PORT)
//                                portAddress = portAddress.substr(0, portAddress.size()-1);
//                    }
//                    else if(event.text.unicode > 47 && event.text.unicode < 123) {
//                        if(joinState == IP)
//                            ipAddress += static_cast<char>(event.text.unicode);
//                        else if(joinState == PORT)
//                            portAddress += static_cast<char>(event.text.unicode);
//                    }
//                    else if(event.text.unicode == 10) {
//                        if(joinState == IP)
//                            joinState = PORT;
//                        else if(joinState == PORT)
//							gameStarted = true;
//                    }
//                    else
//                        cout << "Invalid input: " << event.text.unicode << " Only checking for numbers and backspace" << endl;
//                }
            break;
        default: ;
    }
}


