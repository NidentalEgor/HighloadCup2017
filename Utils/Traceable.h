#ifndef TRACEABLE_H_INCLUDED
#define TRACEABLE_H_INCLUDED
#pragma once

#include <iostream>
#include <string>

#include "../Submodules/fmt/fmt/format.h"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

// #define DEBUG_LOG

class Traceable
{
public:
    template <typename... Args>
    static void DebugTrace(
            const char* file_name,
            const int line,    
            const char* format,
            const Args& ... args)
    {
    #ifdef DEBUG_LOG
        if (!file_name || !format)
        {
            return;
        }

        fmt::print("{}: line {}: ", file_name, line);
        fmt::print(format, args...);
        fmt::print("\n");
    #endif
    }

    template <typename... Args>
    static void DebugTrace(
            const char* format,
            const Args& ... args)
    {
    #ifdef DEBUG_LOG
        if (!format)
        {
            return;
        }

        fmt::print(format, args...);
        fmt::print("\n");
    #endif
    }

    static void DebugTrace(
            const char* message)
    {
    #ifdef DEBUG_LOG
        std::cout << message << std::endl;
    #endif
    }

    static void DebugTraceCharacters(
            const char* message,
            const size_t characters_amount)
    {
    #ifdef DEBUG_LOG
        std::cout.write(message, characters_amount);
        std::cout << std::endl;
    #endif
    }

    template <typename... Args>
    static void Trace(
            const char* file_name,
            const int line,    
            const char* format,
            const Args& ... args)
    {
        if (!file_name || !format)
        {
            return;
        }

        fmt::print("{}: line {}: ", file_name, line);
        fmt::print(format, args...);
        fmt::print("\n");
    }

    template <typename... Args>
    static void Trace(
            const char* format,
            const Args& ... args)
    {
        if (!format)
        {
            return;
        }

        fmt::print(format, args...);
        fmt::print("\n");
    }

    static void Trace(
            const char* message)
    {
        fmt::print("{}\n",message);
    }

    static void TraceCharacters(
            const char* message,
            const size_t characters_amount)
    {
        std::cout.write(message, characters_amount);
        std::cout << std::endl;
    }
};

#endif // TRACEABLE_H_INCLUDED