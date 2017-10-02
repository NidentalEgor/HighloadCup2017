#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED
#pragma once

#include <string>

#include "rapidjson/document.h"
#include "Identifiable.h"
#include "DataTypes.h"

enum class Gender
{
    Male,
    Female
};

class User : public Identifiable
{
public:
    User(
        const uint32_t id,
        const std::string& email,
        const std::string& first_name,
        const std::string& last_name,
        const Gender gender,
        const Timestamp timestamp);

    User(const rapidjson::Value& user_element);
    
private:
    std::string email_;
    std::string first_name_;
    std::string last_name_;
    Gender gender_;
    Timestamp timestamp_;
};

#endif // USER_H_INCLUDED