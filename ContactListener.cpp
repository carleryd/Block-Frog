#include "ContactListener.h"

ContactListener::ContactListener() {
    numFootContacts = 0;
}

int ContactListener::getNumFootContacts() {
    return numFootContacts;
}

void ContactListener::BeginContact(b2Contact* contact) {
    //check if fixture A was the foot sensor
    void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    if ( (uintptr_t)fixtureUserData == 3 )
        numFootContacts++;
    
    //check if fixture B was the foot sensor
    fixtureUserData = contact->GetFixtureB()->GetUserData();
    if ( (uintptr_t)fixtureUserData == 3 )
        numFootContacts++;
}

void ContactListener::EndContact(b2Contact* contact) {
    //check if fixture A was the foot sensor
    void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    if ( (uintptr_t)fixtureUserData == 3 )
        numFootContacts--;
    
    //check if fixture B was the foot sensor
    fixtureUserData = contact->GetFixtureB()->GetUserData();
    if ( (uintptr_t)fixtureUserData == 3 )
        numFootContacts--;
}