#ifndef __Block_Frog__ContactListener__
#define __Block_Frog__ContactListener__

#include <iostream>
#include <Box2D/Box2D.h>
#include <vector>
using namespace std;

class Game;

class ContactListener : public b2ContactListener
{
public:
    ContactListener(Game* game);
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    
    void setHookActive(bool active);
    
    bool getHostGame() { return hostGame; }
    bool getJoinGame() { return joinGame; }
    int getNumFootContacts(int birthNumber);
    b2Body* getRecentHookContact(int birthNumber);
    
    
    void removeRecentHookContact(int birthNumber);
private:
    Game* game;
    
    int numFootContacts[10];
    b2Body* recentHookContacts[10];
    // So that it wont hook with stuff when it shouldnt(released hook)
    bool hookIsActive;
    bool hostGame;
    bool joinGame;
};

#endif /* defined(__Block_Frog__ContactListener__) */
