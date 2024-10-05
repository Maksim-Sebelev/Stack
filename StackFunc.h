#ifndef STACK_FUNC_H
#define STACK_FUNC_H

#include "Stack.h"
#include "ErrorFunc.h"


#ifdef DEBUG
    #define CTOR(StackPtr, StackSize, Name) Ctor(StackPtr, StackSize, __FILE__, __LINE__, __func__, Name)
#else
    #define CTOR(StackPtr, StackSize)       Ctor(StackPtr, StackSize)
#endif

ON_SCANARY
(
const StackCanary_t LeftStackCanary  = 0xDEADDEAD;
const StackCanary_t RightStackCanary = 0xDEADDEDA;
)

ON_DCANARY
(
const DataCanary_t LeftDataCanary  = 0xEDADEDA;
const DataCanary_t RightDataCanary = 0xDEDDEAD;
)

const unsigned int CapPushReallocCoef = 2;
const unsigned int CapPopReallocCoef  = 4;

ErrorType Ctor             (Stack_t* Stack, const size_t StackDataSize ON_DEBUG(, const char* File, int Line, const char* Func, const char* Name));
ErrorType Dtor             (Stack_t* Stack);
ErrorType Push             (Stack_t*, StackElem_t PushElem);
ErrorType Pop              (Stack_t* Stack, StackElem_t* PopElem);
ErrorType PrintStack       (Stack_t* Stack);
ON_SHASH
(
uint64_t CalcRealStackHash (Stack_t* Stack);
)

ON_DCANARY
(
DataCanary_t GetLeftDataCanary       (Stack_t* Stack);
DataCanary_t GetRightDataCanary      (Stack_t* Stack);
size_t       GetLeftDataCanaryIndex  (Stack_t* Stack);
);
size_t       GetRightDataCanaryIndex (Stack_t* Stack);

size_t GetNewCtorCapacity            (size_t StackDataSize);
size_t GetNewPushCapacity            (Stack_t* Stack);
size_t GetNewPopCapacity             (Stack_t* Stack);
size_t GetNewCapacity                (size_t Capacity); 



ErrorType PushReallocWithNewCapacity (Stack_t* Stack, ErrorType* Err);
ErrorType PopReallocWithNewCapacity  (Stack_t* Stack, ErrorType* Err);
ErrorType CtorCallocDataWithNewCapacity             (Stack_t* Stack, ErrorType* Err);
ErrorType DtorFreeData               (Stack_t* Stack, ErrorType* Err);


#endif
