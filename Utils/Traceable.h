#ifndef TRACEABLE_H_INCLUDED
#define TRACEABLE_H_INCLUDED
#pragma once

#include <iostream>

class Traceable
{
public:
    void Trace(
            const char* message)
    {
        std::cout << message << std::endl;
    }
};

#endif // TRACEABLE_H_INCLUDED