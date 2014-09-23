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