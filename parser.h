#ifndef PARSER_H
#define PARSER_H

#include "parser/parser_common.h"
#include "parser/parser_expr.h"

Node *parseLabelBlockStatement()
{
    Node *statements = createNode(N_LABEL_STATEMENT);
    parseBlock(statements);
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

void init(S_Script newScript, Token *newTokens)
{
    script = newScript;
    tokens = newTokens;
    tokenIndex = 0;
}

char *printNodeToPtr(char **outPtr, Node *node, int level)
{
    asprintf(outPtr, "%s%*s", *outPtr, level * 2, "");
    switch (node->type)
    {
    case N_LABEL_STATEMENT:
        break;
    case N_LABEL_END:
        break;
    case N_SWITCH_STATEMENT:
        break;
    case N_SWITCH_END:
        break;
    case N_BINARY:
        asprintf(outPtr, "%sBINARY:\n", *outPtr);
        printNodeToPtr(outPtr, &node->blocks[0], level + 1);
        asprintf(outPtr, "%s%*s", *outPtr, (level + 1) * 2, "");
        asprintf(outPtr, "%sOP: %s\n", *outPtr, node->blocks[1].stringValue);
        printNodeToPtr(outPtr, &node->blocks[2], level + 1);
        break;
    case N_VARIABLE:
        asprintf(outPtr, "%sVAR: '%s'\n", *outPtr, node->stringValue);
        break;
    case N_STRING_VAR:
        asprintf(outPtr, "%sSTRING_VAR '%s'\n", *outPtr, node->stringValue);
        printNodeToPtr(outPtr, &node->blocks[0], level + 1);
        break;
    case N_NUMBER_VAR:
        asprintf(outPtr, "%sNUMBER_VAR '%s'\n", *outPtr, node->stringValue);
        printNodeToPtr(outPtr, &node->blocks[0], level + 1);
        break;
    case N_BOOL_VAR:
        asprintf(outPtr, "%sBOOL_VAR '%s'\n", *outPtr, node->stringValue);
        printNodeToPtr(outPtr, &node->blocks[0], level + 1);
        break;
    case N_VALUE_NUMBER:
        asprintf(outPtr, "%sNUMBER: '%d'\n", *outPtr, node->numberValue);
        break;
    case N_VALUE_STRING:
        asprintf(outPtr, "%sSTRING: %s\n", *outPtr, node->stringValue);
        break;
    case N_VALUE_BOOL:
        asprintf(outPtr, "%sBOOL %d\n", *outPtr, node->boolValue);
        break;
    case N_OPERATOR:
        break;
    case N_EOF:
        break;
    }
}

#endif
