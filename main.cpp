#include "Stack.hpp"

int main()
{
    Stack_t Stack = {};
    StackElem_t PopELem = 0;

    STACK_ASSERT(StackCtor(&Stack, 8));          //stack initialization
    for (int i = 0; i < 1050; i++)
    {
        STACK_ASSERT(StackPush(&Stack, 10));         //put element in stack end
    }

    char* danger = (char*) Stack.data;

    for (int i = -8; i < 0; i++)
    {
        danger[i] = 0;
    }

    STACK_ASSERT(PrintStack(&Stack));

    STACK_ASSERT(StackDtor(&Stack));             //free memory that stack used

    return EXIT_SUCCESS;
}


// int main()
// {
//     Stack_t Stack = {};
//     StackElem_t PopELem = 0;

//     STACK_ASSERT(StackCtor(&Stack, 8));          //stack initialization
//     STACK_ASSERT(StackPush(&Stack, 10));         //put element in stack end
//     STACK_ASSERT(StackPop(&Stack, &PopELem));    //remove last element in stack
//     STACK_ASSERT(PrintStack(&Stack));            //show all element in stack
//     STACK_ASSERT(StackDtor(&Stack));             //free memory that stack used

//     return EXIT_SUCCESS;
// }
