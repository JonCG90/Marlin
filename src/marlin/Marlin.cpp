//
//  Marlin.cpp
//  Marlin
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#include <iostream>
#include "Marlin.hpp"
#include "MarlinPriv.hpp"

#include <vulkan/vulkan.h>

void Marlin::HelloWorld(const char * s)
{
    MarlinPriv *theObj = new MarlinPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void MarlinPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

