#ifndef I_VALIDATEABLE_H_INCLUDED
#define I_VALIDATEABLE_H_INCLUDED
#pragma once

class IValidateable
{
public:
    virtual bool Validate(
            const std::string& content) final
    {
        return Validate(content.c_str());
    }


    virtual bool Validate(
            const char* content) = 0;
};

#endif // I_VALIDATEABLE_H_INCLUDED