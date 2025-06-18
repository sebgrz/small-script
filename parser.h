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

Node *parseExpr()
{
    Node *binaryNode = NULL;
    for (;;)
    {
        Token token = getToken();
        switch (token.type)
        {
        case T_Variable:
        case T_Number: {
            // TODO if binaryNode != NULL
            if (expectToken(5, T_EndExpr, T_Minus, T_Plus, T_Asteriks, T_Slash))
            {
            }
            else if (expectToken(1, T_Eq) && expectNextToken(1, T_Eq))
            {
                getToken(); // burn the second T_Eq
                binaryNode = createNode(N_BINARY);

                Node *leftNode = createNode(token.type == T_Variable ? N_VARIABLE : N_NUMBER);
                if (token.type == T_Variable)
                {
                    leftNode->stringValue = getTokenValue(&token) + 1;
                }
                else
                {
                    leftNode->numberValue = atoi(getTokenValue(&token));
                }
                Node *operatorNode = createNode(N_OPERATOR);
                operatorNode->stringValue = memString("==");

                arrput(binaryNode->blocks, *leftNode);
                arrput(binaryNode->blocks, *operatorNode);

                if (!expectNextToken(2, T_Variable, T_Number))
                {
                    token = getToken();
                    printf("parseExpr unexpected next token type: %d for '=='\n", token.type);
                    exit(EXIT_FAILURE);
                }
            }
            else
            {
                token = getToken();
                printf("parseExpr unexpected next token type: %d for T_Variable, T_Number\n", token.type);
                exit(EXIT_FAILURE);
            }

            break;
        }
        case T_String:
        default:
            printf("parseExpr unexpected token type: %d\n", token.type);
            exit(EXIT_FAILURE);
        }
    }
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
        Node *exprNode = parseExpr();
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
