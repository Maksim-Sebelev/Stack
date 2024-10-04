#ifndef ERROR_FUNC_H
#define ERROR_FUNC_H

#include <stdlib.h>

#include "Stack.h"
#include "ColorPrint.h"

struct Warnings
{
    unsigned char PopInEmptyStack             : 1;
    unsigned char ToBigCapacity               : 1;
    unsigned char PushInFullStack             : 1;
};

 
struct FatalErrors
{
    unsigned int StackNull                   : 1;
    unsigned int DataNull                    : 1;
    unsigned int CallocCtorNull              : 1;
    unsigned int ReallocPushNull             : 1;
    unsigned int ReallocPopNull              : 1;
    ON_SCANARY
    (
    unsigned int LeftStackCanaryChanged      : 1;
    unsigned int RightStackCanaryChanged     : 1;
    )
    ON_DCANARY
    (
    unsigned int LeftDataCanaryChanged       : 1;
    unsigned int RightDataCanaryChanged      : 1;
    )
    ON_POISON
    (
    unsigned int DataElemBiggerSizeNotPoison : 1;
    )
    ON_SHASH
    (    
    unsigned int StackHashChanged            : 1;
    )
    ON_DHASH
    (
    unsigned int DataHashChanged             : 1;
    )
    ON_DEBUG
    (
    unsigned int SizeBiggerCapacity          : 1;
    unsigned int CapacitySmallerMin          : 1;
    unsigned int CapacityBiggerMax           : 1;
    unsigned int CtorStackNameNull           : 1;
    unsigned int CtorStackFileNull           : 1;
    unsigned int CtorStackFuncNull           : 1;
    unsigned int CtorStackLineNegative       : 1;
    )
};


struct ErrorType
{
    unsigned int IsFatalError : 1;
    unsigned int IsWarning    : 1;
    Warnings Warning;
    FatalErrors FatalError;
    const char* File;
    int Line;
    const char* Func;
};


#define ERR_RETURN_WARN_PRINT(StackPtr, Err) do                      \
{                                                                     \
    ErrorType ErrCopy = Err;                                           \
    Verif(Stack, &ErrCopy);                                             \
    ON_DEBUG(ErrPlaceCtor(&ErrCopy, __FILE__, __LINE__, __func__);)      \
    if (ErrCopy.IsFatalError == 1 || ErrCopy.IsWarning == 1)              \
    {                                                                      \
        return ErrCopy;                                                     \
    }                                                                        \
} while (0)                                                                   \


#define RETURN(StackPtr, Err) do                                      \
{                                                                      \
    ErrorType ErrCopy = Err;                                            \
    ON_DEBUG(ErrPlaceCtor(&ErrCopy, __FILE__, __LINE__, __func__);)      \
    Verif(StackPtr, &ErrCopy);                                            \
    return ErrCopy;                                                        \
} while (0)                                                                 \


#define DUMP(Stack) Dump(Stack, __FILE__, __LINE__, __func__);

#ifdef DEBUG
    #define ASSERT(Err) do                                 \
    {                                                       \
        ErrorType ErrCopy = Err;                             \
        AssertPrint(ErrCopy, __FILE__, __LINE__, __func__);   \
        if (ErrCopy.IsFatalError == 1)                         \
        {                                                       \
            COLOR_PRINT(CYAN, "abort() in 3, 2, 1...\n")         \
            abort();                                              \
        }                                                          \
    } while (0)                                                     \

#else
    #define ASSERT(Err) AssertPrint(Err, __FILE__, __LINE__, __func__);
#endif

void Verif       (Stack_t* Stack, ErrorType* Error);
void Dump        (Stack_t* Stack, const char* File, int Line, const char* Func);
void PrintError  (ErrorType Error);
void PrintPlace  (const char* File, const int Line, const char* Function);
void AssertPrint (ErrorType Err, const char* File, int Line, const char* Func);
ON_DEBUG
(
void ErrPlaceCtor(ErrorType* Err, const char* File, int Line, const char* Func);
)

#endif
