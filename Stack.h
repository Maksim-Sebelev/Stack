#ifndef STACK_H
#define STACK_H

#include <stdio.h>
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
typedef int StackCanary_t;
typedef StackElem_t DataCanary_t;


const size_t MinCapacity = 2  ON_DCANARY(+ 2);
const size_t MaxCapacity = 2097152 ON_DCANARY(+ 2); // 2^21 = 2097152

ON_POISON(const int Poison = 0xDEEEEEAD;)

ON_DEBUG
(
struct NamePlaceVar
{
    const char* File;
    int Line;
    const char* Func;
    const char* Name;
};
)

struct Stack_t
{
    ON_SCANARY(StackCanary_t LeftStackCanary;)
    ON_DEBUG(NamePlaceVar Var;)
    StackElem_t* Data; 
    ON_DHASH(int DataHash;)
    size_t Size;
    size_t Capacity;
    ON_SHASH(int StackHash;)
    ON_SCANARY(StackCanary_t RightStackCanary;)
};

#endif
