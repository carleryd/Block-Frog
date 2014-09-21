//
//  ContactListener.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-20.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

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