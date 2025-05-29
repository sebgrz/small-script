#ifndef LEXER_H
#define LEXER_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

typedef struct S_Script
{
    char **inputVars;
    char **outputVars;
    char **output; // ["out_value", "123", "out_name", "lorem"]
    char **code;
} S_Script;

typedef enum TokenType
{
    T_None,

    T_Switch,     // switch
    T_Default,    // default
    T_EndExpr,    // ;
    T_LeftParen,  // {
    T_RightParen, // }
    T_Eq,         // =
    T_Gt,         // >
    T_Lt,         // <
    T_Plus,       // +
    T_Minus,      // -
    T_Asterisk,   // *
    T_Slash,      // /
    T_Ampersand,  // &
    T_VerBar,     // |

    T_Label,    // label:
    T_Variable, // :var
    T_End,      // end
    T_True,     // true
    T_False,    // false
    T_Literal,

    T_BoolVar,   // bool
    T_StringVar, // string
    T_NumberVar, // number

    T_Number, // 123
    T_String, // "lorem"

    T_EOF,
} TokenType;

typedef struct Token
{
    TokenType type;
    int start, length;
} Token;

Token *lexer(S_Script *script, char **errorMsg)
{
    char **source = script->code;
    Token *tokens = NULL;

    int index = 0;
    char ch;
    Token token = {};
    for (;;)
    {
        ch = *(*source + index);

        // end of file
        if (ch == '\0')
        {
            token = (Token){
                .type = T_EOF,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
            break;
        }
        else if (ch == ':') // variable
        {
            token = (Token){
                .type = T_Variable,
                .start = index + 1, // omit :
                .length = 0,
            };

            ch = *(*source + index + 1);
            while ((ch >= 'a' && ch <= 'z') || ch == '_' || (ch >= '0' && ch <= '9'))
            {
                index++;
                token.length = index - token.start + 1;
                ch = *(*source + index + 1);
            }

            arrput(tokens, token);
        }
        else if (ch == '=') // equal
        {
            token = (Token){
                .type = T_Eq,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '>') // >
        {
            token = (Token){
                .type = T_Gt,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '<') // >
        {
            token = (Token){
                .type = T_Lt,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '+') // +
        {
            token = (Token){
                .type = T_Plus,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '-') // -
        {
            token = (Token){
                .type = T_Minus,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '&') // &
        {
            token = (Token){
                .type = T_Ampersand,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '|') // |
        {
            token = (Token){
                .type = T_VerBar,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '*') // *
        {
            token = (Token){
                .type = T_Asterisk,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '/') // /
        {
            token = (Token){
                .type = T_Slash,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == ';') // {
        {
            token = (Token){
                .type = T_EndExpr,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '{') // {
        {
            token = (Token){
                .type = T_LeftParen,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '}') // }
        {
            token = (Token){
                .type = T_RightParen,
                .start = index,
                .length = 0,
            };
            arrput(tokens, token);
        }
        else if (ch == '"') // start string
        {
            token = (Token){
                .type = T_String,
                .start = index + 1, // omit "
                .length = 0,
            };

            ch = *(*source + index + 1);
            while (ch != '"')
            {
                index++;
                token.length = index - token.start + 1;
                ch = *(*source + index + 1);

                // check \" -> continue
                if (ch == '"' && *(*source + index) == '\\')
                {
                    index++;
                    token.length = index - token.start + 1;
                    ch = *(*source + index + 1);
                }
            }
            index++;

            arrput(tokens, token);
        }
        else if ((ch >= 'a' && ch <= 'z')) // label: or switch or types - bool, number, string
        {
            token = (Token){
                .type = T_Literal,
                .start = index,
                .length = 0,
            };
            ch = *(*source + index + 1);
            bool isLabel = false;
            while ((ch >= 'a' && ch <= 'z') || ch == '_' || (ch >= '0' && ch <= '9'))
            {
                index++;
                token.length = index - token.start + 1;
                ch = *(*source + index + 1);
                if (ch == ':') // end label
                {
                    index++;
                    isLabel = true;
                    break;
                }
            }

            char *tokenCheck = malloc(token.length + 1);
            *(tokenCheck + token.length) = '\0';
            strncpy(tokenCheck, *source + token.start, token.length);
            if (isLabel)
            {
                token.type = T_Label;
            }
            else if (strcmp(tokenCheck, "switch") == 0)
            {
                token.type = T_Switch;
            }
            else if (strcmp(tokenCheck, "default") == 0)
            {
                token.type = T_Default;
            }
            else if (strcmp(tokenCheck, "string") == 0)
            {
                token.type = T_StringVar;
            }
            else if (strcmp(tokenCheck, "number") == 0)
            {
                token.type = T_NumberVar;
            }
            else if (strcmp(tokenCheck, "bool") == 0)
            {
                token.type = T_BoolVar;
            }
            else if (strcmp(tokenCheck, "end") == 0)
            {
                token.type = T_End;
            }
            else if (strcmp(tokenCheck, "true") == 0)
            {
                token.type = T_True;
            }
            else if (strcmp(tokenCheck, "false") == 0)
            {
                token.type = T_False;
            }
            else
            {
                token.type = T_Literal;
            }
            free(tokenCheck);

            arrput(tokens, token);
        }
        else if ((ch >= '0' && ch <= '9')) // 123
        {
            token = (Token){
                .type = T_Number,
                .start = index,
                .length = 0,
            };
            ch = *(*source + index + 1);
            while ((ch >= '0' && ch <= '9'))
            {
                index++;
                token.length = index - token.start + 1;
                ch = *(*source + index + 1);
            }

            arrput(tokens, token);
        }

        index++;
    }

    return tokens;
}

#endif
