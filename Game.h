#ifndef __Block_Frog__Game__
#define __Block_Frog__Game__

#include <iostream>
#include <SFML/Graphics.hpp>
#include "UDPNetwork.h"
#include <Box2D/Box2D.h>
#include "Player.h"
#include <list>
#include <thread>
#include <typeinfo>
class Utility;
class Shape;
class ShapeFactory;
class Controller;
class OSHandler;
class UDPNetwork;
class Textor;
class ContactListener;

enum {SERVER, CLIENT, SINGLE_PLAYER};

class Game
{
public:
	Game(sf::RenderWindow* window, OSHandler* osHandler);
	~Game();
    
    void init(int playerType, sf::IpAddress* serverip = nullptr, unsigned short serverPort = 0);
    void run();
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

private:
	void removeFallenBoxes(std::list<Shape*>& todelete);
	void calcViewOffset();

	void boxHandling(); //destroy boxes. 
	Shape* createBoxes(); //for server only. Returns pointer to last created box, nullptr if no box was created
	void handleThreads();
	void playerHandling();
	void requestShapeUpdates();
	void requestPlayerUpdates();

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
    
    // Keep track of amount of players. This will affect the userData set for each player
    int playerAmount;

	//network
	UDPNetwork* localHost;
	std::thread* network;
	std::thread* join;
    
    std::vector<Shape*> boxes;
	list<Shape*> deletion;
	std::vector<shapeSync*> localChanges;
	sf::Shape* water;
	double duration;
	float riseSpeed;
	int secPerDrops; //time before a new block is dropped
	int killOffset; //how far under the screen blocks will be killed
	bool allowJoin;
	float updateTime;
	friend Controller;
	friend UDPNetwork;
};

#endif /* defined(__Block_Frog__Game__) */
