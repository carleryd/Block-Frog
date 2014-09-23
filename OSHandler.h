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
