#ifndef IDENTIFIABLE_H_INCLUDED
#define IDENTIFIABLE_H_INCLUDED
#pragma once

#include "DataTypes.h"

class Identifiable
{
public:
    Identifiable()
    {
    }

    Identifiable(const Id id)
        : id(id)
    {
    }

    Id GetId()
    {
        return id;
    };

// protected:
public:
    Id id;
};

#endif // IDENTIFIABLE_H_INCLUDED