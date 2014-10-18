#include "ContactListener.h"
#include "Game.h"

ContactListener::ContactListener(Game* game_) {
    game = game_;
	for(int i = 0; i < 10; i++) {
        numFootContacts[i] = 0;
        recentHookContacts[i] = NULL;
    }
    hostGame = false;
    joinGame = false;
}

int ContactListener::getNumFootContacts(int birthNumber) {
    return numFootContacts[birthNumber-1];
}

b2Body* ContactListener::getRecentHookContact(int birthNumber) {
    return recentHookContacts[birthNumber-1];
}

void ContactListener::setHookActive(bool active) {
	hookIsActive = active;
}

void ContactListener::removeRecentHookContact(int birthNumber) {
    recentHookContacts[birthNumber-1] = NULL;
}

// DESCRIPTION:
// Fixtures can be registered with a user data of a certain size.
// EXAMPLE: footSensorFixture->SetUserData( (void*)3 );
// REGISTERED FIXTURES:
// 1-10 	foot sensors for players
// 11-20	hook collision for players
// 80-81	hostGame and joinGame objects
// 71		hook length item
// 99		randomly spawned rectangles(createRandomShape(...))

// Checking if contact between bodies has started
void ContactListener::BeginContact(b2Contact* contact) {
    void* fixtureAUserData = contact->GetFixtureA()->GetUserData();
    void* fixtureBUserData = contact->GetFixtureB()->GetUserData();
    
    
	switch((uintptr_t)fixtureAUserData) {
        case 1:
//            cout << "foot contact " << 1 << endl;
            numFootContacts[1-1]++;
            break;
        case 2:
//            cout << "foot contact " << 2 << endl;
            numFootContacts[2-1]++;
            break;
        case 3:
//            cout << "foot contact " << 3 << endl;
            numFootContacts[3-1]++;
            break;
        case 4:
//            cout << "foot contact " << 4 << endl;
            numFootContacts[4-1]++;
            break;
        case 11:
            if(hookIsActive) {
                cout << "hook contact " << 11 << endl;
                if((uintptr_t)fixtureBUserData == 99) {
                    recentHookContacts[11-11] = contact->GetFixtureB()->GetBody();
                }
                if((uintptr_t)fixtureBUserData == 80) {
                    cout << "HOST GAME" << endl;
                    hostGame = true;
                }
                if((uintptr_t)fixtureBUserData == 81) {
                    cout << "JOIN GAME" << endl;
                    joinGame = true;
                }
            }
            break;
        case 12:
            cout << "hook contact " << 12 << endl;
            if((uintptr_t)fixtureBUserData == 99) {
                recentHookContacts[12-11] = contact->GetFixtureB()->GetBody();
            }
            break;
        case 13:
            if((uintptr_t)fixtureBUserData == 99) {
                recentHookContacts[13-11] = contact->GetFixtureB()->GetBody();
            }
            break;
        case 14:
            if((uintptr_t)fixtureBUserData == 99) {
                recentHookContacts[14-11] = contact->GetFixtureB()->GetBody();
            }
            break;
            
        default: ;
    }
    
    switch((uintptr_t)fixtureBUserData) {
        case 1:
//            cout << "foot contact " << 1 << endl;
            numFootContacts[1-1]++;
            break;
        case 2:
//            cout << "foot contact " << 2 << endl;
            numFootContacts[2-1]++;
            break;
        case 3:
//            cout << "foot contact " << 3 << endl;
            numFootContacts[3-1]++;
            break;
        case 4:
//            cout << "foot contact " << 4 << endl;
            numFootContacts[4-1]++;
            break;
        case 11:
//            cout << "hook contact " << 11 << endl;
            if(hookIsActive) {
                if((uintptr_t)fixtureAUserData == 99) {
                    recentHookContacts[11-11] = contact->GetFixtureA()->GetBody();
                }
                if((uintptr_t)fixtureAUserData == 80) {
                    cout << "HOST GAME" << endl;
                    hostGame = true;
                }
                if((uintptr_t)fixtureAUserData == 81) {
                    cout << "JOIN GAME" << endl;
                    joinGame = true;
                }
            }
            break;
        case 12:
            if((uintptr_t)fixtureAUserData == 99) {
                recentHookContacts[12-11] = contact->GetFixtureA()->GetBody();
            }
            break;
        case 13:
            if((uintptr_t)fixtureAUserData == 99) {
                recentHookContacts[13-11] = contact->GetFixtureA()->GetBody();
            }
            break;
        case 14:
            if((uintptr_t)fixtureAUserData == 99) {
                recentHookContacts[14-11] = contact->GetFixtureA()->GetBody();
            }
            break;
            
        default: ;
    }
}

// Checking if contact between bodies has ended
void ContactListener::EndContact(b2Contact* contact) {
    void* fixtureAUserData = contact->GetFixtureA()->GetUserData();
    void* fixtureBUserData = contact->GetFixtureB()->GetUserData();
    
   	switch((uintptr_t)fixtureAUserData) {
        case 1:
//            cout << "foot lost contact " << 1 << endl;
            numFootContacts[1-1]--;
            break;
        case 2:
//            cout << "foot lost contact " << 2 << endl;
            numFootContacts[2-1]--;
            break;
        case 3:
//            cout << "foot lost contact " << 3 << endl;
            numFootContacts[3-1]--;
            break;
        case 4:
//            cout << "foot lost contact " << 4 << endl;
            numFootContacts[4-1]--;
            break;
        case 11:
//            cout << "hook lost contact " << 11 << endl;
            break;
        case 12:
//            cout << "hook lost contact " << 12 << endl;
            break;
            
        default: ;
    }
    
    switch((uintptr_t)fixtureBUserData) {
        case 1:
//            cout << "foot lost contact " << 1 << endl;
            numFootContacts[1-1]--;
            break;
        case 2:
//            cout << "foot lost contact " << 2 << endl;
            numFootContacts[2-1]--;
            break;
        case 3:
//            cout << "foot lost contact " << 3 << endl;
            numFootContacts[3-1]--;
            break;
        case 4:
//            cout << "foot lost contact " << 4 << endl;
            numFootContacts[4-1]--;
            break;
        case 11:
//            cout << "hook lost contact " << 11 << endl;
            break;
        case 12:
//            cout << "hook lost contact " << 12 << endl;
            break;
            
        default: ;
    }
}