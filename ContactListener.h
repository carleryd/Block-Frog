//
//  ContactListener.h
//  Block Frog
//
//  Created by roflmao on 2014-09-20.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#ifndef __Block_Frog__ContactListener__
#define __Block_Frog__ContactListener__

#include <iostream>
#include <Box2D/Box2D.h>

class ContactListener : public b2ContactListener
{
public:
    ContactListener();
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);
    int getNumFootContacts();
    
private:
    int numFootContacts;
};

#endif /* defined(__Block_Frog__ContactListener__) */
