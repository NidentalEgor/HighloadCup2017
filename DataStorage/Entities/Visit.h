#ifndef VISIT_H_INCLUDED
#define VISIT_H_INCLUDED
#pragma once

#include "../../Submodules/rapidjson/include/rapidjson/document.h"

#include "Identifiable.h"
#include "DataTypes.h"
#include "ISerializable.h"
#include "IDeserializable.h"
#include "IValidateable.h"

struct Visit
    : public Identifiable
    , public IDeserializable
    , public ISerializable
    , public IValidateable
{
public:
    Visit(
        const Id id,
        const Id location_id,
        const Id user_id,
        const Timestamp visited_at,
        const Mark mark);

    Visit();

    void Deserialize(
            const rapidjson::Value& visit_element) override final;

    std::unique_ptr<std::string> Serialize() const override final;

    bool Validate(
            const char* content) const override;

public:
    Id location_id;
    Id user_id;
    Timestamp visited_at;
    Mark mark;
};

#endif // VISIT_H_INCLUDED
