#ifndef I_SERIALIZABLE_H_INCLUDED
#define I_SERIALIZABLE_H_INCLUDED
#pragma once

#include <memory>
#include <string>

#include "../../Submodules/rapidjson/include/rapidjson/document.h"

class ISerializable
{
public:
    virtual ~ISerializable()
    {
    };

    virtual std::unique_ptr<std::string> Serialize() const = 0;
};

#endif // I_SERIALIZABLE_H_INCLUDED