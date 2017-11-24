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
        : id_(id)
    {
    }

    Id GetId()
    {
        return id_;
    };

// protected:
public:
    Id id_;
};

#endif // IDENTIFIABLE_H_INCLUDED