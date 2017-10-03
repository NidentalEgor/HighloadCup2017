#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED
#pragma once

#include <string>
#include <memory>

#include "rapidjson/document.h"
#include "Identifiable.h"
#include "IDeserializable.h"

class Location
    : public Identifiable
    , public IDeserializable
{
public:
    Location(
        const uint32_t id,
        const std::string& place,
        const std::string& country,
        const std::string& city,
        const uint32_t distance);
    
    Location();

    // Location(const rapidjson::Value& location_element);

    void Deserialize(
            const rapidjson::Value& location_element) override final;

    std::unique_ptr<std::string> Serialize() const;

private:
    std::string place_;
    std::string country_;
    std::string city_;
    uint32_t distance_;
};

#endif // LOCATION_H_INCLUDED