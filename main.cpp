#include "Stack.hpp"

int main()
{
    Stack_t Stack = {};
    StackElem_t PopELem = 0;

    STACK_ASSERT(StackCtor(&Stack, 8)); //stack initialization

    for (int i = 0; i < 5; i++)
    {
        STACK_ASSERT(StackPush(&Stack, i+1));         //put element in stack end
    }

    // STACK_ASSERT(StackPush               (&Stack, 5));         //put element in stack end
    // STACK_ASSERT(Pop                (&Stack, &PopELem)); //remove last element in stack
    STACK_ASSERT(PrintStack(&Stack));            //show all element in stack
    // STACK_ASSERT(PrintLastStackElem (&Stack));            //show last element in stack
    STACK_ASSERT(StackDtor(&Stack));            //free memory that stack used

    return EXIT_SUCCESS;
}
