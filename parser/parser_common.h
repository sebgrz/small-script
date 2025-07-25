#ifndef PARSER_COMMON_H
#define PARSER_COMMON_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include "../lexer.h"

typedef enum NodeType
{
    N_LABEL_STATEMENT,
    N_LABEL_END,

    N_SWITCH_STATEMENT,
    N_SWITCH_END,

    N_BINARY,

    N_VALUE_NUMBER,
    N_VALUE_STRING,
    N_VALUE_BOOL,

    N_VARIABLE,
    N_NUMBER_VAR,
    N_STRING_VAR,
    N_BOOL_VAR,

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

Node *parseExpression();

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
    Token token = tokens[tokenIndex];
    tokenIndex++;

    return token;
}


Node *parseSwitchStatement()
{
    if (!expectNextToken(1, T_Variable))
    {
        printf("token: T_Variable is required for switch statement\n");
        exit(EXIT_FAILURE);
    }
    Token switchLiteralToken = getToken();
    Token variableToken = getToken();
    char *variableName = getTokenValue(&variableToken);

    if (!expectToken(1, T_LeftParen))
    {
        printf("token: T_LeftParen is required for switch statement body\n");
        exit(EXIT_FAILURE);
    }
    // consume T_LeftParen
    getToken();

    // start to parse switch body

    if (!expectToken(1, T_RightParen))
    {
        printf("token: T_RightParen is required for switch statement end body\n");
        exit(EXIT_FAILURE);
    }
    // consume T_RightParen
    getToken();
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
        varNode = createNode(N_BOOL_VAR);
        break;
    case T_StringVar:
        varNode = createNode(N_STRING_VAR);
        break;
    case T_NumberVar:
        varNode = createNode(N_NUMBER_VAR);
        break;
    default:
        return NULL;
    }
    varNode->stringValue = variableName;

    Token nextToken = getToken();
    switch (nextToken.type)
    {
    case T_EndExpr: {
        break;
    }
    case T_Eq: {
        Node *exprNode = parseExpression();
        arrput(varNode->blocks, *exprNode);
        break;
    }
    default:
        printf("token: T_Literal is required for variable expression\n");
        exit(EXIT_FAILURE);
    }

    return varNode;
}

void parseBlock(Node *statements) {
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
            arrput(statements->blocks, *parseSwitchStatement());
        }
        else
        {
            break;
        }
    }
}

#endif
