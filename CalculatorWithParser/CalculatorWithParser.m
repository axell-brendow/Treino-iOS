#import <Foundation/Foundation.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter_and_lexer.h"

// https://sweettutos.com/2012/08/11/objective-c-on-windows-yes-you-can/

#define OPERATION_SIZE 100

@interface Calculator : NSObject
{
    @private
        int error;
        char errorMsg[500];
        double result;
        char myOperation[OPERATION_SIZE];
        Lexer* lexer;
        Interpreter* interpreter;
}

- (Calculator*) initMe;
- (void) readOperation;
- (void) printResult;
- (double) doOperation;
- (void) calculate;
@end

@implementation Calculator

- (Calculator*) initMe
{
    self = [super init];

    if (self)
    {
        self->errorMsg[0] = '\0';
        self->myOperation[0] = '\0';
        self->lexer = NULL;
        self->interpreter = NULL;
    }

    return self;
}

- (void) readOperation
{
    printf("Enter an expression. Ex.:  (4 * (1 + 3)) + 2 * 2   or\n\
exit to exit: ");

    fgets(myOperation, OPERATION_SIZE, stdin);
    myOperation[strlen(myOperation) - 1] = '\0';
}

- (void) printResult
{
    printf("\n\n%s = %g", myOperation, result);
}

- (double) doOperation
{
    lexer = lexer_new(myOperation);
    interpreter = interpreter_new(lexer);

    result = interpreter_run(interpreter);

    free(lexer);
    free(interpreter);

    return result;
}

- (void) calculate
{
    [self readOperation];

    while (strstr(myOperation, "exit") == NULL)
    {
        [self doOperation];
        [self printResult];
        printf("\n\n---------------------------------------\n\n");
        [self readOperation];
    }
}

@end

int main (int argc, const char * argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    Calculator * calculator = [[Calculator alloc] initMe];

    [calculator calculate];
    NSLog(@"");

    [pool drain];
    return 0;
}
