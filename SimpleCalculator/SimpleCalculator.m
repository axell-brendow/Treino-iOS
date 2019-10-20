#import <Foundation/Foundation.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://sweettutos.com/2012/08/11/objective-c-on-windows-yes-you-can/

#define operate(operator) (operand1 operator operand2)

#define operation_header(name, operator)\
    - (double) name;

#define operation(name, operator)\
    - (double) name\
    {\
        self->error = 0;\
        return self->result = operate(operator);\
    }

#define OPERATION_SIZE 100

@interface Calculator : NSObject
{
    @private
        int error;
        char errorMsg[500];
        double operand1;
        double operand2;
        double result;
        char myOperator[10];
        char myOperation[OPERATION_SIZE];
}

- (Calculator*) initMe;
- (void) readOperation;
- (void) printResult;

operation_header(sum, +)
operation_header(subtract, -)
operation_header(multiply, *)
operation_header(divide, /)

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
        self->myOperator[0] = '\0';
        self->myOperation[0] = '\0';
    }

    return self;
}

- (void) readOperation
{
    printf("Enter operation. Ex.:  1 + 3  or  7 - 5  or  7 * 7  or  5 / 2  or\n\
exit to exit: ");

    fgets(myOperation, OPERATION_SIZE, stdin);
    myOperation[strlen(myOperation) - 1] = '\0';

    if (strstr(myOperation, "exit") != NULL) strcpy(myOperator, "exit");

    else sscanf(myOperation, "%lf%s%lf", &operand1, myOperator, &operand2);
}

- (void) printResult
{
    if (error) printf("\n\nERROR: %s", errorMsg);

    else printf("\n\n%g %s %g = %g", operand1, myOperator, operand2, result);
}

operation(sum, +)
operation(subtract, -)
operation(multiply, *)
operation(divide, /)

- (double) doOperation
{
    if (strcmp(myOperator, "/") == 0 && operand2 == 0)
    {
        error = 1;
        strcpy(errorMsg, "Can not divide by zero");
    }

    else
    {
        if      (strcmp(myOperator, "+") == 0) [self sum];
        else if (strcmp(myOperator, "-") == 0) [self subtract];
        else if (strcmp(myOperator, "*") == 0) [self multiply];
        else if (strcmp(myOperator, "/") == 0) [self divide];
        else
        {
            error = 1;
            strcpy(errorMsg, "Invalid operand");
        }
    }

    return result;
}

- (void) calculate
{
    [self readOperation];

    while (strcmp(myOperator, "exit") != 0)
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
