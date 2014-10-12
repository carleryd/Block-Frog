#include "Game.h"
#include "OSHandler.h"
#include "Server.h"
#include "Client.h"
#include "ShapeFactory.h"
#include "Utility.h"
#include "Textor.h"
#include "ContactListener.h"

using namespace std;

Game::Game(sf::RenderWindow* window_, OSHandler* osHandler_)
{
    window = window_;
    osHandler = osHandler_;
    
	view = new sf::View();
	view->setCenter((float)window->getSize().x/2, (float)window->getSize().y/2);
	view->setSize(sf::Vector2f(window->getSize()));
	window->setView(*view);
    
    // Setting up Box2D physics
    // Define the gravity vector.
    b2Vec2 gravity(0.0f, -9.8f);
    
    // Construct a world object, which will hold and simulate the rigid bodies.
    world = new b2World(gravity);
	water = new sf::RectangleShape(sf::Vector2f(float(window->getSize().x * 2), float(window->getSize().y)));
	water->setFillColor(sf::Color(0, 0, 255, 100));	
	textor = new Textor(osHandler_);
	exitCalled = false;
	updateTime = 0.5;
}

Game::~Game()
{
	while(!boxes.empty())
	{
		delete boxes.back();
		boxes.pop_back();
	}
	while(!remotePlayers.empty())
	{
		delete remotePlayers.back();
		remotePlayers.pop_back();
	}
	delete world;
	delete player;
	delete view;
	delete shapeFactory;
	//network->~thread();
}

void Game::init(int playerType, sf::IpAddress* serverAddress, unsigned short serverPort) {
	//shapefactory for creating shapes easily
	shapeFactory = new ShapeFactory(this);
    utility = new Utility(this);
    
    contactListener = new ContactListener();
    world->SetContactListener(contactListener);
    
	player = new Player(this);
    player->init(player);
	playerAmount = 1;
    
	packetParser = new PacketParser(*shapeFactory);
	
    boxes.push_back(shapeFactory->createRectangle(new b2Vec2(750.0f, 50.0f),
                                                  new b2Vec2(0.0f, -float(window->getSize().y)/2),
                                                  false,
                                                  1.0,
                                                  1.0)
                    );
    
	riseSpeed = 0; //-0.2f;
	killOffset = 30;
	secPerDrops = 1;
	allowJoin = true;
    
	//networking
	switch (playerType)
	{
        case SERVER:
            localHost = new Server("HOST", *shapeFactory, this);
            player->setName("HOST");
            //join = new thread(&Server::waitForPlayers, dynamic_cast<Server*>(localHost), std::ref(allowJoin));
            //dynamic_cast<Server*>(localHost)->waitForPlayers();
            cout << "Waiting for players..." << endl;
            window->setTitle("SERVER");
            break;
        case CLIENT:
            localHost = new Client("CLIENT", *serverAddress, serverPort, *shapeFactory);
            player->setName("CLIENT");
            dynamic_cast<Client*>(localHost)->connect(player->getPosition());
            window->setTitle("CLIENT");
            break;
        case SINGLE_PLAYER:
            localHost = new Server("HOST", *shapeFactory, this);
            player->setName("HOST");
            window->setTitle("SINGLE PLAYER");
            break;
        default:
            break;
	}
	network = new thread(&UDPNetwork::listen, localHost);
	
	window->setActive(true);
	
	secPerDrops = 5;
}

sf::RenderWindow* Game::getWindow() { return window; }
b2World* Game::getWorld() { return world; }
Player* Game::getPlayer() { return player; }
Utility* Game::getUtility() { return utility; }
OSHandler* Game::getOSHandler() { return osHandler; }

void Game::setUtility(Utility* utility_) { utility = utility_; }
void Game::setPlayer(Player* player_) { player = player_; }

void Game::run() {
	//Handle data that was received since last timestep
	localHost->handleReceivedData(this);

	boxHandling();
	calcViewOffset();
	playerBoxInteraction();

	//water
	water->setPosition(float(window->getSize().x * -0.1), float(window->getSize().y - viewOffset.y -10));
	window->draw(*water);

    if(localHost->isServer())
    {
        Shape* box = createBoxes();
        if(box != nullptr)
        {
            sf::Packet packet = packetParser->pack(box);
            dynamic_cast<Server*>(localHost)->broadCast(packet);
        }
    }
	//else 
	//{
	//	boxes.push_back( static_cast<Shape*>(dynamic_cast<Client*>(localHost)->listenToServer()));
	//}

	//player
	playerHandling();

	//move view up/raise water level
	view->move(0, riseSpeed);
	window->setView(*view);
    
    // timeStep, velocityIterations, positionIterations
    world->Step(1.0f/60.0f, 10, 8);
    world->ClearForces();

	//do  networking that has happened during this frame
	//e.g. send data that player has moved a block


	//rebroadcast everything that server has learnt from the other clients about the game state
	if(localHost->isServer())
	{
		Server* server = dynamic_cast<Server*>(localHost);

		//broadcast shapes that server has affected
		for (shapeSync* shapeSync : localChanges)
		{
			sf::Packet p = packetParser->pack(shapeSync);
			server->broadCast(p);
		}
	}
	else
	{
		//send local changes to server
		for (shapeSync* shapeSync : localChanges)
		{
			sf::Packet p = packetParser->pack(shapeSync);
			dynamic_cast<Client*>(localHost)->sendToServer(p);
		}
		//request server update on shapes
		requestShapeUpdates();
	}
	requestPlayerUpdates();
	//clear memory that is only valid this time step.
	while(!localChanges.empty())
	{
		delete localChanges.back();
		localChanges.pop_back();
	}
}

