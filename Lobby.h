#ifndef __Block_Frog__Lobby__
#define __Block_Frog__Lobby__

#include <iostream>
#include <SFML/Graphics.hpp>
#include "UDPNetwork.h"
#include <thread>

class Game;
class Director;
class OSHandler;
class Textor;
class Player;
struct Button;

enum { SERVER, CLIENT, SINGLE_PLAYER };

struct LobbyPlayer {
    LobbyPlayer(int ID_, string alias_, string color_) : ID(ID_), alias(alias_), color(color_) {}
    int ID;
    string alias;
    string color;
    
    bool operator < (const LobbyPlayer& lp) const {
        return (ID < lp.ID);
    }
};

class Lobby
{
public:
	Lobby(Game* game_, Director* director_);
    ~Lobby();
    
    void init(int playerType, sf::IpAddress* serverip = nullptr, unsigned short serverPort = 0);
    void run();
    void draw();
    void update();
    
    void addPlayer(LobbyPlayer lp);
    
    vector<LobbyPlayer>& getConnected() { return connected; }
    vector<string> getColors() { return colors; }
    UDPNetwork* getLocalHost() { return localHost; }
    thread* getNetwork() { return network; }
    
private:
    // Class objects
    Game* game;
    Director* director;
    OSHandler* osHandler;
    Textor* textor;
    
    // SFML
    sf::RenderWindow* window;
    
    // Local
    vector<LobbyPlayer> connected;
    vector<Button> buttons;
    vector<string> colors;
    
    // Network
	UDPNetwork* localHost;
	std::thread* network;
};

#endif /* defined(__Block_Frog__Lobby__) */
