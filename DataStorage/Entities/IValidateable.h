#ifndef I_VALIDATEABLE_H_INCLUDED
#define I_VALIDATEABLE_H_INCLUDED
#pragma once

class IValidateable
{
public:
    virtual ~IValidateable() = default;

    virtual bool Validate(
            const std::string& content) const final
    {
        return Validate(content.c_str());
    }


    virtual bool Validate(
            const char* content) const = 0;
};

#endif // I_VALIDATEABLE_H_INCLUDED