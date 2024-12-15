#include "Stack.hpp"

int main()
{
    Stack_t Stack        = {};

    size_t StackCapacity = 128;
    StackElem_t PushElem = 8; 
    StackElem_t PopELem  = 0;

    STACK_ASSERT(StackCtor  (&Stack, StackCapacity));     //stack initialization
    STACK_ASSERT(StackPush  (&Stack, PushElem));         //put element in stack end
    STACK_ASSERT(StackPop   (&Stack, &PopELem));        //remove last element in stack
    STACK_ASSERT(PrintStack (&Stack));                 //show all element in stack
    STACK_ASSERT(StackDtor  (&Stack));                //free memory that stack used

    return EXIT_SUCCESS;
}
