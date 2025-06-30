#ifndef PARSER_H
#define PARSER_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "lexer.h"

typedef enum NodeType
{
    N_LABEL_STATEMENT,
    N_LABEL_END,

    N_SWITCH_STATEMENT,
    N_SWITCH_END,

    N_ASSIGNMENT_EXPRESSION,
    N_BINARY,

    N_VARIABLE,
    N_NUMBER,
    N_STRING,
    N_BOOL,

    N_OPERATOR,

    N_EOF,
} NodeType;

int tokenIndex = 0;
Token *tokens = NULL;
S_Script script;

typedef struct Node
{
    enum NodeType type;
    struct Node *blocks;
    union {
        bool boolValue;
        int numberValue;
        char *stringValue;
    };
} Node;

Node *createNode(NodeType type)
{
    Node *node = malloc(sizeof(Node *));
    node->type = type;
    node->blocks = NULL;

    return node;
}

Node *createBinaryNode(Node leftNode, Node operatorNode, Node rightNode)
{
    Node *node = createNode(N_BINARY);
    arrput(node->blocks, leftNode); // left
    arrput(node->blocks, operatorNode);
    arrput(node->blocks, rightNode); // right
    return node;
}

void freeNode(Node *node)
{
    if (node->stringValue != NULL)
    {
        free(node->stringValue);
    }

    if (node->blocks != NULL)
    {
        for (int i = 0; i < arrlen(node->blocks); i++)
        {
            Node innerNode = node->blocks[i];
            freeNode(&innerNode);
        }
        arrfree(node->blocks);
    }
}

bool expectToken(int length, ...)
{
    va_list expectedTokenTypes;
    va_start(expectedTokenTypes, length);
    Token token = tokens[tokenIndex];

    bool found = false;
    for (int i = 0; i < length; i++)
    {
        TokenType type = (TokenType)va_arg(expectedTokenTypes, TokenType);
        if (token.type == type)
        {
            found = true;
            break;
        }
    }

    va_end(expectedTokenTypes);
    return found;
}

char *getTokenValue(Token *token)
{
    char *value = (char *)malloc(token->length + 1);
    *(value + token->length) = '\0';

    strncpy(value, *script.code + token->start, token->length);
    return value;
}

char *memString(const char *value)
{
    int len = strlen(value);
    char *newValue = (char *)malloc(len + 1);
    *(newValue + len) = '\0';

    strncpy(newValue, value, len);
    return newValue;
}

bool expectNextToken(int length, ...)
{
    va_list expectedTokenTypes;
    va_start(expectedTokenTypes, length);
    Token token = tokens[tokenIndex + 1];

    bool found = false;
    for (int i = 0; i < length; i++)
    {
        TokenType type = (TokenType)va_arg(expectedTokenTypes, TokenType);
        if (token.type == type)
        {
            found = true;
            break;
        }
    }

    va_end(expectedTokenTypes);
    return found;
}

Token getToken()
{
    tokenIndex++;
    if (tokenIndex >= arrlen(tokens))
    {
        printf("tokenIndex >= arrlen(tokens)\n");
        exit(EXIT_FAILURE);
    }
    return tokens[tokenIndex];
}

Node *parsePrimary()
{
    if (expectToken(3, T_Variable, T_Number, T_String))
    {
        Token token = getToken();
        Node *primaryNode = createNode(token.type == T_Variable ? N_VARIABLE
                                       : token.type == T_String ? N_STRING
                                                                : N_NUMBER);
        if (token.type == T_Variable || token.type == T_String)
        {
            primaryNode->stringValue = getTokenValue(&token) + 1;
        }
        else
        {
            primaryNode->numberValue = atoi(getTokenValue(&token));
        }

        return primaryNode;
    }
    else if (expectToken(2, T_True, T_False))
    {
        Token token = getToken();
        Node *primaryNode = createNode(N_BOOL);
        primaryNode->boolValue = token.type == T_True;

        return primaryNode;
    }

    printf("parsePrimary: expect T_Variable, T_Number, T_String, T_True, T_False\n");
    exit(EXIT_FAILURE);
}

Node *parseFactor()
{
    Node *expr = parsePrimary();
    while (expectToken(2, T_Slash, T_Asterisk))
    {
        Token operatorToken = getToken();
        Node *operatorNode = createNode(N_OPERATOR);
        operatorNode->stringValue = operatorToken.type == T_Slash ? memString("/") : memString("*");

        Node *binaryNode = createBinaryNode(*expr, *operatorNode, *parsePrimary());
        expr = binaryNode;
    }

    return expr;
}

