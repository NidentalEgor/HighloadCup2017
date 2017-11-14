#ifndef DATA_EXTRACTER_H_INCLUDED
#define DATA_EXTRACTER_H_INCLUDED
#pragma once

#include <string>

class DataExtracter
{
public:
    bool ExtractData(
            const std::string& archive_path,
            const std::string& distanation_folder);

private:

};

#endif // DATA_EXTRACTER_H_INCLUDED