void Game::spawnBox(sf::Vector2i position) {
    // adjPos is a position adjusted to window position, also adjusted according to Box2D positions, y upwards
    sf::Vector2i adjPos = sf::Vector2i(position.x - window->getSize().x/2,
		-position.y + window->getSize().y/2);

	//shapeFactory->sfvec_to_b2vec(position),
	
	boxes.push_back(
		shapeFactory->createRectangle(
			new b2Vec2(20.0f, 20.0f),
			new b2Vec2(float(adjPos.x), float(adjPos.y)),
			true));
}

void Game::removeFallenBoxes(list<Shape*>& deletion)
{
	while(!deletion.empty())
	{
		vector<Shape*>::iterator todelete = std::find_if(boxes.begin(), boxes.end(), [&deletion](Shape* b)
		{
			return deletion.front() == b;
		}
		);
		if(todelete != boxes.end())
		{
			sf::Packet p = packetParser->pack<int>(UDPNetwork::REMOVE_SHAPE, (*todelete)->getId());
			dynamic_cast<Server*>(localHost)->broadCast(p);
			boxes.erase(todelete);
			delete deletion.front();
			deletion.pop_front();
		}
	}
	deletion.clear();
}

void Game::calcViewOffset()
{
	sf::Vector2f halfExtents = view->getSize() / 2.0f;
	sf::Vector2f translation = view->getCenter() - halfExtents;
	viewOffset.x = (int)translation.x;
	viewOffset.y = (int)translation.y;
}

void Game::boxHandling()
{
	//handle boxes
    for(Shape* box : boxes) 
	{
        box->update();
        window->draw(*box->getShape());
		/*string s = "ID: " +  to_string(box->getId());
		window->draw(textor->write(s, box->getShape()->getPosition()));*/
		//check if box has fallen "outside"
		if(localHost->isServer() && box->getShape()->getPosition().y > window->getSize().y - viewOffset.y + killOffset)
			deletion.push_back(box);
    }
	if(localHost->isServer())
		removeFallenBoxes(deletion);
}

Shape* Game::createBoxes()
{
	//random dropping of boxes
    timer = clock.getElapsedTime();
    duration = timer.asSeconds();
    
	if(duration > secPerDrops)
	{
		boxes.push_back(shapeFactory->createRandomShape(viewOffset));
		duration = 0;
        clock.restart();
		return boxes.back();
	}	
	else
		return nullptr;
}

void Game::addRemotePlayer(Player* p)
{
	playerAmount++;
    p->setBirthNumber(playerAmount);
	remotePlayers.push_back(p);
	p->init(p);
}

void Game::exitGame()
{
	cout << "Exiting game..." << endl;
	localHost->exit = true;
	if(localHost->isServer())
	{
		allowJoin = false; //prepare listen thread for shutdown
		Server* server = dynamic_cast<Server*>(localHost);
		sf::Packet packet;
		packet << UDPNetwork::SERVER_EXIT;
		packet << player->getName();
		server->broadCast(packet);
		cout << "Waiting for network thread to join..." << endl;
	}
	else
	{
		Client* client = dynamic_cast<Client*>(localHost);
		sf::Packet packet;
		packet << UDPNetwork::CLIENT_EXIT;
		packet << player->getName();
		client->sendToServer(packet);
		
	}
	cout << "Waiting for listen thread to join..."<< endl;
	network->join();
	window->close();
	exitCalled = true;
	cout << "Good bye!" << endl;
}

bool Game::removeRemotePlayer(string name)
{
	//remove remote player
	list<Player*>::iterator i = remove_if(remotePlayers.begin(), remotePlayers.end(), [name](Player* a)
	{
		return a->getName() == name;
	});
	if(i != remotePlayers.end())
	{
		delete *i;
		remotePlayers.erase(i);
		if(!localHost->isServer())
			return true;
		else
			return dynamic_cast<Server*>(localHost)->dropPlayer(name);
	}
	else
		return false;
}

