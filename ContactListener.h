#ifndef __Block_Frog__ContactListener__
#define __Block_Frog__ContactListener__

#include <iostream>
#include <Box2D/Box2D.h>
#include <vector>
using namespace std;

class ContactListener : public b2ContactListener
{
public:
    ContactListener();
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    
    void setHookActive(bool active);
    
    int getNumFootContacts();
    b2Body* getRecentHookContact();
    void removeRecentHookContact();
private:
    int numFootContacts;
    b2Body* recentHookContact;
    bool hookIsActive;
};

#endif /* defined(__Block_Frog__ContactListener__) */
