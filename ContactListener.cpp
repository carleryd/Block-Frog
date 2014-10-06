#include "ContactListener.h"

ContactListener::ContactListener() {
    numFootContacts = 0;
    recentHookContact = NULL;
}

int ContactListener::getNumFootContacts() {
    return numFootContacts;
}

b2Body* ContactListener::getRecentHookContact() {
    return recentHookContact;
}

void ContactListener::removeRecentHookContact() {
    recentHookContact = NULL;
}

// DESCRIPTION:
// Fixtures can be registered with a user data of a certain size.
// EXAMPLE: footSensorFixture->SetUserData( (void*)3 );
// REGISTERED FIXTURES:
// 3: Foot sensor
// 4: The tip of the hook
// 5: Dynamic box in world

// Checking if contact between bodies has started
void ContactListener::BeginContact(b2Contact* contact) {
    void* fixtureAUserData = contact->GetFixtureA()->GetUserData();
    void* fixtureBUserData = contact->GetFixtureB()->GetUserData();
    
    if((uintptr_t)fixtureAUserData == 3)
        numFootContacts++;
    else if((uintptr_t)fixtureAUserData == 4) {
//     	cout << "START" << endl;
        if((uintptr_t)fixtureBUserData == 5) {
//            cout << "HIT BOX" << endl;
        	recentHookContact = contact->GetFixtureB()->GetBody();
        }
    }
    
    if ( (uintptr_t)fixtureBUserData == 3 )
        numFootContacts++;
    else if ( (uintptr_t)fixtureBUserData == 4 ) {
//     	cout << "START" << endl;
        if((uintptr_t)fixtureAUserData == 5) {
//            cout << "HIT BOX" << endl;
	        recentHookContact = contact->GetFixtureA()->GetBody();
        }
    }
}

// Checking if contact between bodies has ended
void ContactListener::EndContact(b2Contact* contact) {
    void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    if ( (uintptr_t)fixtureUserData == 3 )
        numFootContacts--;
//    else if ( (uintptr_t)fixtureUserData == 4 )
//        cout << "END" << endl;
    
    fixtureUserData = contact->GetFixtureB()->GetUserData();
    if ( (uintptr_t)fixtureUserData == 3 )
        numFootContacts--;
//    else if ( (uintptr_t)fixtureUserData == 4 )
//        cout << "END" << endl;
}