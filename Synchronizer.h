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
private:
	b2Vec2 interpolate(const b2Vec2& oldV, const b2Vec2& newV);
	const float interpolate(float oldV, float newV);

	Game& game;
	float updateTime;
};

#endif