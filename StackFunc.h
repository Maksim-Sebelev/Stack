#ifndef STACK_FUNC_H
#define STACK_FUNC_H

#include "Stack.h"
#include "ErrorFunc.h"


#ifdef DEBUG
    #define CTOR(StackPtr, StackSize, Name) do                                           \
    {                                                                                     \
        ASSERT(Ctor(StackPtr, StackSize, __FILE__, __LINE__, __func__, Name));             \
    } while (0)                                                                             \

#else
    #define CTOR(StackPtr, StackSize) do                                           \
    {                                                                               \
        ASSERT(Ctor(StackPtr, StackSize));                                           \
    } while (0);                                                                      \
    
#endif

const unsigned int CapPushReallocCoef = 2;
const unsigned int CapPopReallocCoef  = 4;


typedef int Canary_t;

const Canary_t LeftStackCanary  = 0xDEADDEAD;
const Canary_t RightStackCanary = 0xDEADDED;

const Canary_t LeftDataCanary   = 0xEDADEDA;
const Canary_t RightDataCanary  = 0xDEDDEAD;

// Ctor()
ErrorType Ctor(Stack_t* Stack, const size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name));
ErrorType Dtor(Stack_t* Stack);
ErrorType Push(Stack_t*, StackElem_t PushElem);
ErrorType Pop(Stack_t* Stack, StackElem_t* PopElem);
ErrorType PrintStack(Stack_t* Stack);
ON_DEBUG
(
int CalcRealStackHash(Stack_t* Stack);
);



#endif

