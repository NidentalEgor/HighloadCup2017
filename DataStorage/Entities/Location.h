#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED
#pragma once

#include <string>
#include <memory>

#include "../../Submodules/rapidjson/include/rapidjson/document.h"

#include "DataTypes.h"
#include "Identifiable.h"
#include "ISerializable.h"
#include "IDeserializable.h"
#include "IValidateable.h"

struct Location
    : public Identifiable
    , public IDeserializable
    , public ISerializable
    , public IValidateable
{
public:
    Location(
        const Id id,
        const std::string& place,
        const std::string& country,
        const std::string& city,
        const Distance distance);
    
    Location();

    void Deserialize(
            const rapidjson::Value& location_element) final;

    std::unique_ptr<std::string> Serialize() const final;

    bool Validate(
            const char* content) const final;

public:
    std::string place;
    std::string country;
    std::string city;
    Distance distance;
};

#endif // LOCATION_H_INCLUDED