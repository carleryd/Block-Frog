//
//  OSHandler.h
//  Block Frog
//
//  Created by roflmao on 2014-09-20.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#ifndef __Block_Frog__OSHandler__
#define __Block_Frog__OSHandler__

#include <iostream>

class OSHandler
{
public:
    void win32();
    void mac();
    std::string getResourcePath();
private:
    std::string resourcePath;
};

#endif /* defined(__Block_Frog__OSHandler__) */
