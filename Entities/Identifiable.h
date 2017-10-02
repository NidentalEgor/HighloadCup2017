#ifndef IDENTIFIABLE_H_INCLUDED
#define IDENTIFIABLE_H_INCLUDED
#pragma once

class Identifiable
{
public:
    Identifiable()
    {
    }

    Identifiable(const uint32_t id)
        : id_(id)
    {
    }

    uint32_t GetId()
    {
        return id_;
    };

protected:
    uint32_t id_;
};

#endif // IDENTIFIABLE_H_INCLUDED