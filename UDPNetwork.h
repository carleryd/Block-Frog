#ifndef  UDPNETWORK_H
#define UDPNETWORK_H
#include <SFML/Network.hpp>
#include <vector>
#include "ShapeFactory.h"
#include "PacketParser.h"
#include <list>
#include <atomic>
class Game;
using namespace std;

/*
	HOW TO SEND DATA THE FIRST TIME
	1. SEND PLAYER NAME TO SERVER
	2. SERVER RECEVIES AND RESPONDES WITH "WELCOME TO [HOST NAME]'S SERVER"
	HOW TO SEND DATA _EVERY_ _OTHER_ _TIME_!
	1. send packet containing:
		1st. int (enum) telling receiver what kind of data is to be expected.
		2nd. The rest of the data. IMPORTANT: must be unpacked the same way it was packed. 
	2. let receiver handle expceted data in predetermined way.
*/

struct packetInfo
{
	sf::Packet packet;
	sf::IpAddress senderAddress;
	unsigned short senderPort;
};

class UDPNetwork
{
public:
	~UDPNetwork();
	int send(sf::Packet packet, sf::IpAddress& remoteAddress, unsigned short& remotePort);
	/*
		This function is for when you want to know who the sender is
	*/
	int receive(sf::Packet* packet, sf::IpAddress& remoteAddress, unsigned short& remotePort);
	/*
		This function is used when you'll accept data no matter the sender
	*/
	int receive(sf::Packet* packet);
	virtual bool isServer() = 0;
	sf::IpAddress getMyAddress() const{return myAddress;};
	unsigned short getMyPort() const{return mySocket.getLocalPort();};
	//while loop that listens for 
	void listen();
	void handleReceivedData(Game* game);
	bool exit;

	enum typreceive{SERVER_EXIT, CLIENT_EXIT, NEW_PLAYER,
		SHAPE, ALL_SHAPES, SHAPE_SYNCH,
		PLAYER_MOVE};
protected:
	UDPNetwork(string playerName, ShapeFactory& factory);

	list<packetInfo> packets;

	sf::UdpSocket mySocket;
	sf::IpAddress myAddress;
	std::string playerName;
	PacketParser packetParser;
	sf::SocketSelector selector;
private:
	std::atomic<bool> packetsOccupied;
};



#endif