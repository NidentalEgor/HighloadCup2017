#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED
#pragma once

#include <string>
#include <memory>

#include "../../Submodules/rapidjson/include/rapidjson/document.h"

#include "DataTypes.h"
#include "Identifiable.h"
#include "ISerializable.h"
#include "IDeserializable.h"
#include "IValidateable.h"

struct User
    : public Identifiable
    , public IDeserializable
    , public ISerializable
    , public IValidateable
{
public:
    User();

    User(
        const Id id,
        const std::string& email,
        const std::string& first_name,
        const std::string& last_name,
        const Gender gender,
        const Timestamp birth_date);

    void Deserialize(
            const rapidjson::Value& user_element) final;

    std::unique_ptr<std::string> Serialize() const final;

    bool Validate(
            const char* content) const final;
    
public:
    std::string email;
    std::string first_name;
    std::string last_name;
    Gender gender;
    Timestamp birth_date;
};

#endif // USER_H_INCLUDED