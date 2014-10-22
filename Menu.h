#ifndef __Block_Frog__Menu__
#define __Block_Frog__Menu__

#include <iostream>
#include <SFML/Graphics.hpp>
#include "UDPNetwork.h"

class Textor;
class OSHandler;
class Game;
class Controller;
class StartMenu;

using namespace std;

class Menu
{
public:
    Menu(sf::RenderWindow* window, OSHandler* osHandler);
    void run();
    void update();
	Game* getGame() {return game;};
    StartMenu* getStartMenu() { return startMenu; }
private:
    void draw();
    
    Game* game;
    StartMenu* startMenu;
    Controller* controller;
    Textor* textor;
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
    
    friend Controller;
    
    // Server
    sf::IpAddress* a;
    string in;
    unsigned short p;
};

#endif /* defined(__Block_Frog__Menu__) */
