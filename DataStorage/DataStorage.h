#ifndef DATA_STORAGE_H_INCLUDED
#define DATA_STORAGE_H_INCLUDED
#pragma once

#include <string>
#include <unordered_map>

#include "Location.h"
#include "User.h"
#include "Visit.h"
#include "DataTypes.h"

// template<Location>
// struct EntitiesName
// {
//     const std::string entities_name = "locations";
// };

// template<User>
// struct EntitiesName
// {
//     const std::string entities_name = "users";
// };

// template<Visit>
// struct EntitiesName
// {
//     const std::string entities_name = "visits";
// };

template <typename T>
using Container = std::unordered_map<uint32_t, T>;

class DataStorage
{
public:
    void LoadData(
            const std::string& folder_path);
private:
    template <typename T>
    void ParseFile(
            const std::string& file_path,
            const std::string& entities_name,            
            Container<T>& container);

private:
    Container<Location> locations_;
    Container<User> users_;
    Container<Visit> visits_;
};

#endif // DATA_STORAGE_H_INCLUDED