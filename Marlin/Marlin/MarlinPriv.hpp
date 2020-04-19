//
//  MarlinPriv.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

/* The classes below are not exported */
#pragma GCC visibility push(hidden)

class MarlinPriv
{
    public:
    void HelloWorldPriv(const char *);
};

#pragma GCC visibility pop
