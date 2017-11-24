#ifndef DATA_TYPES_H_INCLUDED
#define DATA_TYPES_H_INCLUDED
#pragma once

#include <string>

using Id = uint32_t;

using Distance = uint32_t;

using Timestamp = long int;

using Mark = short;

enum class Gender
{
    Male,
    Female,
    Any
};

#endif // DATA_TYPES_H_INCLUDED