#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#define DEBUG
#define STACK_CANARY
#define DATA_CANARY
#define STACK_HASH
#define DATA_HASH
#define POISON

#ifdef DEBUG
    #define ON_DEBUG(...) __VA_ARGS__
    #define OFF_DEBUG(...)
#else
    #define ON_DEBUG(...)
    #define OFF_DEBUG(...) __VA_ARGS__
#endif


#ifdef STACK_CANARY
    #define ON_SCANARY(...) __VA_ARGS__
#else
    #define ON_SCANARY(...)
#endif


#ifdef DATA_CANARY
    #define ON_DCANARY(...)  __VA_ARGS__
#else
    #define ON_DCANARY(...)
#endif


#ifdef STACK_HASH
    #define ON_SHASH(...) __VA_ARGS__
#else   
    #define ON_SHASH(...)
#endif


#ifdef DATA_HASH
    #define ON_DHASH(...) __VA_ARGS__
#else
    #define ON_DHASH(...)
#endif


#ifdef POISON
    #define ON_POISON(...) __VA_ARGS__
#else
    #define ON_POISON(...)
#endif


typedef int StackElem_t;

ON_SCANARY(typedef uint64_t StackCanary_t;)
ON_DCANARY(typedef uint64_t DataCanary_t;)

const size_t MinCapacity = 2;
const size_t MaxCapacity = 1<<21;

ON_POISON(const StackElem_t Poison = 0xDEEEEEAD;)

ON_DEBUG
(
struct NamePlaceVar
{
    const char* File;
    int         Line;
    const char* Func;
    const char* Name;
};
)

struct Stack_t
{
    ON_SCANARY(StackCanary_t LeftStackCanary;)
    size_t Size;
    size_t Capacity;
    StackElem_t* Data;
    ON_DEBUG(NamePlaceVar Var;)
    ON_DHASH(uint64_t DataHash;)
    ON_SHASH(uint64_t StackHash;)
    ON_SCANARY(StackCanary_t RightStackCanary;)
};

#endif
