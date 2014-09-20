//
//  OSHandler.cpp
//  Block Frog
//
//  Created by roflmao on 2014-09-20.
//  Copyright (c) 2014 TDDD23. All rights reserved.
//

#include "OSHandler.h"
#include "ResourcePath.hpp"

void OSHandler::win32() {
    resourcePath = "";
}

void OSHandler::mac() {
    resourcePath = macResourcePath();
}

std::string OSHandler::getResourcePath() {
    return resourcePath;
}