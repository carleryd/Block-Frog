#ifndef REMOVER_H
#define REMOVER_H
#include <stack>

using namespace std;
class Shape;
class Player;
class Game;
class PacketParser;

class Remover
{
public:
	Remover(Game* game);
	~Remover(void);

	//remembers shapes that are to be killed
	void checkShapeRemoval(Shape* shape);
	//kills all the shapes that has been cought in checkShapeKill
	void removeShapes();
	void checkPlayerKill(Player* p);
	void respawnPlayer(Player* p);
private:
	float updateKillOffset();
	float updateStaleOffset();

	float killOffset;
	float staleOffset;
	stack<Shape*> deletion;
	PacketParser* packetParser;
	Game* game;
};

#endif