#include "Synchronizer.h"
#include "Client.h"
#include "Server.h"
#include <Box2D\Box2D.h>

Synchronizer::Synchronizer(Game& g):
	game(g)
{
	updateTime = 0.2;
}


Synchronizer::~Synchronizer(void)
{
}

void Synchronizer::updateShapes(shapeSync* s)
{
	vector<Shape*>& boxes = game.getShapes();
	int id = s->shapeID;
	vector<Shape*>::iterator i = find_if(boxes.begin(), boxes.end(), [id](Shape* s)
	{
		return s->getId() == id;
	});
	if(i != boxes.end())
	{
		Shape* shape = *i;
		shape->getBody()->SetAngularVelocity(s->angularVel);
		//shape->getBody()->SetLinearVelocity(s->velocity);
		//shape->setPosition(&s->position, s->angle);
		shape->getBody()->SetLinearVelocity(interpolate(shape->getBody()->GetLinearVelocity() ,s->velocity));
		shape->setPosition( new b2Vec2( interpolate( shape->getBody()->GetPosition(), s->position )), s->angle);
		shape->getBody()->GetFixtureList()->SetUserData((void*)(uintptr_t)s->hookUserData);
		shape->resetUpdateClock();
	}
	else
	{
		/*cout << "Shapes in vector boxes: ";
		for (Shape* shape : boxes)
		{
			cout << shape->getId() << " ";
		}
		cout << endl;
		cout << "Shape not found for update! ID: " << s->shapeID << endl;*/
		/*cout << "Creating new shape with ID " << s->shapeID << endl;
		Shape* replacement = shapeFactory->createRectangle(&s->size, &s->position, true);
		replacement->setId(s->shapeID);
		replacement->setPosition(&s->position, s->angle);
		replacement->getBody()->SetAngularVelocity(s->angularVel);
		replacement->getBody()->SetLinearVelocity(s->velocity);
		boxes.push_back(replacement);*/
	}
}

void Synchronizer::updatePlayer(player_info* p)
{
	Player* player = game.getRemotePlayer(p->name);
	if(player != nullptr)
	{
		//cout << "Updating player " << player->getName() << endl;
		player->setPosition(&p->position);
		player->getBody()->SetLinearVelocity(p->velocity);
		//update player hook
		//Circle * hookTip = player->getHookTip();
		//hookTip->setPosition(&p->hookTip.position, p->hookTip.angle);
		/*hookTip->getBody()->SetLinearVelocity(p->hookTip.velocity);
		hookTip->getBody()->SetAngularVelocity(p->hookTip.angularVel);*/
		//Rectangle* hookbase = player->getHookBase();
		//hookbase->setPosition(&p->hookBase.position, p->hookBase.angle);
		/*Circle * hookTip = player->getHookTip();
		hookTip->setPosition(&p->hookTip.position, p->hookTip.angle);
		/*hookTip->getBody()->SetLinearVelocity(p->hookTip.velocity);
		hookTip->getBody()->SetAngularVelocity(p->hookTip.angularVel);
		Rectangle* hookbase = player->getHookBase();
		hookbase->setPosition(&p->hookBase.position, p->hookBase.angle);*/
		player->getBox()->resetUpdateClock();
	}
	/*else
		cerr << "Could not find player \'" << p->name << "\'" << endl;*/
}

void Synchronizer::requestShapeUpdates()
{
	//shapes
	vector<Shape*>& boxes = game.getShapes();
	vector<Shape*>::iterator i = game.getShapes().begin()+1;
	for(;i != boxes.end(); ++i)
	{
		Shape* s = *i;
		if(s != nullptr && s->timeSinceUpdate().asSeconds() > updateTime)
		{
			//cout << "requesting synch data for shape "<< s->getId() << endl;
			sf::Packet request = game.getPacketParser()->pack<int>(UDPNetwork::SHAPE_SYNCH_REQUEST, s->getId());
			dynamic_cast<Client*>(game.getLocalHost())->sendToServer(request);
		}
	}
}

void Synchronizer::requestPlayerUpdates()
{
	//remote players
	list<Player*>& rPlayers = game.getRemotePlayers();
	list<Player*>::iterator listItr = rPlayers.begin();
	for(; listItr != rPlayers.end(); ++listItr)
	{
		Player* p = *listItr;
		if(p != nullptr && p->getBox()->timeSinceUpdate().asSeconds() > updateTime)
		{
			//cout << "Request update for player: " << p->getName() << endl;
			sf::Packet request = game.getPacketParser()->pack<string>(UDPNetwork::PLAYER_SYNCH_REQUEST, p->getName());
			if(game.getLocalHost()->isServer())
			{
				Server* server = dynamic_cast<Server*>(game.getLocalHost());
				client* receiver = server->getClient(p->getName());
				game.getLocalHost()->send(request, receiver->clientAddress, receiver->clientPort);
			}
			else
			{
				dynamic_cast<Client*>(game.getLocalHost())->sendToServer(request);
			}
		}
	}
}

b2Vec2 Synchronizer::interpolate(const b2Vec2& oldV, const b2Vec2& newV)
{
	return b2Vec2(0.5*oldV + 0.5*newV);
}

const float Synchronizer::interpolate(float o, float n)
{
	return o*0.5 + 0.5*n;
}