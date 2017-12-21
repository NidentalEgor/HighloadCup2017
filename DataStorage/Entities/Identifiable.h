#ifndef IDENTIFIABLE_H_INCLUDED
#define IDENTIFIABLE_H_INCLUDED
#pragma once

#include "DataTypes.h"

struct Identifiable
{
public:
    Identifiable(const Id id)
        : id(id)
    {
    }

    Id GetId()
    {
        return id;
    };

public:
    Id id;
};

#endif // IDENTIFIABLE_H_INCLUDED