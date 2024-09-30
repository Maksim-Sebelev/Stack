#ifndef ERROR_FUNC_H
#define ERROR_FUNC_H

#include <stdlib.h>

#include "Stack.h"
#include "ColorPrint.h"

enum ErrorCode
{
    NO_ERR = 0,
    POP_IN_EMPTY_STACK,
    TO_BIG_CAPACITY,
    PUSH_IN_FULL_STACK,
    LEFT_STACK_CANARY_CHANGED,
    RIGHT_STACK_CANARY_CHANGED,
    LEFT_DATA_CANARY_CHANGED,
    RIGHT_DATA_CANARY_CHANGED

    ON_DEBUG
    (,
    STACK_NULL,
    DATA_NULL,
    SIZE_BIGGER_CAPACITY,
    CALLOC_CTOR_NULL,
    REALLOC_PUSH_NULL,
    REALLOC_POP_NULL,
    CAPACITY_SMALLER_MIN,
    CAPACITY_BIGGER_MAX,
    DATA_ELEM_BIGGER_SIZE_IS_NOT_POISON,
    STACK_HASH_CHANGED,
    DATA_HASH_CHANGED,
    STACK_CTOR_NAME_NULL,
    STACK_CTOR_FILE_NULL,
    STACK_CTOR_FUNC_NULL,
    STACK_CTOR_LINE_NEGATIVE
    )
};


#define ERROR_RETURN(Err) do    \
{                                \
    ErrorCode ErrCopy = Err;      \
    if (ErrCopy != NO_ERR)         \
    {                               \
        return ErrCopy;              \
    }                                 \
} while (0);                           \


#define DUMP(Stack) do                                                                     \
{                                                                                           \
    COLOR_PRINT(GREEN, "\nDump BEGIN\n\n");                                                  \
    COLOR_PRINT(VIOLET, "Where Dump made:\n");                                                \
    COLOR_PRINT(WHITE, "File [%s]\nLine [%d]\nFunc [%s]\n\n", __FILE__, __LINE__, __func__);   \
    Dump(Stack);                                                                                \
    COLOR_PRINT(GREEN, "\n\nDump END\n\n");                                                      \
} while (0);                                                                                      \


#ifdef DEBUG

    #define ASSERT(Err) do                                                                  \
    {                                                                                        \
        ErrorCode ErrCopy = Err;                                                              \
        PrintError(ErrCopy, __FILE__, __LINE__, __func__);                                     \
        if (ErrCopy != NO_ERR && ErrCopy != POP_IN_EMPTY_STACK && ErrCopy != TO_BIG_CAPACITY)   \
        {                                                                                        \
            COLOR_PRINT(CYAN, "abort() in 3, 2, 1...\n");                                         \
            abort();                                                                               \
        }                                                                                           \
    } while (0);                                                                                     \

#else
    #define ASSERT(Err) do                                                                  \
    {                                                                                        \
        ErrorCode ErrCopy = Err;                                                              \
        PrintError(ErrCopy, __FILE__, __LINE__, __func__);                                     \
    } while (0);                                                                                \

#endif


ErrorCode Verif(Stack_t* Stack);
void PrintError(const ErrorCode Error, const char* File, const int Line, const char* Function);
void Dump(Stack_t* Stack);
void PrintPlace(const char* File, const int Line, const char* Function);


#endif