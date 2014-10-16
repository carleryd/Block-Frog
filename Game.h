#ifndef __Block_Frog__Game__
#define __Block_Frog__Game__

#include <iostream>
#include <SFML/Graphics.hpp>
#include "UDPNetwork.h"
#include <Box2D/Box2D.h>
#include "Player.h"
#include <list>
#include <thread>
class Utility;
class Shape;
class ShapeFactory;
class Controller;
class OSHandler;
class UDPNetwork;
class Textor;
class ContactListener;
class Remover;
class Synchronizer;

enum {SERVER, CLIENT, SINGLE_PLAYER};

class Game
{
public:
	Game(sf::RenderWindow* window, OSHandler* osHandler);
	~Game();
    
    void basicInit();
    void init(int playerType, sf::IpAddress* serverip = nullptr, unsigned short serverPort = 0);
    void initStartMenu();
    void runStartMenu();
    void run();
    void removeStartMenu();
	void exitGame();

    void spawnBox(sf::Vector2i position);
    
    // Getter methods
    sf::RenderWindow* getWindow();
    b2World* getWorld();
    Player* getPlayer();
	/*
		get any one player, remote or local
		returns nullptr if not found
	*/
	Player* getPlayer(string name);
    Utility* getUtility();
	sf::Vector2i& getViewOffset() {return viewOffset;};
	list<Player*>& getRemotePlayers() {return remotePlayers;};
	//returns nullptr if not found
	Player* getRemotePlayer(string name);
	vector<Shape*>& getShapes() {return boxes;};
	//returns nullptr if not found
	Shape* getShape(int id);
	Shape* getLastStaticShape() const {return lastStaticShape;};
	sf::Shape* getWater() const {return water;};
    OSHandler* getOSHandler();
    ContactListener* getContactListener() { return contactListener; }
    
    // Setter methods
    void setUtility(Utility* utility);
    void setPlayer(Player* player);
    
    // Network
	void addRemotePlayer(Player* rPlayer);
	bool removeRemotePlayer(string name);
	bool playersAllowedToJoined(){return allowJoin;};
	UDPNetwork* getLocalHost(){return localHost;};
	PacketParser* getPacketParser() const {return packetParser;};
	//synchronize shapes against servers game state
	void updateShapes(shapeSync* s); 
	void updatePlayer(player_info* p);
	void removeShape(int id);
	bool exitCalled;
	void startRise();

private:
	void removeFallenBoxes(std::list<Shape*>& todelete);
	void calcViewOffset();

	void boxHandling(); //destroy boxes. 
	Shape* createBoxes(); //for server only. Returns pointer to last created box, nullptr if no box was created
	void handleThreads();
	void playerHandling();


	//if local player interacts with boxes save changes to localChanges
	void playerBoxInteraction();
	
    b2World* world;
    Player* player; 					// This is the player that the player controlls
    Utility* utility;					// Access this object for certain utility functions
	list<Player*> remotePlayers;
    OSHandler* osHandler;
	ShapeFactory* shapeFactory;
    ContactListener* contactListener;
    
    sf::RenderWindow* window;
	sf::View* view;
	sf::Vector2i viewOffset;
    sf::Clock clock;
    sf::Time timer;
	PacketParser* packetParser;
	Textor* textor;
	Remover* remover;
    
    // This is for the live menu
    Shape* hostRectangle;
    Shape* joinRectangle;
    
    // Keep track of amount of players. This will affect the userData set for each player
    int playerAmount;

    // This score is incremented when new boxes are created in createBoxes()
    int score;
    
	//network
	UDPNetwork* localHost;
	std::thread* network;
    std::vector<shapeSync*> localChanges;
    std::clock_t start;	
	Synchronizer* synchronizer;

    std::vector<Shape*> boxes;
	Shape* lastStaticShape;
	sf::Shape* water;
	double duration;
	bool rise;
	float riseSpeed;
	int secPerDrops; //time before a new block is dropped
	bool allowJoin;
	//float updateTime;
	int staticPlatform; //every staticPlatform:th created box will be a static platform
	friend Controller;
	friend UDPNetwork;
};

#endif /* defined(__Block_Frog__Game__) */
