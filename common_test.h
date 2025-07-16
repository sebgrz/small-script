#ifndef COMMON_TEST_H
#define COMMON_TEST_H

#include "CUnit/CUnitCI.h"
#include "parser.h"

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

char *TEST_SIMPLE_VARIABLE_EXPRESSION_1 = "\
number calculation = 10 + :number * 5;\
";

char *TEST_SIMPLE_VARIABLE_EXPRESSION_2 = "\
number calculation = 10 + :number * 5 - 1;\
";

char *EXPECTED_SIMPLE_VARIABLE_EXPRESSION_RESULT_1 = "NUMBER_VAR 'calculation'\n\
  BINARY:\n\
    NUMBER: '10'\n\
    OP: +\n\
    BINARY:\n\
      VAR: 'number'\n\
      OP: *\n\
      NUMBER: '0'\n\
";

char *EXPECTED_SIMPLE_VARIABLE_EXPRESSION_RESULT_2 = "NUMBER_VAR 'calculation'\n\
  BINARY:\n\
    BINARY:\n\
      NUMBER: '10'\n\
      OP: +\n\
      BINARY:\n\
        VAR: 'number'\n\
        OP: *\n\
        NUMBER: '0'\n\
    OP: -\n\
    NUMBER: '0'\n\
";

#endif
