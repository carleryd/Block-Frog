#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H
#include "Game.h"
struct shapeSync;
struct player_info;

class Synchronizer
{
public:
	Synchronizer(Game& game);
	~Synchronizer(void);

	void updateShapes(shapeSync* s); 
	void updatePlayer(player_info* p);

	void requestShapeUpdates();
	void requestPlayerUpdates();
    void requestGameHasStarted();
    void requestConnectedPlayers();
private:
	b2Vec2 interpolate(const b2Vec2& oldV, const b2Vec2& newV);
	const float interpolate(float oldV, float newV);

	bool isWithinThresshold(b2Vec2& v);

	Game& game;
	float updateTime;
	b2Vec2 threshold;
    UDPNetwork* localHost;
};

#endif