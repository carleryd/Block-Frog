#include "Game.h"
#include "OSHandler.h"
#include "Server.h"
#include "Client.h"
#include "ShapeFactory.h"
#include "Utility.h"
#include "Textor.h"
#include "ContactListener.h"
#include "Remover.h"
#include "Synchronizer.h"

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
	water->setFillColor(sf::Color(255, 0, 0, 150));
	textor = new Textor(osHandler_);
	exitCalled = false;
	
	staticPlatform = 0;
    score = 0;
	localHost = NULL;
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
	exitGame();
}

void Game::initStartMenu() {
    hostRectangle = shapeFactory->createRectangle(new b2Vec2(100.0f, 50.0f),
                                                         new b2Vec2(-200.0f, 30.0f),
                                                         false,
                                                         1.0,
                                                         1.0);
    hostRectangle->getBody()->GetFixtureList()->SetUserData((void*)80);
    
    joinRectangle = shapeFactory->createRectangle(new b2Vec2(100.0f, 50.0f),
                                                         new b2Vec2(200.0f, 30.0f),
                                                         false ,
                                                         1.0,
                                                         1.0);
    joinRectangle->getBody()->GetFixtureList()->SetUserData((void*)81);
    
    boxes.push_back(shapeFactory->createRectangle(new b2Vec2(750.0f, 50.0f),
                                                  new b2Vec2(0.0f, -float(window->getSize().y)/2),
                                                  false,
                                                  1.0,
                                                  1.0)
                    );
    
    boxes.push_back(hostRectangle);
    boxes.push_back(joinRectangle);
    
    riseSpeed = 0;
    secPerDrops = 9999;
    window->setActive(true);
}

void Game::removeStartMenu() {
    while(!boxes.empty()) {
        delete *boxes.begin();
     	boxes.erase(boxes.begin());
    }
}

void Game::runStartMenu() {
    // timeStep, velocityIterations, positionIterations
    world->Step(1.0f/60.0f, 10, 8);
    world->ClearForces();
    
    player->draw();
	player->update();
    
    for(Shape* box : boxes)
	{
        box->update();
        window->draw(*box->getShape());
    }
    
    window->draw(textor->write("Make a choice", sf::Vector2f(window->getSize().x/2, 100)));
	window->draw(textor->write("Host", hostRectangle->getShape()->getPosition() - sf::Vector2f(0, 10)));
    window->draw(textor->write("Join", joinRectangle->getShape()->getPosition() - sf::Vector2f(0, 10)));
}

void Game::basicInit() {
    //shapefactory for creating shapes easily
	shapeFactory = new ShapeFactory(this);
    utility = new Utility(this);
    
    contactListener = new ContactListener(this);
    world->SetContactListener(contactListener);
    
    player = new Player(this);
    player->init(player);
	playerAmount = 1;
}

