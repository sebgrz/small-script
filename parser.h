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

Token getToken()
{

    tokenIndex++;
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
        arrput(nodes, label);
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
