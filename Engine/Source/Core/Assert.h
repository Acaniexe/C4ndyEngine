#pragma once
#include "Core.h"
#include <cstdio>

namespace C4ndy::Internal
{
    void AssertionFail(const char* expr, const char* file, int line, const char* msg = nullptr)
    {
        if(msg)
            fprintf(stderr, "[C4NDY ASSERT]: %s\n Expression: %s\n File        : %s\n Line          : %d\n", msg, expr, file, line);
        else
            fprintf(stderr, "[C4NDY ASSERT]: Assertion Failed\n  Expression : %s\n  File       : %s\n  Line       : %d\n", expr, file, line);
        C4NDY_DEBUG_BREAK;
    }
}

#ifdef _DEBUG
    #define C4NDY_ASSERT(expr) \
        do { if(!(expr)) { C4ndy::Internal::AssertionFail(#expr, __FILE__, __LINE__); } } while(0)
    
    #define C4NDY_ASSERT_MSG(expr, msg) \
        do { if(!(expr)) { C4ndy::Internal::AssertionFail(#expr, __FILE__, __LINE__, msg); } } while(0)
#else
    #define C4NDY_ASSERT(expr)          do {} while(0)
    #define C4NDY_ASSERT_MSG(expr, msg) do {} while(0)
#endif