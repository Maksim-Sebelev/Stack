#ifndef STACK_FUNC_H
#define STACK_FUNC_H

#include "Stack.h"
#include "ErrorFunc.h"


#ifdef DEBUG
    #define CTOR(StackPtr, StackSize, Name) do                                           \
    {                                                                                     \
        ASSERT(Ctor(StackPtr, StackSize, __FILE__, __LINE__, __func__, Name));             \
    } while (0);                                                                            \

#else
    #define CTOR(StackPtr, StackSize) do                                           \
    {                                                                               \
        ASSERT(Ctor(StackPtr, StackSize));                                           \
    } while (0);                                                                      \
    
#endif

const unsigned int CapPushReallocCoef = 2;
const unsigned int CapPopReallocCoef  = 4;

const int LeftStackCanary  = 0xDEADDEAD;
const int RightStackCanary = 0xDEADDED;
const int LeftDataCanary   = 0xEDADEDA;
const int RightDataCanary  = 0xDEDDEAD;


ErrorCode Ctor(Stack_t* Stack, const size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name));
ErrorCode Dtor(Stack_t* Stack);
ErrorCode Push(Stack_t*, StackElem_t PushElem);
ErrorCode Pop(Stack_t* Stack, StackElem_t* PopElem);
ErrorCode PrintStack(Stack_t* Stack);


#endif

