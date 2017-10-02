#ifndef DATA_TYPES_H_INCLUDED
#define DATA_TYPES_H_INCLUDED
#pragma once

#include <string>

using Timestamp = long long;

template<typename T>
struct EntitiesName
{
    const std::string entities_name = "";
};

#endif // DATA_TYPES_H_INCLUDED