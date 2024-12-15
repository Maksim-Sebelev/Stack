#ifndef STACK_H
#define STACK_H

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ColorPrint.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#define STACK_DEBUG
#define STACK_CANARY
#define STACK_DATA_CANARY
#define STACK_HASH
#define STACK_DATA_HASH
#define STACK_DATA_POISON

//----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef STACK_DEBUG
    #define ON_STACK_DEBUG(...) __VA_ARGS__
    #define OFF_STACK_DEBUG(...)
#else
    #define ON_STACK_DEBUG(...)
    #define OFF_STACK_DEBUG(...) __VA_ARGS__
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef STACK_CANARY
    #define ON_STACK_CANARY(...) __VA_ARGS__
#else
    #define ON_STACK_CANARY(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef STACK_DATA_CANARY
    #define ON_STACK_DATA_CANARY(...)  __VA_ARGS__
#else
    #define ON_STACK_DATA_CANARY(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef STACK_HASH
    #define ON_STACK_HASH(...) __VA_ARGS__
#else   
    #define ON_STACK_HASH(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef STACK_DATA_HASH
    #define ON_STACK_DATA_HASH(...) __VA_ARGS__
#else
    #define ON_STACK_DATA_HASH(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef STACK_DATA_POISON
    #define ON_STACK_DATA_POISON(...) __VA_ARGS__
#else
    #define ON_STACK_DATA_POISON(...)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef int StackElem_t;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_STACK_CANARY(typedef uint64_t StackCanary_t;)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Stack_t
{
    ON_STACK_CANARY(StackCanary_t leftStackCanary;)
    size_t size;
    size_t capacity;
    StackElem_t* data;
    ON_STACK_HASH(uint64_t stackHash;)
    ON_STACK_DATA_HASH(uint64_t dataHash;)
    ON_STACK_CANARY(StackCanary_t rightStackCanary;)
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Warnings
{
    unsigned char PopInEmptyStack             : 1;
    unsigned char TooBigCapacity              : 1;
    unsigned char PushInFullStack             : 1;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct FatalErrors
{
    unsigned char StackNull                   : 1;
    unsigned char DataNull                    : 1;
    unsigned char CallocCtorNull              : 1;
    unsigned char ReallocPushNull             : 1;
    unsigned char ReallocPopNull              : 1;
    ON_STACK_CANARY
    (
    unsigned char LeftStackCanaryChanged      : 1;
    unsigned char RightStackCanaryChanged     : 1;
    )
    ON_STACK_DATA_CANARY
    (
    unsigned char LeftDataCanaryChanged       : 1;
    unsigned char RightDataCanaryChanged      : 1;
    )
    ON_STACK_DATA_POISON
    (
    unsigned char DataElemBiggerSizeNotPoison : 1;
    )
    ON_STACK_HASH
    (    
    unsigned char StackHashChanged            : 1;
    )
    ON_STACK_DATA_HASH
    (
    unsigned char DataHashChanged             : 1;
    )
    ON_STACK_DEBUG
    (
    unsigned char SizeBiggerCapacity          : 1;
    unsigned char CapacitySmallerMin          : 1;
    unsigned char CapacityBiggerMax           : 1;
    unsigned char CtorStackNameNull           : 1;
    unsigned char CtorStackFileNull           : 1;
    unsigned char CtorStackFuncNull           : 1;
    unsigned char CtorStackLineNegative       : 1;
    )
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

struct ErrorType
{
    unsigned int IsFatalError : 1;
    unsigned int IsWarning    : 1;
    Warnings     Warning;
    FatalErrors  FatalError;
    const char*  file;
    int          line;
    const char*  func;
};

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ErrorType StackCtor               (Stack_t* stack, size_t StackDataSize);
ErrorType StackDtor               (Stack_t* stack);
ErrorType PrintStack              (Stack_t* stack);
ErrorType PrintLastStackElem      (Stack_t* stack);
ErrorType StackPush               (Stack_t* stack, StackElem_t PushElem);
ErrorType StackPop                (Stack_t* stack, StackElem_t* PopElem);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ON_STACK_DEBUG
(
void Dump(const Stack_t* stack, const char* file, int line, const char* func);

#define DUMP(stack) Dump(stack, __FILE__, __LINE__, __func__)
)

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void AssertPrint (ErrorType Err, const char* file, int line, const char* func);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef STACK_DEBUG
    #define STACK_ASSERT(Err) do                             \
    {                                                         \
        ErrorType ErrCopy = Err;                               \
        if (ErrCopy.IsFatalError || ErrCopy.IsWarning)          \
        {                                                        \
            AssertPrint(ErrCopy, __FILE__, __LINE__, __func__);   \
            COLOR_PRINT(CYAN, "abort() in 3, 2, 1...\n");          \
            abort();                                                \
        }                                                            \
    } while (0)                                                       \

#else
    #define STACK_ASSERT(Err) AssertPrint(Err, __FILE__, __LINE__, __func__)
#endif

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

#endif
