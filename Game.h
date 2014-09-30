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
class Shape;
class ShapeFactory;
class Controller;
class OSHandler;
class UDPNetwork;

enum {SERVER, CLIENT, SINGLE_PLAYER};

class Game
{
public:
	Game(sf::RenderWindow* window, OSHandler* osHandler, int playerType, sf::IpAddress* serverip = nullptr, unsigned short serverPort = 0);
	~Game();
    void run();
    void init();

    void spawnBox(sf::Vector2i position);
    
    sf::RenderWindow* getWindow();
    b2World* getWorld();
    Player* getPlayer();
	sf::Vector2i& getViewOffset() {return viewOffset;};
	list<Player*>& getRemotePlayers() {return remotePlayers;};
    OSHandler* getOSHandler();
    
    float getPixelToMeter();
    float getMeterToPixel();
    float getOffSetX();
    float getOffSetY();
    
	void addRemotePlayer(Player* rPlayer);
	bool playersAllowedToJoined(){return allowJoin;};
	UDPNetwork* getLocalHost(){return localHost;};
	PacketParser* getPacketParser() const {return packetParser;};

private:
	void removeFallenBoxes(std::list<Shape*>& todelete);
	void calcViewOffset();

	void boxHandling(); //destroy boxes. 
	Shape* createBoxes(); //for server only. Returns pointer to last created box
	void handleThreads();
	
    b2World* world;
    Player* player; //this is the player that the player controlls
	list<Player*> remotePlayers; //
    OSHandler* osHandler;
	ShapeFactory* shapeFactory;
    
    sf::RenderWindow* window;
	sf::View* view;
	sf::Vector2i viewOffset;
    sf::Clock clock;
    sf::Time timer;
	PacketParser* packetParser;

	//network
	UDPNetwork* localHost;
	std::thread* network;
	std::thread* join;
    
    std::vector<Shape*> boxes;
	double duration;
	float riseSpeed;
    float pixelToMeter;
    float meterToPixel;
    float offSetX;
    float offSetY;
	int secPerDrops; //time before a new block is dropped
	int killOffset; //how far under the screen blocks will be killed
	bool allowJoin;
	friend Controller;
	friend UDPNetwork;
};

#endif /* defined(__Block_Frog__Game__) */