void Game::init(int playerType, sf::IpAddress* serverAddress, unsigned short serverPort) {
	//shapefactory for creating shapes easily
//	shapeFactory = new ShapeFactory(this);
//    utility = new Utility(this);
//    
//    contactListener = new ContactListener();
//    world->SetContactListener(contactListener);
    
//	player = new Player(this);
//    player->init(player);
//	playerAmount = 1;
    
	packetParser = new PacketParser(*shapeFactory);
	
	//ground
    boxes.push_back(shapeFactory->createRectangle(new b2Vec2(750.0f, 50.0f),
                                                  new b2Vec2(0.0f, -float(window->getSize().y)/2),
                                                  false,
                                         	         1.0,
                                                  1.0)
                    );
	
	lastStaticShape = boxes.back();
	//test shape
    //boxes.push_back(shapeFactory->createRandomShape(viewOffset));

	rise = false;
	riseSpeed = -0.2f;
	allowJoin = true;
	remover = new Remover(this);
	synchronizer = new Synchronizer(*this);
    
	//networking
	switch (playerType)
	{
        case SERVER:
            localHost = new Server("HOST", *shapeFactory, this);
            player->setName("HOST");
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
	
	secPerDrops = 2;
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
	//playerBoxInteraction();

	//water
	//water->setPosition(float(window->getSize().x * -0.1), float(window->getSize().y - viewOffset.y -10));
	water->setPosition(float(window->getSize().x * -0.1), float( viewOffset.y + water->getLocalBounds().height - 10));
	window->draw(*water);
    
    // score)
	window->draw(textor->write("Score: " + to_string(score), sf::Vector2f(window->getSize().x/2, 30+viewOffset.y)));

    if(localHost->isServer())
    {
        Shape* box = createBoxes();
        if(box != nullptr)
        {
            sf::Packet packet = packetParser->pack(box);
            dynamic_cast<Server*>(localHost)->broadCast(packet);
			if(staticPlatform == 0)
			{
				b2Vec2 v = lastStaticShape->getBody()->GetPosition();
				v.y += 10;
				boxes.push_back(shapeFactory->createItem(&v));
				packet.clear();
				packet = packetParser->pack(boxes.back());
				dynamic_cast<Server*>(localHost)->broadCast(packet);
			}
        }
    }

	//player
	playerHandling();

	//move view up/raise water level
	if(rise)
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
		/*for (shapeSync* shapeSync : localChanges)
		{
			sf::Packet p = packetParser->pack(shapeSync);
			server->broadCast(p);
		}*/

		//broadcast water level:
		sf::Packet p = packetParser->pack<int>(UDPNetwork::WATER_LEVEL, viewOffset.y);
		server->broadCast(p);
		sf::Packet score = packetParser->pack<int>(UDPNetwork::SCORE_CHANGE, Game::score);
		server->broadCast(score);
	}
	else
	{
		//send local changes to server
		/*for (shapeSync* shapeSync : localChanges)
		{
			sf::Packet p = packetParser->pack(shapeSync);
			dynamic_cast<Client*>(localHost)->sendToServer(p);
		}*/
		//request server update on shapes
		synchronizer->requestShapeUpdates();
	}

	synchronizer->requestPlayerUpdates();
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
		//check if box has fallen "outside"
		if(localHost->isServer())
			remover->checkShapeRemoval(box);
    }

	if(localHost->isServer())
		remover->removeShapes();
}

Shape* Game::createBoxes()
{
	//random dropping of boxes
    timer = clock.getElapsedTime();
    duration = timer.asSeconds();
	staticPlatform = ++staticPlatform % 4;
    
	if(duration > secPerDrops)
	{
		Shape* newBox;
		if(staticPlatform == 0)
		{
			newBox = lastStaticShape = shapeFactory->createRandomShape(viewOffset, false);
            newBox->getBody()->SetUserData((void*)98);
			staticPlatform = 0;
		}
		else
			newBox = shapeFactory->createRandomShape(viewOffset); 
		boxes.push_back(newBox);
        score += 50;
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
    cout << "playerAmount: " << playerAmount << endl;
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
				Shape* shape = getShape(id);
				if(shape != nullptr)
					shape->resetUpdateClock();
				if(push)
					localChanges.push_back(s);
			}
		}
	}
}

void Game::updateShapes(shapeSync* s)
{
	synchronizer->updateShapes(s);
}

void Game::updatePlayer(player_info* p)
{
	synchronizer->updatePlayer(p);
}

void Game::removeShape(int id)
{
	vector<Shape*>::iterator del = find_if(boxes.begin(), boxes.end(), [id](Shape* s)
	{
		return s->getId() == id;
	});
	if(del != boxes.end())
	{
		(*del)->getUserData().toBeRemoved = true;
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
	remover->checkPlayerKill(player);
	remover->respawnPlayer(player);

	for(list<Player*>::iterator itr = remotePlayers.begin(); itr != remotePlayers.end(); itr++)
	{
		Player* remotePlayer = *itr;
		remotePlayer->draw();
		remotePlayer->update();
		window->draw(textor->write(remotePlayer->getName(), remotePlayer->getBox()->getShape()->getPosition()));
		/*if(remotePlayer->getBox()->getShape()->getPosition().y > /*window->getSize().y - viewOffset.y*//* viewOffset.y + killOffset)
			remotePlayer->setPosition(lastStaticShape->getPosition());*/
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

void Game::startRise()
{
	rise = true;
	sf::Packet p = packetParser->pack<int>(UDPNetwork::START_RISE, 1);
	if(localHost->isServer())
		dynamic_cast<Server*>(localHost)->broadCast(p);
};
