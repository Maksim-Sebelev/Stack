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



ON_SCANARY
(
const StackCanary_t LeftStackCanary  = 0xDEADDEAD;
const StackCanary_t RightStackCanary = 0xDEADDED;
)

ON_DCANARY
(
const DataCanary_t LeftDataCanary   = 0xEDADEDA;
const DataCanary_t RightDataCanary  = 0xDEDDEAD;
)

// Ctor()
ErrorType Ctor(Stack_t* Stack, const size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name));
ErrorType Dtor(Stack_t* Stack);
ErrorType Push(Stack_t*, StackElem_t PushElem);
ErrorType Pop(Stack_t* Stack, StackElem_t* PopElem);
ErrorType PrintStack(Stack_t* Stack);

ON_SHASH
(
int CalcRealStackHash(Stack_t* Stack);
);

ON_DCANARY
(
DataCanary_t GetLeftDataCanary(Stack_t* Stack);
DataCanary_t GetRightDataCanary(Stack_t* Stack);
size_t GetLeftDataCanaryIndex(Stack_t* Stack);
size_t GetRightDataCanaryIndex(Stack_t* Stack);
);
size_t GetStackCapacity(Stack_t* Stack);
size_t GetStackSize(Stack_t* Stack);
size_t GetNewPushCapacity(Stack_t* Stack);
size_t GetNewPopCapacity(Stack_t* Stack);
size_t GetDataBeginIndex(Stack_t* Stack);
size_t GetMaxCapacity();
size_t GetMinCapacity();


#endif

