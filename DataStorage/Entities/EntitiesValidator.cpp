#include <unordered_map>
#include <string>

#include "../../Utils/Macroses.h"

#include "EntitiesValidator.h"

std::unordered_map<int, std::string> EntitiesValidator::entity_attribute_to_string
{
    { static_cast<int>(EntitiesValidator::EntityAttributes::Place),      "place"       },
    { static_cast<int>(EntitiesValidator::EntityAttributes::Country),    "country"     },
    { static_cast<int>(EntitiesValidator::EntityAttributes::City),       "city"        },
    { static_cast<int>(EntitiesValidator::EntityAttributes::Distance),   "distance"    },
    { static_cast<int>(EntitiesValidator::EntityAttributes::LocationId), "location"    },
    { static_cast<int>(EntitiesValidator::EntityAttributes::UserId),     "user"        },
    { static_cast<int>(EntitiesValidator::EntityAttributes::VisitedAt),  "visited_at"  },
    { static_cast<int>(EntitiesValidator::EntityAttributes::Mark),       "mark"        },
    { static_cast<int>(EntitiesValidator::EntityAttributes::Email),      "email"       },
    { static_cast<int>(EntitiesValidator::EntityAttributes::FirstName),  "first_name"  },
    { static_cast<int>(EntitiesValidator::EntityAttributes::LastName),   "last_name"   },
    { static_cast<int>(EntitiesValidator::EntityAttributes::Gender),     "gender"      },
    { static_cast<int>(EntitiesValidator::EntityAttributes::BirthDate),  "birth_date"  }
};

EntitiesValidator::EntitiesValidator()
    : attributes_mask_(0)
{
}

bool EntitiesValidator::CheckStringAttribute(
        const EntityAttributes attribute)
{
    const char* attribute_name =
            entity_attribute_to_string[static_cast<int>(attribute)].c_str();

    if (json_content_.HasMember(attribute_name))
    {
        if (json_content_[attribute_name].IsString())
        {
            attributes_mask_ |= static_cast<int>(attribute);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

bool EntitiesValidator::CheckUint64Attribute(
        const EntityAttributes attribute)
{
    const char* attribute_name =
            entity_attribute_to_string[static_cast<int>(attribute)].c_str();

    if (json_content_.HasMember(attribute_name))
    {
        if (json_content_[attribute_name].IsUint64())
        {
            attributes_mask_ |= static_cast<int>(attribute);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

bool EntitiesValidator::CheckInt64Attribute(
        const EntityAttributes attribute)
{
    const char* attribute_name =
            entity_attribute_to_string[static_cast<int>(attribute)].c_str();

    if (json_content_.HasMember(attribute_name))
    {
        if (json_content_[attribute_name].IsInt64())
        {
            attributes_mask_ |= static_cast<int>(attribute);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

uint64_t EntitiesValidator::GetUint64Attribute(
        const EntityAttributes attribute)
{
    return json_content_[
            entity_attribute_to_string[
                static_cast<int>(attribute)].c_str()].GetUint64();
}

int64_t EntitiesValidator::GetInt64Attribute(
        const EntityAttributes attribute)
{
    return json_content_[
            entity_attribute_to_string[
                static_cast<int>(attribute)].c_str()].GetInt64();
}

std::string EntitiesValidator::GetStringAttribute(
        const EntityAttributes attribute)
{
    return json_content_[
            entity_attribute_to_string[
                static_cast<int>(attribute)].c_str()].GetString();
}

bool EntitiesValidator::LocationDataForUpdate(
        const char* content)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            !json_content_.Parse(content).HasParseError(),
            false);

    return CheckStringAttribute(EntityAttributes::Place) &&
            CheckStringAttribute(EntityAttributes::Country) &&
            CheckStringAttribute(EntityAttributes::City) &&
            CheckUint64Attribute(EntityAttributes::Distance); 
}
        
bool EntitiesValidator::VisitDataForUpdate(
        const char* content)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            !json_content_.Parse(content).HasParseError(),
            false);

    return CheckUint64Attribute(EntityAttributes::LocationId) &&
            CheckUint64Attribute(EntityAttributes::UserId) &&
            CheckInt64Attribute(EntityAttributes::VisitedAt) &&
            CheckUint64Attribute(EntityAttributes::Mark); 
}
        
bool EntitiesValidator::UserDataForUpdate(
        const char* content)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            !json_content_.Parse(content).HasParseError(),
            false);

    return CheckStringAttribute(EntityAttributes::Email) &&
            CheckStringAttribute(EntityAttributes::FirstName) &&
            CheckStringAttribute(EntityAttributes::LastName) &&
            CheckStringAttribute(EntityAttributes::Gender) &&
            CheckInt64Attribute(EntityAttributes::BirthDate); 
}