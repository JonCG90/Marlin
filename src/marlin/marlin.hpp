//
//  marlin.hpp
//  Marlin
//
//  Created by Jonathan Graham on 4/19/20.
//  Copyright © 2020 Jonathan Graham. All rights reserved.
//

#ifndef MARLIN_HPP
#define MARLIN_HPP

namespace marlin
{

class MlnContext
{
public:
    
    MlnContext() = default;
    ~MlnContext() = default;
    
    void createInstance();

private:
        
private:
    
};

void init();

void render();

void deinit();

}

#endif /* marlin_hpp */
