#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://ruslanspivak.com/lsbasi-part5/

const char *LPAREN = "LPAREN";
const char *RPAREN = "RPAREN";
const char *NUMBER = "NUMBER";
const char *PLUS = "PLUS";
const char *MINUS = "MINUS";
const char *MUL = "MUL";
const char *DIV = "DIV";
const char *EOFTOKEN = "EOF";

// -------------------- Class Token

typedef struct Token Token;

struct Token
{
    char type[30];
    double value;
};

Token *token_new(const char *type, double value)
{
    Token *token = (Token *) malloc(sizeof(Token));

    strcpy(token->type, type);
    token->value = value;

    return token;
}

int equals(const char *str1, const char *str2)
{
    return strcmp(str1, str2) == 0;
}

int isWhitespace(char c)
{
    return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

int isDigit(char c)
{
    return c >= '0' && c <= '9';
}

// -------------------- Class Lexer

typedef struct Lexer Lexer;

#define MAX_EXPRESSION_SIZE 10000

struct Lexer
{
    char expression[MAX_EXPRESSION_SIZE];
    int pos;
    int exprLength;
    char current_char;
};

Lexer* lexer_new(const char *expression)
{
    Lexer* lexer = (Lexer *) malloc(sizeof(Lexer));

    strcpy(lexer->expression, expression);
    lexer->pos = 0;
    lexer->exprLength = strlen(expression);
    lexer->current_char = expression[lexer->pos];

    return lexer;
}

void lexer_error(Lexer* lexer, const char *msg)
{
    fprintf(stderr, "\nERROR: %s\n%s\n", msg, lexer->expression);
    char spaces[lexer->pos + 2];

    memset(spaces, ' ', lexer->pos); // Preenche spaces com ' '

    spaces[lexer->pos] = '^';
    spaces[lexer->pos + 1] = '\0';
    puts(spaces);

    exit(1);
}

void lexer_advance(Lexer* lexer)
{
    lexer->pos++;
    lexer->current_char = ( lexer->pos >= lexer->exprLength ) ?
        EOF : lexer->expression[lexer->pos];
}

void lexer_skip_spaces(Lexer* lexer)
{
    while (lexer->current_char != EOF && isWhitespace(lexer->current_char))
    {
        lexer_advance(lexer);
    }
}

double lexer_get_double(Lexer* lexer)
{
    int numberOfDots = 0, offset = 0;
    char doubleStr[100];

    if (lexer->current_char == '.') lexer_error(lexer, ". (dot) before number");

    while (lexer->current_char != EOF && (isDigit(lexer->current_char) ||
        lexer->current_char == '.'))
    {
        if (lexer->current_char == '.' && ++numberOfDots > 1)
            lexer_error(lexer, "two . (dots) in a number");

        doubleStr[offset++] = lexer->current_char;
        lexer_advance(lexer);
    }

    doubleStr[offset] = '\0';

    return atof(doubleStr);
}

Token *lexer_next_token(Lexer* lexer)
{
    while (lexer->current_char != EOF)
    {
        if (isWhitespace(lexer->current_char))
        {
            lexer_skip_spaces(lexer);
            continue;
        }

        if (isDigit(lexer->current_char))
            return token_new(NUMBER, lexer_get_double(lexer));

        if (lexer->current_char == '(')
        {
            lexer_advance(lexer);
            return token_new(LPAREN, '(');
        }

        if (lexer->current_char == ')')
        {
            lexer_advance(lexer);
            return token_new(RPAREN, ')');
        }

        if (lexer->current_char == '+')
        {
            lexer_advance(lexer);
            return token_new(PLUS, '+');
        }

        if (lexer->current_char == '-')
        {
            lexer_advance(lexer);
            return token_new(MINUS, '-');
        }

        if (lexer->current_char == '*')
        {
            lexer_advance(lexer);
            return token_new(MUL, '*');
        }

        if (lexer->current_char == '/')
        {
            lexer_advance(lexer);
            return token_new(DIV, '/');
        }

        lexer_error(lexer, "undefined token");
    }

    return token_new(EOFTOKEN, EOF);
}

// -------------------- Class Interpreter

typedef struct Interpreter Interpreter;

struct Interpreter
{
    Lexer* lexer;
    Token* current_token;
};

void interpreter_eat(Interpreter* interpreter, const char* token_type);
double interpreter_factor(Interpreter *interpreter);
double interpreter_term(Interpreter* interpreter);
double interpreter_expr(Interpreter* interpreter);

Interpreter* interpreter_new(Lexer* lexer)
{
    Interpreter* interpreter = (Interpreter*) malloc(sizeof(Interpreter));

    interpreter->lexer = lexer;
    interpreter->current_token = lexer_next_token(lexer);

    return interpreter;
}

/**
 * Consumes the current token and goes to the next.
 * 
 * @param interpreter Expression interpreter.
 * @param token_type Expected token type.
 */
void interpreter_eat(Interpreter* interpreter, const char* token_type)
{
    if (equals(interpreter->current_token->type, token_type))
    {
        free(interpreter->current_token);
        interpreter->current_token = lexer_next_token(interpreter->lexer);
    }

    else if (equals(interpreter->current_token->type, EOFTOKEN))
        lexer_error(interpreter->lexer, "End-of-file");

    else lexer_error(interpreter->lexer, "invalid token type");
}

/**
 * Try to get a number from where the interpreter is.
 * 
 * @param interpreter Expression interpreter.
 * 
 * @return double Search's result (the number).
 */
double interpreter_factor(Interpreter *interpreter)
{
    // factor : NUMBER | LPAREN expr RPAREN
    double value = EOF;

    if (equals(interpreter->current_token->type, NUMBER))
    {
        value = interpreter->current_token->value;
        interpreter_eat(interpreter, NUMBER);
    }

    else if (equals(interpreter->current_token->type, LPAREN))
    {
        interpreter_eat(interpreter, LPAREN);
        value = interpreter_expr(interpreter);
        interpreter_eat(interpreter, RPAREN);
    }

    else lexer_error(interpreter->lexer, "invalid token");

    return value;
}

/**
 * Try to search for an expression like:
 * 
 * number or
 * number * number or
 * number / number
 * 
 * @param interpreter Expression interpreter.
 * 
 * @return double Search's result (the number).
 */
double interpreter_term(Interpreter* interpreter)
{
    // term : factor ((MUL | DIV) factor)*
    double result = interpreter_factor(interpreter);
    Token* token;

    while (equals(interpreter->current_token->type, MUL) ||
        equals(interpreter->current_token->type, DIV))
    {
        token = interpreter->current_token;

        if (equals(token->type, MUL))
        {
            interpreter_eat(interpreter, MUL);
            result = result * interpreter_factor(interpreter);
        }

        if (equals(token->type, DIV))
        {
            interpreter_eat(interpreter, DIV);
            result = result / interpreter_factor(interpreter);
        }
    }

    return result;
}

double interpreter_expr(Interpreter* interpreter)
{
    /*
    expr   : term ((PLUS | MINUS) term)*
    term   : factor ((MUL | DIV) factor)*
    factor : NUMBER | LPAREN expr RPAREN
    */

    double result = interpreter_term(interpreter);
    Token* token;

    while (equals(interpreter->current_token->type, PLUS) ||
        equals(interpreter->current_token->type, MINUS))
    {
        token = interpreter->current_token;

        if (equals(token->type, PLUS))
        {
            interpreter_eat(interpreter, PLUS);
            result = result + interpreter_term(interpreter);
        }

        if (equals(token->type, MINUS))
        {
            interpreter_eat(interpreter, MINUS);
            result = result - interpreter_term(interpreter);
        }
    }
    
    return result;
}

double interpreter_run(Interpreter* interpreter)
{
    double result = interpreter_expr(interpreter);

    free(interpreter->current_token);

    return result;
}
