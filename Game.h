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
class Lobby;
class Director;

//enum {SERVER, CLIENT, SINGLE_PLAYER};

class Game
{
public:
	Game(sf::RenderWindow* window, OSHandler* osHandler);
	~Game();
    void destroyPlayer();
    
    void basicInit();
    void init();
    void initStartMenu();
    void runStartMenu();
    void run();
    void removeStartMenu();
	void exitGame();

    void spawnBox(sf::Vector2i position);
    
    // Getter methods
	Textor* getTextor() {return textor;};
    sf::RenderWindow* getWindow() { return window; }
    b2World* getWorld() { return world; }
    Player* getPlayer() { return player; }
    Utility* getUtility() { return utility; }
    OSHandler* getOSHandler() { return osHandler; }
    ShapeFactory* getShapeFactory() { return shapeFactory; }
    Synchronizer* getSynchronizer() { return synchronizer; }

	/*
		get any one player, remote or local
		returns nullptr if not found
	*/
	Player* getPlayer(int localID);
	sf::Vector2i& getViewOffset() {return viewOffset;};
	list<Player*>& getRemotePlayers() {return remotePlayers;};
	//returns nullptr if not found
	Player* getRemotePlayer(int localID);
	vector<Shape*>& getShapes() {return boxes;};
	//returns nullptr if not found
	Shape* getShape(int id);
	Shape* getLastStaticShape() const {return lastStaticShape;};
	sf::Shape* getWater() const {return water;};
    ContactListener* getContactListener() { return contactListener; }
    Lobby* getLobby() { return lobby; }
    Director* getDirector() { return director; }
    bool getGameHasStarted() { return gameHasStarted; }
    int getLocalID() { return localID; }
    
    // Setter methods
    void setUtility(Utility* utility);
    void setPlayer(Player* player);
    void setStaticPlatform(Shape* shape) { lastStaticShape = shape; }
    void setElapsedPrepTime(int elapsedPrepTime_) { elapsedPrepTime = elapsedPrepTime_; }
    void setGameHasStarted(bool hasStarted) { gameHasStarted = hasStarted; }
    void setLobby(Lobby* lobby_) { lobby = lobby_; }
    void setDirector(Director* director_) { director = director_; }
    // Unique ID for every machine connected to game.
    void setLocalID(int localID_) { localID = localID_; }
    
    // Network
	void addRemotePlayer(Player* rPlayer);
	bool removeRemotePlayer(int localID);
	bool playersAllowedToJoined(){return allowJoin;};
	UDPNetwork* getLocalHost(){return localHost;};
    void setLocalHost(UDPNetwork* localHost_) { localHost = localHost_; }
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
    void createStaticPlatform();
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
    Lobby* lobby;
    Director* director;
    
    sf::RenderWindow* window;
	sf::View* view;
	sf::Vector2i viewOffset;
    sf::Clock clock;
    sf::Time timer;
	PacketParser* packetParser;
	Textor* textor;
	Remover* remover;
    
    // temporary
    Shape* staticPlatform;
    
    // This is for the live menu
    Shape* hostRectangle;
    Shape* joinRectangle;
    
    // Keep track of amount of players. This will affect the userData set for each player
    int playerAmount;

    // This score is incremented when new boxes are created in createBoxes()
    int score;
    bool gameHasStarted;
    
	//network
    int localID;
	UDPNetwork* localHost;
	std::thread* network;
    std::vector<shapeSync*> localChanges;
    std::clock_t start;	
	Synchronizer* synchronizer;

    std::vector<Shape*> boxes;
    std::vector<Shape*> staticShapes;
	Shape* lastStaticShape;
	
    // Water
    sf::Shape* water;
    // int values, 1 is start, for each new stage a new static platform is created
    int waterStage;
    int prevStage;
    float platformsPerLevel;
    int arrangeX;
    int prevArrangeX;
    sf::Vector2i oldViewOffset;
    
    // Holds timer.asSeconds() value. Used to create new boxes in intervals
	double duration;
    // Holds timer.asSeconds() value. Used to count time in preparation before game starts
    int prepTime;
    int elapsedPrepTime;
    int boxCount;
//    int gameLevel;
    
	bool rise;
	float riseSpeed;
	int secPerDrops; //time before a new block is dropped
	bool allowJoin;
	//float updateTime;
//	int staticPlatform; //every staticPlatform:th created box will be a static platform
	friend Controller;
	friend UDPNetwork;
};

#endif /* defined(__Block_Frog__Game__) */
