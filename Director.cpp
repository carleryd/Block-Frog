#include "Director.h"
#include "Textor.h"
#include "Game.h"
#include "Player.h"
#include "Controller.h"
#include "StartMenu.h"
#include "Lobby.h"
#include "Server.h"

Director::Director(sf::RenderWindow* window_, OSHandler* osHandler_) {
    window = window_;
    osHandler = osHandler_;
    
	textor = new Textor(osHandler);
    window->setKeyRepeatEnabled(false);
	inGame = false;
    inLobby = false;
    ipAddress = "";
    portAddress = "";
    
    game = new Game(window_, osHandler_);
    game->setDirector(this);
    lobby = new Lobby(game, this);
    
    state = START;
    joinState = IP;
    
	a = nullptr;
	p = 0;
    
    startMenu = new StartMenu(game);
    controller = new Controller(game, this);
}

void Director::run() {
    update();
    draw();
}

void Director::draw() {
    switch(state) {
        case START:
            break;
        case LOBBY:
            lobby->draw();
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

void Director::update() {
    switch(state) {
        case START:
            startMenu->run();
            controller->checkInput();
            
            if(game->getContactListener()->getHostGame()) {
                cout << "HOOKED HOST GAME" << endl;
                delete startMenu;
                joinAs = SERVER;
                state = LOBBY;
            }
            else if(game->getContactListener()->getJoinGame()) {
                cout << "HOOKED JOIN GAME" << endl;
                delete startMenu;
                joinAs = CLIENT;
                state = JOIN;
            }
            break;
        case LOBBY:
            if(!inLobby) {
                if(joinAs == SERVER) {
                    lobby->init(SERVER);
                }
                else if(joinAs == CLIENT) {
                    lobby->init(CLIENT, new sf::IpAddress(ipAddress), atoi(portAddress.c_str()));
                }
                inLobby = true;
            }
            controller->checkInput();
            lobby->update();
            break;
        case JOIN:
            controller->checkInput();
            break;
        case GAME:
        {
            if(joinAs == SERVER) {
                sf::Packet packet = game->getPacketParser()->pack(UDPNetwork::GAME_STARTED);
                dynamic_cast<Server*>(lobby->getLocalHost())->broadCast(packet);
            }
			if(!inGame) {
                game->init();
                inGame = true;
            }
            game->run();
            controller->checkInput();
        }
            break;
        default: ;
    }
}