Node *parseTerm()
{
    Node *expr = parseFactor();
    while (expectToken(2, T_Minus, T_Plus))
    {
        Token operatorToken = getToken();
        Node *operatorNode = createNode(N_OPERATOR);
        operatorNode->stringValue = operatorToken.type == T_Minus ? memString("-") : memString("+");

        Node *binaryNode = createBinaryNode(*expr, *operatorNode, *parseFactor());
        expr = binaryNode;
    }

    return expr;
}

Node *parseComparison()
{
    Node *expr = parseTerm();
    while (expectToken(2, T_Gt, T_Lt))
    {
        Token operatorToken = getToken();
        Node *operatorNode = createNode(N_OPERATOR);
        if (expectToken(1, T_Eq))
        {
            getToken(); // burn T_Eg token
            operatorNode->stringValue = operatorToken.type == T_Gt ? memString(">=") : memString("<=");
        }
        else
        {
            operatorNode->stringValue = operatorToken.type == T_Gt ? memString(">") : memString("<");
        }

        Node *binaryNode = createBinaryNode(*expr, *operatorNode, *parseTerm());
        expr = binaryNode;
    }

    return expr;
}

Node *parseEqual()
{
    Node *expr = parseComparison();
    while (expectToken(1, T_Eq) && expectNextToken(1, T_Eq))
    {
        getToken();
        getToken();
        Node *operatorNode = createNode(N_OPERATOR);
        operatorNode->stringValue = memString("==");

        Node *binaryNode = createBinaryNode(*expr, *operatorNode, *parseComparison());
        expr = binaryNode;
    }

    return expr;
}

Node *parseAnd()
{
    Node *expr = parseEqual();
    while (expectToken(1, T_Ampersand) && expectNextToken(1, T_Ampersand))
    {
        getToken();
        getToken();
        Node *operatorNode = createNode(N_OPERATOR);
        operatorNode->stringValue = memString("&&");

        Node *binaryNode = createBinaryNode(*expr, *operatorNode, *parseEqual());
        expr = binaryNode;
    }

    return expr;
}

Node *parseOr()
{
    Node *expr = parseAnd();
    while (expectToken(1, T_VerBar) && expectNextToken(1, T_VerBar))
    {
        getToken();
        getToken();
        Node *operatorNode = createNode(N_OPERATOR);
        operatorNode->stringValue = memString("||");

        Node *binaryNode = createBinaryNode(*expr, *operatorNode, *parseAnd());
        expr = binaryNode;
    }

    return expr;
}

Node *parseVariableExpr()
{
    if (!expectNextToken(1, T_Literal))
    {
        printf("token: T_Literal is required for variable expression\n");
        exit(EXIT_FAILURE);
    }

    Token variableToken = getToken();
    Token literalToken = getToken();
    char *variableName = getTokenValue(&literalToken);

    Node *varNode = NULL;
    switch (variableToken.type)
    {
    case T_BoolVar:
        varNode = createNode(N_BOOL);
        break;
    case T_StringVar:
        varNode = createNode(N_STRING);
        break;
    case T_NumberVar:
        varNode = createNode(N_NUMBER);
        break;
    default:
        return NULL;
    }

    Token nextToken = getToken();
    switch (nextToken.type)
    {
    case T_EndExpr: {
        break;
    }
    case T_Eq: {
        Node *exprNode = parseOr();
        // TODO: find T_EndExpr;
        arrput(varNode->blocks, *exprNode);
        break;
    }
    default:
        printf("token: T_Literal is required for variable expression\n");
        exit(EXIT_FAILURE);
    }

    return varNode;
}

Node *parseLabelBlockStatement()
{
    Node *statements = createNode(N_LABEL_STATEMENT);
    for (;;)
    {
        if (expectToken(1, T_Variable))
        {
            // variable assignment expression
        }
        else if (expectToken(3, T_BoolVar, T_StringVar, T_NumberVar))
        {
            // define variable expression
            arrput(statements->blocks, *parseVariableExpr());
        }
        else if (expectToken(1, T_Switch))
        {
        }
        else
        {
            break;
        }
    }

    return statements;
}

Node *parse()
{
    Node *nodes = NULL;

    if (expectToken(T_Label))
    {
        Node *label = parseLabelBlockStatement();
        if (!expectToken(1, T_End))
        {
            freeNode(label);
            if (nodes != NULL)
            {
                for (int i = 0; i < arrlen(nodes); i++)
                {
                    freeNode(&nodes[i]);
                }
            }
            printf("WRONG TOKEN: expect T_End of label\n");
            exit(EXIT_FAILURE);
        }
        arrput(nodes, *label);
    }
    else if (expectToken(1, T_EOF))
    {
        return nodes;
    }
    else
    {
        printf("WRONG TOKEN: expect T_Label | T_EOF\n");
        exit(EXIT_FAILURE);
    }
}

#endif
