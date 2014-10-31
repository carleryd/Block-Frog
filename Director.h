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
class Lobby;
struct Button;

using namespace std;

struct Button {
    Button(sf::Vector2i position_, sf::Vector2i size_, string function_)
    : position(position_), size(size_), function(function_) {}
    
    sf::Vector2i position;
    sf::Vector2i size;
    string function;
};

enum State {
    START,
    LOBBY,
    JOIN,
    GAME
};

class Director
{
public:
    Director(sf::RenderWindow* window, OSHandler* osHandler);
    void run();
    void update();
	Game* getGame() {return game;};
    StartMenu* getStartMenu() { return startMenu; }
    Lobby* getLobby() { return lobby; }
    vector<Button>& getButtons() { return buttons; }
    
    void addButton(Button& button) { buttons.push_back(button); }
    void setState(State state_) { state = state_; }
    
private:
    void draw();
    
    Game* game;
    Lobby* lobby;
    StartMenu* startMenu;
    Controller* controller;
    Textor* textor;
    OSHandler* osHandler;
    sf::RenderWindow* window;
	sf::Event event;
    
    vector<Button> buttons;
    
    enum Enter {
        IP,
        PORT
    };
    
    enum JoinAs {
        SERVER,
        CLIENT
    };
    
    bool inGame;
    bool inLobby;
    State state;
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
