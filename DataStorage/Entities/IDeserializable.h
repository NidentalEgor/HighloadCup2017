#ifndef I_DESERIALIZABLE_H_INCLUDED
#define I_DESERIALIZABLE_H_INCLUDED
#pragma once

#include "../../Submodules/rapidjson/include/rapidjson/document.h"

class IDeserializable
{
public:
    virtual ~IDeserializable()
    {
    };

    virtual void Deserialize(
            const rapidjson::Value& user_element) = 0;
};

#endif // I_DESERIALIZABLE_H_INCLUDED