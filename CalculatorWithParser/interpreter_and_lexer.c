#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter_and_lexer.h"

int main()
{
    char expression[MAX_EXPRESSION_SIZE];
    // strcpy(expression, "1 + 2 * 3");
    
    printf("Enter an expression. Ex.:  (4 * (1 + 3)) + 2 * 2   or\n\
exit to exit: ");

    fgets(expression, MAX_EXPRESSION_SIZE, stdin);
    expression[strlen(expression) - 1] = '\0';

    Lexer* lexer = lexer_new(expression);
    Interpreter* interpreter = interpreter_new(lexer);

    printf("\n%s = %g\n", expression, interpreter_run(interpreter));

    free(lexer);
    free(interpreter);

    return 0;
}
