#ifndef ITEM_H 
#define ITEM_H
#include "rectangle.h"

class Player;

class Item :
	public Rectangle
{
public:
	Item(Game* game, b2Vec2* size, b2Vec2* position,
		bool dynamic, int id, float density = 1.0f, float friction = 1.0f, int groupIndex = 1);
	~Item(void);
	void causeEffect(Player* affectee);
};

#endif