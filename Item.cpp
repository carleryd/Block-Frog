#include "Item.h"
#include "Player.h"

Item::Item(Game* game, b2Vec2* size, b2Vec2* position,
		bool dynamic, int id, float density, float friction, int groupIndex):
Rectangle(game, size, position, dynamic, id, density, friction, groupIndex)
{
	shape->setFillColor(sf::Color::Green);
	userData.isItem = true;
}


Item::~Item(void)
{

}

void Item::causeEffect(Player* affectee)
{
	//add randomness and more effects
	affectee->jumpHeight *= 2;
}