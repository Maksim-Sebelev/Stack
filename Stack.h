#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "ColorPrint.h"

//--------------------------------------------------------------------------------------------------------------------------------
#define DEBUG    
#define STACK_CANARY     
#define DATA_CANARY
#define STACK_HASH
#define DATA_HASH
#define POISON
//--------------------------------------------------------------------------------------------------------------------------------

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


struct Warnings
{
    unsigned char PopInEmptyStack             : 1;
    unsigned char TooBigCapacity              : 1;
    unsigned char PushInFullStack             : 1;
};

 
struct FatalErrors
{
    unsigned char StackNull                   : 1;
    unsigned char DataNull                    : 1;
    unsigned char CallocCtorNull              : 1;
    unsigned char ReallocPushNull             : 1;
    unsigned char ReallocPopNull              : 1;
    ON_SCANARY
    (
    unsigned char LeftStackCanaryChanged      : 1;
    unsigned char RightStackCanaryChanged     : 1;
    )
    ON_DCANARY
    (
    unsigned char LeftDataCanaryChanged       : 1;
    unsigned char RightDataCanaryChanged      : 1;
    )
    ON_POISON
    (
    unsigned char DataElemBiggerSizeNotPoison : 1;
    )
    ON_SHASH
    (    
    unsigned char StackHashChanged            : 1;
    )
    ON_DHASH
    (
    unsigned char DataHashChanged             : 1;
    )
    ON_DEBUG
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


struct ErrorType
{
    unsigned int IsFatalError : 1;
    unsigned int IsWarning    : 1;
    Warnings     Warning;
    FatalErrors  FatalError;
    const char*  File;
    int          Line;
    const char*  Func;
};

//operation with stack

ErrorType Ctor               (Stack_t* Stack, const size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name));
ErrorType Dtor               (Stack_t* Stack);
ErrorType PrintStack         (Stack_t* Stack);
ErrorType PrintLastStackElem (Stack_t* Stack);
ErrorType Push               (Stack_t* Stack, StackElem_t PushElem);
ErrorType Pop                (Stack_t* Stack, StackElem_t* PopElem);

//--------------------------------------------------------------------------------------------------------------------------

//stack error func

#define VERIF(StackPtr, Err) Verif(StackPtr, &Err ON_DEBUG(, __FILE__, __LINE__, __func__))

ON_DEBUG
(
void Dump(const Stack_t* Stack, const char* File, int Line, const char* Func);
#define DUMP(Stack) Dump(Stack, __FILE__, __LINE__, __func__)
)

#define RETURN_IF_ERR_OR_WARN(StackPtr, Err) do                      \
{                                                                     \
    ErrorType ErrCopy = Err;                                           \
    Verif(Stack, &ErrCopy ON_DEBUG(, __FILE__, __LINE__, __func__));    \
    if (ErrCopy.IsFatalError == 1 || ErrCopy.IsWarning == 1)             \
    {                                                                     \
        return ErrCopy;                                                    \
    }                                                                       \
} while (0)                                                                  \


#ifdef DEBUG
    #define ASSERT(Err) do                                 \
    {                                                       \
        ErrorType ErrCopy = Err;                             \
        AssertPrint(ErrCopy, __FILE__, __LINE__, __func__);   \
        if (ErrCopy.IsFatalError == 1)                         \
        {                                                       \
            COLOR_PRINT(CYAN, "abort() in 3, 2, 1...\n");        \
            abort();                                              \
        }                                                          \
    } while (0)                                                     \

#else
    #define ASSERT(Err) AssertPrint(Err, __FILE__, __LINE__, __func__)
#endif

void AssertPrint (ErrorType Err, const char* File, int Line, const char* Func);


#endif
