#include "CUnit/CUnitCI.h"
#include "lexer.h"

char *TEST_SCRIPT = "\
description:\
	:desc_amount = :in_amount;\
	string name ;\
	switch :in_stats {\
		\"money\" >\
			:name = \"Money\";\
		default >\
			:name = \"<Unkno\\\"w;n>\";\
	}\
\
	:desc_stats_name = :name;\
  end\
";

char *TEST_SCRIPT_WITHOUT_SWITCH = "\
description:\
	:desc_amount = :in_amount;\
	string name ;\
\
	:desc_stats_name = :name;\
  end\
";

TokenType EXPECTED_TOKEN[] = {T_Label,    T_Variable, T_Eq,       T_Variable, T_EndExpr,   T_StringVar,
                              T_Literal,  T_EndExpr,  T_Switch,   T_Variable, T_LeftParen, T_String,
                              T_Gt,       T_Variable, T_Eq,       T_String,   T_EndExpr,   T_Default,
                              T_Gt,       T_Variable, T_Eq,       T_String,   T_EndExpr,   T_RightParen,
                              T_Variable, T_Eq,       T_Variable, T_EndExpr,  T_End,       T_EOF};

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

CUNIT_CI_RUN("lexer", CUNIT_CI_TEST(test_lexerHappyPath));
