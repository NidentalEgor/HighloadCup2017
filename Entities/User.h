#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED
#pragma once

#include <string>

#include "rapidjson/document.h"
#include "Identifiable.h"
#include "DataTypes.h"
#include "IDeserializable.h"
#include "Visit.h"

enum class Gender
{
    Male,
    Female
};

class User
    : public Identifiable
    , public IDeserializable
{
public:
    User(
        const uint32_t id,
        const std::string& email,
        const std::string& first_name,
        const std::string& last_name,
        const Gender gender,
        const Timestamp timestamp);
    
    User();

    // User(const rapidjson::Value& user_element);

    void Deserialize(
            const rapidjson::Value& user_element) override final;
    
private:
    std::string email_;
    std::string first_name_;
    std::string last_name_;
    Gender gender_;
    Timestamp timestamp_;

    // std::set<uint32_t, VisitSorterByDate> visits_id_;
};

#endif // USER_H_INCLUDED