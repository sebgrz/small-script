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

char *TEST_SIMPLE_VARIABLE_EXPRESSION = "\
number calculation = 10 + :number * 5;\
";

#endif
