#ifndef ENTITIES_VALIDATOR_H_INCLUDED
#define ENTITIES_VALIDATOR_H_INCLUDED
#pragma once

#include <string>

#include "../../Submodules/rapidjson/include/rapidjson/document.h"

class EntitiesValidator
{
public:
    enum class EntityAttributes
    {
        Place = 1,
        Country = 2,
        City = 4,
        Distance = 8,
        LocationId = 16,
        UserId = 32,
        VisitedAt = 64,
        Mark = 128,
        Email = 256,
        FirstName = 512,
        LastName = 1024,
        Gender = 2048,
        BirthDate = 4096
    };

public:
    EntitiesValidator();

    bool LocationDataForUpdate(
            const char* data);

    bool VisitDataForUpdate(
        const char* content);

    bool UserDataForUpdate(
        const char* content);

    uint64_t GetUint64Attribute(
        const EntityAttributes attribute);
    
    int64_t GetInt64Attribute(
        const EntityAttributes attribute);

    std::string GetStringAttribute(
        const EntityAttributes attribute);

    int GetMask()
    {
        return attributes_mask_;
    }

private:
    bool CheckStringAttribute(
            EntityAttributes attribute);

    bool CheckUint64Attribute(
            EntityAttributes attribute);

    bool CheckInt64Attribute(
        const EntityAttributes attribute);

private:
    int attributes_mask_;
    rapidjson::Document json_content_;
    static std::unordered_map<int, std::string> entity_attribute_to_string;
}; 

#endif // ENTITIES_VALIDATOR_H_INCLUDED