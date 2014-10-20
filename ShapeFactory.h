#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Box2D/Box2D.h>
#include "Shape.h"
#include <random>
using namespace std;

class Game;
class Utility;
class Item;

class ShapeFactory
{
public:
	ShapeFactory(Game* game);
	~ShapeFactory(void);
    
	template<class T>
	b2Vec2* sfvec_to_b2vec(sf::Vector2<T> v);
	//
	Shape* createClientRectangle(b2Vec2* size, b2Vec2* position, bool dynamic, int id, uintptr_t collisionID);
	Shape* createRectangle(b2Vec2* size, b2Vec2* position, bool dynamic,
		float density = 1.0, float friction = 0.5);
	Shape* createRandomShape(sf::Vector2i& viewoffset, bool dynamic = true, uintptr_t collisionID = 99);
	Shape* createItem(b2Vec2* position, int _id = -1);
	
	void setGlobalID(int id_) {id = id_;};
	//returns id++
	int getGlobalID() {return id++;};

private:
	

	Game* game;
	mt19937 mersenneGen;
	uniform_real_distribution<float> dist;
	int minSize;
	int id;
	Utility* utility;
};
#endif