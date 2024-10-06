#include "StackFunc.h"

int main()
{
    Stack_t Stack = {};
    StackElem_t PopELem = 0;
    ASSERT(Ctor       (&Stack, 8 ON_DEBUG(, __FILE__, __LINE__, __func__, "Stack"))); //stack initialization
    ASSERT(Push       (&Stack, 0));         //put element to stack end    
    ASSERT(Pop        (&Stack, &PopELem));  //remove last element in stack
    ASSERT(PrintStack (&Stack));            //show all element in stack
    ASSERT(Dtor       (&Stack));            //free memory that stack used
    return 0;
}
