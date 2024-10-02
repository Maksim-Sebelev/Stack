#ifndef STACK_H
#define STACK_H

#define DEBUG
#define CANARY

#include <stdio.h>
#include <limits.h>

typedef int StackElem_t;

#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
    #define OFF_DEBUG(...)

#else
    #define ON_DEBUG(...)
    #define OFF_DEBUG(...) __VA_ARGS__
#endif


#ifdef CANARY
    #define ON_CANARY(...) __VA_ARGS__
#else
    #define ON_CANARY(...)
#endif

const size_t MinCapacity = 2;
const size_t MaxCapacity = 1 << 21; // 2^21 = 2097152

const int Poison = 0xDEEEEEAD;

struct NamePlaceVar
{
    const char* File;
    int Line;
    const char* Func;
    const char* Name;
};

struct Stack_t
{
    StackElem_t LeftStackCanary;
    ON_DEBUG(NamePlaceVar Var;)
    StackElem_t* Data; 
    ON_DEBUG(int DataHash;)
    size_t Size;
    size_t Capacity;
    ON_DEBUG(int StackHash;)
    StackElem_t RightStackCanary;
};

#endif
