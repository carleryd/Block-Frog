//
//  OSHandler.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-20.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "OSHandler.h"
#ifdef __APPLE__
#include "ResourcePath.hpp"
#endif
void OSHandler::win32() {
    resourcePath = "";
}

#ifdef __APPLE__
void OSHandler::mac() {
    resourcePath = macResourcePath();
}
#endif

std::string OSHandler::getResourcePath() {
    return resourcePath;
}