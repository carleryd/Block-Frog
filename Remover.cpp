#include "Remover.h"
#include "Shape.h"
#include "Game.h"
#include "PacketParser.h"
#include "Server.h"
#include "Client.h"
#include "Textor.h"

Remover::Remover(Game* g):
	packetParser(g->getPacketParser()), game(g)
{
	killOffset = 60;
	staleOffset = 20;
}


Remover::~Remover(void)
{
}

void Remover::checkShapeRemoval(Shape* shape)
{
	//check for killing shape
	if(shape->getShape()->getPosition().y > updateKillOffset() && shape != game->getLastStaticShape())
	{
		deletion.push(shape);
//		cout << "shape killed at "<< updateKillOffset() << endl;
	}
	//check for making shape stale
	else if(shape->getShape()->getPosition().y > updateStaleOffset()) //make sure there is still a static platform to stand on
	{
		shape->makeStatic();
		sf::Packet p = packetParser->pack<int>(UDPNetwork::SHAPE_STATIC, shape->getId());
		dynamic_cast<Server*>(game->getLocalHost())->broadCast(p);
	}

}

void Remover::checkPlayerKill(Player* player)
{
	if(player->getBox()->getShape()->getPosition().y > updateKillOffset()
		&& !player->isDead())
	{
		string s = player->getName() + " is now sleeping with the fishes.";
		player->setDeath(true);
		sf::Vector2f v = sf::Vector2f(game->getWindow()->getSize().x/2, game->getWindow()->getSize().y/2);
		v += sf::Vector2f(game->getViewOffset());
		game->getTextor()->writeTemporaryText(s, v, 3);
		
		sf::Packet p = packetParser->pack(UDPNetwork::PLAYER_DEAD);
		if(game->getLocalHost()->isServer())
		{
			dynamic_cast<Server*>(game->getLocalHost())->broadCast(p);
		}
		else
		{
			dynamic_cast<Client*>(game->getLocalHost())->sendToServer(p);
		}
	}
}

void Remover::removeShapes()
{
	while(!deletion.empty())
	{
		Shape* del = deletion.top();
		auto& boxes = game->getShapes();
		vector<Shape*>::iterator todelete = std::find_if(boxes.begin(), boxes.end(), [del](Shape* b)
		{
			return del == b;
		}
		);
		//if found delete
		if(todelete != boxes.end())
		{
			sf::Packet p = packetParser->pack<int>(UDPNetwork::REMOVE_SHAPE, (*todelete)->getId());
			dynamic_cast<Server*>(game->getLocalHost())->broadCast(p);

			delete deletion.top();
			boxes.erase(todelete);
			deletion.pop();
		}
		else
			deletion.pop();
	}
}

float Remover::updateKillOffset()
{
	return updateStaleOffset() + killOffset;
}

float Remover::updateStaleOffset()
{
	return game->getWater()->getPosition().y;
}

void Remover::respawnPlayer(Player* player)
{
	if(player->isDead())
	{
		b2Vec2 spawn = game->getLastStaticShape()->getBody()->GetPosition();
		spawn.y += 10;
		player->resetPlayer(&spawn);

		/*string s = player->getName() + " has returned from the grave.";
		sf::Vector2f v = sf::Vector2f(game->getWindow()->getSize().x/2, game->getWindow()->getSize().y/2);
		v += sf::Vector2f(game->getViewOffset());
		game->getTextor()->writeTemporaryText(s, v, 3);*/

		player->setDeath(false);
        
		sf::Packet p = packetParser->pack(UDPNetwork::PLAYER_RES);
        p << player->getName();
        p << spawn.x;
        p << spawn.y;
        
		UDPNetwork* lh = game->getLocalHost();
		if(lh->isServer())
		{
			dynamic_cast<Server*>(lh)->broadCast(p);
		}
		else
		{
			dynamic_cast<Client*>(lh)->sendToServer(p);
		}
	}
}