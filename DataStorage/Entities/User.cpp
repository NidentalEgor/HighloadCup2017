#include <sstream>

#include "../../Utils/Macroses.h"

#include "User.h"

User::User()
    : Identifiable(0)
    , email()
    , first_name()
    , last_name()
    , gender(Gender::Any)
    , birth_date(std::numeric_limits<Timestamp>::min())
{
}

User::User(
        const Id id,
        const std::string& email,
        const std::string& first_name,
        const std::string& last_name,
        const Gender gender,
        const Timestamp birth_date)
    : Identifiable(id)
    , email(email)
    , first_name(first_name)
    , last_name(last_name)
    , gender(gender)
    , birth_date(birth_date)
{
}

void User::Deserialize(
        const rapidjson::Value& user_element)
{
    id = user_element["id"].GetInt();
    email = user_element["email"].GetString();
    first_name = user_element["first_name"].GetString();
    last_name = user_element["last_name"].GetString();
    gender =
        strcmp(user_element["gender"].GetString(), "m") == 0
        ? Gender::Male
        : Gender::Female;
    birth_date = user_element["birth_date"].GetInt();
}

std::unique_ptr<std::string> User::Serialize() const
{
    std::stringstream str;

    std::string user_gender(gender == Gender::Male ? "m" : "f");

    str << "{\"id\":" << id <<
            ",\"email\":\"" << email <<
            "\",\"first_name\":\"" << first_name <<
            "\",\"last_name\":\"" << last_name <<
            "\",\"gender\":\"" << user_gender <<
            "\",\"birth_date\":" << birth_date << '}';

    return std::make_unique<std::string>(str.str());
}

bool User::Validate(
        const char* content) const
{
    rapidjson::Document json_content;;
    ENSURE_TRUE_OTHERWISE_RETURN(
            !json_content.Parse(content).HasParseError(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("id") &&
                json_content["id"].IsUint64(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("email") &&
                json_content["email"].IsString(),
            false);
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("first_name") &&
                json_content["first_name"].IsString(),
            false);
            
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("last_name") &&
                json_content["last_name"].IsString(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("gender") &&
                json_content["gender"].IsString(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("birth_date") &&
                json_content["birth_date"].IsInt64(),
            false);

    return true;
}