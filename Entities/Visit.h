#ifndef VISIT_H_INCLUDED
#define VISIT_H_INCLUDED
#pragma once

#include "rapidjson/document.h"
#include "Identifiable.h"
#include "DataTypes.h"
#include "IDeserializable.h"

using Mark = short;

class Visit
    : public Identifiable
    , public IDeserializable
{
public:
    Visit(
        const uint32_t id,
        const uint32_t location_id,
        const uint32_t user_id,
        const Timestamp timestamp,
        const Mark mark);

    Visit();

    void Deserialize(
            const rapidjson::Value& visit_element) override final;

    // Visit(const rapidjson::Value& visit_element);

    bool operator<(
            const Visit& right) const;

    bool operator<(
            const Timestamp& right) const;
    
// private:
public:
    uint32_t location_id_;
    uint32_t user_id_;
    Timestamp timestamp_;
    Mark mark_;
};

// struct VisitSorterByDate
// {
//     bool operator()(
//             const Visit& left,
//             const Visit& right)
//     {
//         return left.timestamp < right.time
//     }
// };

#endif // VISIT_H_INCLUDED
