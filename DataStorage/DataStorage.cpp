#include <dirent.h>
// #include <experimental/filesystem>
#include <iostream>

#include "DataStorage.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <cstdio>

using namespace rapidjson;

void DataStorage::LoadData(const std::string& folder_path)
{
    DIR* directory;
    struct dirent* entity;
    if ((directory = opendir (folder_path.c_str())) != nullptr)
    {
        while ((entity = readdir (directory)) != nullptr)
        {
            const std::string file_path =
                    folder_path + std::string(entity->d_name);
            switch (entity->d_name[0])
            {
                case 'l':
                {
                    ParseFile(file_path, "locations", locations_);
                } break;

                case 'u':
                {
                    ParseFile(file_path, "users", users_);
                } break;

                case 'v':
                {
                    ParseFile(file_path, "visits", visits_);
                } break;
            }
        }
        closedir(directory);
    }
    else
    {
      /* could not open directory */
      perror ("");
      //return EXIT_FAILURE;
    }
}

template <typename T>
void DataStorage::ParseFile(
        const std::string& file_path,
        const std::string& entities_name,
        Container<T>& container)
{
    std::cout << file_path << std::endl;
    FILE* file =
    fopen(
        file_path.c_str(),
        "r");
    char buffer[65536];
    FileReadStream file_read_stream(
        file,
        buffer,
        sizeof(buffer));

    Document entities;
    entities.ParseStream(file_read_stream);

    fclose(file);
    
    const Value& data = entities[entities_name.c_str()];
    assert(data.IsArray());

    for (const auto& entity : data.GetArray())
    {
        T element(entity);
        // T element;
        // element.Deserialize(entity);
        container.emplace(element.GetId(), element);
    }

    std::cout << "Container size = " << container.size() << std::endl;
}
