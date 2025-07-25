#ifndef PARSER_EXPR_H
#define PARSER_EXPR_H

#include "parser_common.h"

Node *parsePrimary()
{
    if (expectToken(3, T_Variable, T_Number, T_String))
    {
        Token token = getToken();
        Node *primaryNode = createNode(token.type == T_Variable ? N_VARIABLE
                                       : token.type == T_String ? N_VALUE_STRING
                                                                : N_VALUE_NUMBER);
        if (token.type == T_Variable || token.type == T_String)
        {
            primaryNode->stringValue = getTokenValue(&token);
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
        Node *primaryNode = createNode(N_VALUE_BOOL);
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

Node *parseExpression()
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

#endif
