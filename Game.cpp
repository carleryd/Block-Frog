#include "Game.h"
#include "OSHandler.h"
#include "Server.h"
#include "Client.h"
#include "ShapeFactory.h"
#include "Utility.h"

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
}

Game::~Game()
{
	while(!boxes.empty())
	{
		delete boxes.back();
		boxes.pop_back();
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
	player = new Player(this);
    player->init();
    
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

sf::RenderWindow* Game::getWindow() {
    return window;
}

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
    player->draw();
	player->update();
    
	for(list<Player*>::iterator itr = remotePlayers.begin(); itr != remotePlayers.end(); itr++)
	{
		(*itr)->draw();
		(*itr)->update();
	}

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
		if(!server->getPlayerInfos().empty())
		{
			vector<player_info*>& playerMoves = server->getPlayerInfos();
			for(int i = 0; i < playerMoves.size(); i++)
			{
				//find player that has moved
				string name = playerMoves[i]->name;
				vector<client*>::iterator player = std::find_if(server->getClients().begin(), server->getClients().end(), [name](client* c)
				{
					return name == c->name;
				});
				//broadcast it to every player except the player that made the move
				//since that player already knows that.
                sf::Packet packet = packetParser->pack(*playerMoves[i]);
				server->broadCastExcept((*player)->clientAddress, (*player)->clientPort, packet);
			}
			playerMoves.clear();
		}
		
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
		vector<Shape*>::iterator todelete = std::remove_if(boxes.begin(), boxes.end(), [&deletion](Shape* b)
		{
			return deletion.front() == b;
		}
		);
		boxes.erase(todelete, boxes.end());
		delete deletion.front();
		deletion.pop_front();
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
	list<Shape*> deletion;
    for(Shape* box : boxes) 
	{
		//check if box has fallen "outside"
		if(box->getShape()->getPosition().y > window->getSize().y - viewOffset.y + killOffset)
			deletion.push_back(box);
        box->update();
        window->draw(*box->getShape());
    }
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
	remotePlayers.push_back(p);
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