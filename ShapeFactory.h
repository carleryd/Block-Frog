#ifndef SHAPE_FACTORY_H
#define SHAPE_FACTORY_H

#include <Box2D/Box2D.h>
#include "Shape.h"
#include <random>
using namespace std;

class Game;

class ShapeFactory
{
public:
	ShapeFactory(Game* game);
	~ShapeFactory(void);
    
	template<class T>
	b2Vec2* sfvec_to_b2vec(sf::Vector2<T> v);
	Shape* createRectangle(b2Vec2* size, b2Vec2* position, bool dynamic, int _id);
	Shape* createRectangle(b2Vec2* size, b2Vec2* position, bool dynamic,
		float density = 1.0, float friction = 0.5);
	Shape* createRandomShape(sf::Vector2i viewOffset);
	
	void setGlobalID(int id_) {id = id_;};
	int getGlobalID() const {return id;};

private:
	//b2Vec2 convert_sf_vector(sf::Vector2i v);

	Game* game;
	mt19937 mersenneGen;
	uniform_real_distribution<float> dist;
	int minSize;
	int id;
};
#endif