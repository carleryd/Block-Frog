#include "Lobby.h"
#include "Server.h"
#include "Client.h"
#include "Game.h"
#include "Textor.h"
#include "Synchronizer.h"
#include "Director.h"
#include "Controller.h"
#include <algorithm>

Lobby::Lobby(Game* game_, Director* director_) {
    game = game_;
    director = director_;
	window = game->getWindow();
    osHandler = game->getOSHandler();
  	textor = new Textor(osHandler);
    
    game->setLobby(this);
    
    colors = { "green", "orange", "blue", "yellow", "pink", "forest", "teal", "gray", "white" };
    localHost = nullptr;
}

Lobby::~Lobby() {}

void Lobby::init(int playerType, sf::IpAddress* serverip, unsigned short serverPort) {
    // Setting up network
    switch (playerType)
	{
        case SERVER:
        {
            localHost = new Server(game);
            window->setTitle("SERVER");
            
            LobbyPlayer host(1, "host", colors[0]);
            game->setLocalID(1);
            
            connected.push_back(host);
        }
            break;
        case CLIENT:
        {
            localHost = new Client(game, *serverip, serverPort);
            dynamic_cast<Client*>(localHost)->connect();
            window->setTitle("CLIENT");
            
            game->setLocalID(9999);
            
            // Get all connected lobby players and add this new client
            // This will also send a LOCAL_ID message to client sending this message
            game->getSynchronizer()->requestConnectedPlayers();
        }
            break;
        default:
            break;
	}
	network = new thread(&UDPNetwork::listen, localHost);
    
    game->setLocalHost(localHost);
    
    // Buttons
    Button startButton(sf::Vector2i(window->getSize().x/2, window->getSize().y-100),
                       sf::Vector2i(100, 50), "STARTBUTTON");
    buttons.push_back(startButton);
    director->addButton(startButton);
}

void Lobby::draw() {
    window->draw(textor->write("Lobby",
                               sf::Vector2f(window->getSize().x/2, 100)));
    
    for(int i = 0; i < connected.size(); ++i) {
        window->draw(textor->write(to_string(connected[i].ID),
                                   sf::Vector2f(1 * window->getSize().x/5, 200 + i*100)));
        window->draw(textor->write(connected[i].alias,
                                   sf::Vector2f(2 * window->getSize().x/5, 200 + i*100)));
        window->draw(textor->write(connected[i].color,
                                   sf::Vector2f(3 * window->getSize().x/5, 200 + i*100)));
    }
    if(localHost != nullptr && localHost->isServer()) {
	    window->draw(textor->write("START GAME",
                                   sf::Vector2f(window->getSize().x/2, window->getSize().y - 100)));
    }
}

void Lobby::update() {
    // Handle data that was received since last timestep
	localHost->handleReceivedData(game);
    
    
//	if(localHost->isServer()) {
//		
//    }
//    else {
//        
//    }
}

void Lobby::addPlayer(LobbyPlayer lp) {
	connected.push_back(lp);
    sort(connected.begin(), connected.end());
}

/*

 När spelare joinar, skicka servern info om andra spelare som joinat.
 Lokalt går client automatiskt in i lobby UI. Behöver info från servern om spelare
 	Namn, färg, redo + kanske mer
 Server broadcastar när spelet ska startas:
 	Server startar upp game som vanligt
 	Clienter startar game och requestar data som vanligt(kanske onödigt nu när allt skapas samtidigt)
 
 

*/