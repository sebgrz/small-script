#include "common_test.h"

static void test_parserSimpleVariable1Expression()
{
    S_Script script = {.code = &TEST_SIMPLE_VARIABLE_EXPRESSION_1};
    char *error_msg = NULL;
    Token *tokens = lexer(&script, &error_msg);

    init(script, tokens);
    Node *varExpr = parseVariableExpr();

    char *nodeOutput = "";
    printNodeToPtr(&nodeOutput, varExpr, 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(varExpr);
    CU_ASSERT_STRING_EQUAL(nodeOutput, EXPECTED_SIMPLE_VARIABLE_EXPRESSION_RESULT_1);
}

static void test_parserSimpleVariable2Expression()
{
    S_Script script = {.code = &TEST_SIMPLE_VARIABLE_EXPRESSION_2};
    char *error_msg = NULL;
    Token *tokens = lexer(&script, &error_msg);

    init(script, tokens);
    Node *varExpr = parseVariableExpr();

    char *nodeOutput = "";
    printNodeToPtr(&nodeOutput, varExpr, 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(varExpr);
    CU_ASSERT_STRING_EQUAL(nodeOutput, EXPECTED_SIMPLE_VARIABLE_EXPRESSION_RESULT_2);
}

CUNIT_CI_RUN("parser", CUNIT_CI_TEST(test_parserSimpleVariable1Expression),
             CUNIT_CI_TEST(test_parserSimpleVariable2Expression));
