#ifndef __Block_Frog__Menu__
#define __Block_Frog__Menu__

#include <iostream>
#include <SFML/Graphics.hpp>
#include "UDPNetwork.h"

class Textor;
class OSHandler;
class Game;
class Controller;

using namespace std;

class Menu
{
public:
    Menu(sf::RenderWindow* window, OSHandler* osHandler);
    void run();
    void update();
private:
    void draw();
    
    Textor* textor;
    Game* game;
    Controller* controller;
    OSHandler* osHandler;
    sf::RenderWindow* window;
	sf::Event event;
    
	enum State {
        START,
        LOBBY,
        JOIN,
        GAME
    };
    
    enum Enter {
        IP,
        PORT
    };
    
    enum JoinAs {
        SERVER,
        CLIENT
    };
    
    bool gameStarted;
    State menuState;
    Enter joinState;
	JoinAs joinAs;
    string ipAddress;
    string portAddress;
    string temp;
    
    // Server
    sf::IpAddress* a;
    string in;
    unsigned short p;
};

#endif /* defined(__Block_Frog__Menu__) */