void Game::playerBoxInteraction()
{
	int id;
	b2ContactEdge* edge = player->getBody()->GetContactList();
	for(;edge; edge = edge->next)
	{
		if(edge->contact->IsTouching())
		{
			id = ((b2BodyUserData*)edge->other->GetUserData())->id;

			//id >0 means that it is not ground i.e. blocks of some kind that isn't players
			if(id > 0)
			{
				//Also check if last entered element in localChanges is for the same block so that we 
				//can save data. Overwrite last element if the same
				bool push;
				shapeSync* s;
				if(!localChanges.empty() && localChanges.back()->shapeID == id)
				{
					s = localChanges.back();
					push = false;
				}
				else
				{
					s = new shapeSync;
					push = true;
				}
				s->shapeID = id;
				s->angularVel = edge->other->GetAngularVelocity();
				s->velocity = edge->other->GetLinearVelocity();
				s->position = edge->other->GetPosition();
				s->angle = edge->other->GetAngle();
				s->hookUserData = (uintptr_t)edge->other->GetFixtureList()->GetUserData();
				getShape(id)->resetUpdateClock();
				if(push)
					localChanges.push_back(s);
			}
		}
	}
}

void Game::updateShapes(shapeSync* s)
{
	int id = s->shapeID;
	vector<Shape*>::iterator i = find_if(boxes.begin(), boxes.end(), [id](Shape* s)
	{
		return s->getId() == id;
	});
	if(i != boxes.end())
	{
		Shape* shape = *i;
		shape->getBody()->SetAngularVelocity(s->angularVel);
		shape->getBody()->SetLinearVelocity(s->velocity);
		shape->setPosition(&s->position, s->angle);
		shape->getBody()->GetFixtureList()->SetUserData((void*) s->hookUserData);
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

void Game::updatePlayer(player_info* p)
{
	Player* player = getRemotePlayer(p->name);
	if(player != nullptr)
	{
		//cout << "Updating player " << player->getName() << endl;
		player->setPosition(&p->position);
		player->getBody()->SetLinearVelocity(p->velocity);
		//update player hook
		Circle * hookTip = player->getHookTip();
		hookTip->setPosition(&p->hookTip.position, p->hookTip.angle);
		/*hookTip->getBody()->SetLinearVelocity(p->hookTip.velocity);
		hookTip->getBody()->SetAngularVelocity(p->hookTip.angularVel);*/
		Rectangle* hookbase = player->getHookBase();
		hookbase->setPosition(&p->hookBase.position, p->hookBase.angle);
		player->getBox()->resetUpdateClock();
	}
	/*else
		cerr << "Could not find player \'" << p->name << "\'" << endl;*/
}

void Game::removeShape(int id)
{
	vector<Shape*>::iterator del = find_if(boxes.begin(), boxes.end(), [id](Shape* s)
	{
		return s->getId() == id;
	});
	if(del != boxes.end())
	{
		delete *del;
		boxes.erase(del);
	}
	else
		cout << "Shape to be removed not found! ID: "<< id << endl;
}

void Game::playerHandling()
{
	player->draw();
	player->update();
	window->draw(textor->write(player->getName(), player->getBox()->getShape()->getPosition()));
    
	for(list<Player*>::iterator itr = remotePlayers.begin(); itr != remotePlayers.end(); itr++)
	{
		Player* remotePlayer = *itr;
		remotePlayer->draw();
		remotePlayer->update();
		window->draw(textor->write(remotePlayer->getName(), remotePlayer->getBox()->getShape()->getPosition()));
	}
}

void Game::requestShapeUpdates()
{
	//shapes
	vector<Shape*>::iterator i = boxes.begin()+1;
	for(;i != boxes.end(); ++i)
	{
		Shape* s = *i;
		if(s != nullptr && s->timeSinceUpdate().asSeconds() > updateTime)
		{
			//cout << "requesting synch data for shape "<< s->getId() << endl;
			sf::Packet request = packetParser->pack<int>(UDPNetwork::SHAPE_SYNCH_REQUEST, s->getId());
			dynamic_cast<Client*>(localHost)->sendToServer(request);
		}
	}
}

void Game::requestPlayerUpdates()
{
	//remote players
	list<Player*>::iterator listItr = remotePlayers.begin();
	for(; listItr != remotePlayers.end(); ++listItr)
	{
		Player* p = *listItr;
		if(p != nullptr && p->getBox()->timeSinceUpdate().asSeconds() > updateTime)
		{
			//cout << "Request update for player: " << p->getName() << endl;
			sf::Packet request = packetParser->pack<string>(UDPNetwork::PLAYER_SYNCH_REQUEST, p->getName());
			if(localHost->isServer())
			{
				Server* server = dynamic_cast<Server*>(localHost);
				client* receiver = server->getClient(p->getName());
				localHost->send(request, receiver->clientAddress, receiver->clientPort);
				
			}
			else
			{
				dynamic_cast<Client*>(localHost)->sendToServer(request);
			}
		}
	}
}

Shape* Game::getShape(int id)
{
	vector<Shape*>::iterator found = find_if(boxes.begin(), boxes.end(), [id](Shape* s)
	{
		return id == s->getId();
	});
	if(found != boxes.end())
		return *found;
	else
		return nullptr;
}

Player* Game::getRemotePlayer(string name)
{
	list<Player*>::iterator found = find_if(remotePlayers.begin(), remotePlayers.end(), [name](Player* player)
	{
		return name == player->getName();
	});
	if(found != remotePlayers.end())
		return *found;
	else
		return nullptr;
}

Player* Game::getPlayer(string name)
{
	if(player->getName() == name)
		return player;
	else
		return getRemotePlayer(name);
}