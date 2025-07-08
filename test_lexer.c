#include "common_test.h"

TokenType EXPECTED_TOKEN[] = {T_Label,    T_Variable, T_Eq,       T_Variable, T_EndExpr,   T_StringVar,
                              T_Literal,  T_EndExpr,  T_Switch,   T_Variable, T_LeftParen, T_String,
                              T_Gt,       T_Variable, T_Eq,       T_String,   T_EndExpr,   T_Default,
                              T_Gt,       T_Variable, T_Eq,       T_String,   T_EndExpr,   T_RightParen,
                              T_Variable, T_Eq,       T_Variable, T_EndExpr,  T_End,       T_EOF};

TokenType EXPECTED_SIMPLE_VARIABLE_EXPRESSION_TOKEN[] = {T_NumberVar, T_Literal,  T_Eq,     T_Number,  T_Plus,
                                                         T_Variable,  T_Asterisk, T_Number, T_EndExpr, T_EOF};

static void test_lexerHappyPath()
{
    S_Script script = {.code = &TEST_SCRIPT};
    char *error_msg = NULL;
    Token *tokens = lexer(&script, &error_msg);

    CU_ASSERT_PTR_NULL_FATAL(error_msg);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tokens);
    for (int i; i < (sizeof(EXPECTED_TOKEN) / sizeof(TokenType)); i++)
    {
        CU_ASSERT_EQUAL_FATAL(tokens[i].type, EXPECTED_TOKEN[i]);
    }
    CU_ASSERT_EQUAL_FATAL(arrlen(tokens), 30);
}

static void test_lexerSimpleVariableExpression()
{
    S_Script script = {.code = &TEST_SIMPLE_VARIABLE_EXPRESSION};
    char *error_msg = NULL;
    Token *tokens = lexer(&script, &error_msg);

    CU_ASSERT_PTR_NULL_FATAL(error_msg);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tokens);
    for (int i; i < (sizeof(EXPECTED_SIMPLE_VARIABLE_EXPRESSION_TOKEN) / sizeof(TokenType)); i++)
    {
        CU_ASSERT_EQUAL_FATAL(tokens[i].type, EXPECTED_SIMPLE_VARIABLE_EXPRESSION_TOKEN[i]);
    }
    CU_ASSERT_EQUAL_FATAL(arrlen(tokens), 10);
}

CUNIT_CI_RUN("lexer", CUNIT_CI_TEST(test_lexerHappyPath), CUNIT_CI_TEST(test_lexerSimpleVariableExpression));
