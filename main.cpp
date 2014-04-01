#include <string>
#include <vector>
#include <cstring>

#include "mjolnir.hh"
#include "options.hh"
#include "tree.hh"
#include "parser.hh"
#include "pass.hh"
#include "variable.hh"

#define	TEST(file, expected, fail_p)				\
  run_test(opt, file, sizeof(file) - 1, expected, fail_p);


enum Expected_result
  {
    SUCCESS,
    FAIL,
  };



static	unsigned long long	nb_test = 0;
static	unsigned long long	failed_test = 0;


static void	run_test(Options &option, const char *file,
			 const size_t len_file, const char *expected,
			 const Expected_result fail_p);
static void	do_tests(void);


static void	test_parse_empty(void);
static void	test_parse_function_prototypes(void);
static void	test_parse_variable_declaration(void);
static void	test_parse_empty_function(void);
static void	test_parse_var_in_scopes(void);
static void	test_parse_additions(void);
static void	test_parse_soustractions(void);
static void	test_parse_multiplications(void);
static void	test_parse_divisions(void);
static void	test_parse_comments(void);



static void	run_test(Options &option, const char *file,
			 const size_t len_file, const char *expected,
			 const Expected_result fail_p)
{
  File	to_test("<test>", file, len_file);
  Scope	*res = pass_manager(to_test, option);
  const std::string stream_res(option.error_stream += option.dump_stream);

  if (((res && fail_p == SUCCESS) || (!res && fail_p == FAIL)) &&
      option.error_stream == expected)
    {}
  else
    {
      if (!failed_test)
	{
	  __builtin_printf("/*******************************************/\n");
	}
      __builtin_printf("%s\n", file);
      if (!(res && fail_p == SUCCESS) || (!res && fail_p == FAIL))
	{
	  __builtin_printf("unexpected returned value\n");
	}
      if (!stream_res.empty() && stream_res.back() != '\n')
	__builtin_printf("WARNING wrong format for message\n");
      __builtin_printf("got\t: %s%c", stream_res.c_str(),
		       stream_res.back() != '\n' ? '\n' : '\0');
      if (expected[0] && expected[strlen(expected) - 1] != '\n')
	__builtin_printf("WARNING wrong format for expected output\n");
      __builtin_printf("expected: %s%c", expected, !expected[0] ||
		       expected[strlen(expected) - 1] != '\n' ? '\n' : '\0');
      __builtin_printf("/*******************************************/\n");
      failed_test++;
    }
  if (res)
    {
      clear_scope(res);
      delete res;
    }
  nb_test++;
  option.error_stream.clear();
  option.dump_stream.clear();
}

static void	test_parse_empty(void)
{
  Options	opt;

  opt.dump_parse_p = true;
  TEST("", "", SUCCESS);
  TEST(";", "", SUCCESS);
  TEST(";;;;;;;;;;;;;;;", "", SUCCESS);
  TEST("\n \t\t\t     \n   \t\t", "", SUCCESS);
}

static void	test_parse_variable_declaration(void)
{
  Options	opt;

  opt.dump_parse_p = true;
  TEST("int", "expected identifier or '('\n", FAIL);
  TEST("int;int", "expected identifier or '('\n", FAIL);
  TEST("int, int", "expected identifier or '('\n", FAIL);
  TEST("int y", "expected ';' or '('\n", FAIL);


  TEST("int;", "i32;\n", SUCCESS);
  TEST("int y;", "i32	y;\n", SUCCESS);
  TEST("int;int;", "i32;\ni32;\n", SUCCESS);
  TEST("int;void;", "i32;\nvoid;\n", SUCCESS);
}

static void	test_parse_function_prototypes(void)
{
  Options	opt;

  opt.dump_parse_p = true;
  TEST("int f()", "expected ';' or '{'\n", FAIL);
  TEST("int f() qwertyui", "expected ';' or '{'\n", FAIL);
  TEST("int()", "expected identifier or '('\n", FAIL);
  TEST("int f(void int);", "expected identifier or ','\n", FAIL);
  TEST("int f(,);", "expected params or ')'\n", FAIL);

  TEST("int f();",		"i32	f();\n", SUCCESS);
  TEST("int f(void);",		"i32	f(void);\n", SUCCESS);
  TEST("int f(void, int);",	"i32	f(void, i32);\n", SUCCESS);
  TEST("int f(int a, int b, int c);", "i32	f(i32 a, i32 b, i32 c);\n",
       SUCCESS);
}

static void	test_parse_empty_function(void)
{
  Options	opt;

  opt.dump_parse_p = true;
  TEST("int f(){}", "i32	f()\n{\n}\n", SUCCESS);
  TEST("int f(int){}", "i32	f(i32)\n{\n}\n", SUCCESS);
  TEST("int f(int i){}", "i32	f(i32 i)\n{\n}\n", SUCCESS);


  TEST("int f()}", "expected ';' or '{'\n", FAIL);
  TEST("int f(){", "missing '}'\n", FAIL);
  TEST("int f(){}}", "too many '}'\n", FAIL);
  TEST("int f()q", "expected ';' or '{'\n", FAIL);

}

static void	test_parse_empty_scopes(void)
{
  Options	opt;

  opt.dump_parse_p = true;
  TEST("{}", "expected identifier\n", FAIL);
  TEST("int f(){{}{}{}{}{}{}{}", "missing '}'\n", FAIL);
  TEST("int f(){{}{}{}{}{}{}}}", "too many '}'\n", FAIL);

  TEST("int f(){{}}", "i32	f()\n{\n{\n}\n}\n", SUCCESS);
  TEST("int f(){{}{}}", "i32	f()\n{\n{\n}\n{\n}\n}\n", SUCCESS);
  TEST("int f(){{{}}}", "i32	f()\n{\n{\n{\n}\n}\n}\n", SUCCESS);
}

static void	test_parse_var_in_scopes(void)
{
  Options	opt;

  opt.dump_parse_p = true;
  TEST("int f(){int i;{int j;}}", "i32	f()\n"
       "{\ni32	i;\n{\ni32	j;\n}\n}\n", SUCCESS);
}


static void	test_parse_additions(void)
{
  Options	opt;

  opt.dump_parse_p = true;

  TEST("+", "expected declaration or identifier\n", FAIL);
  TEST("int i; i +", "expected identifier or constant\n", FAIL);

  TEST("int i; i;", "i32	i;\n\ni;\n", SUCCESS);
  TEST("int i; int j; i + j;", "i32	i;\ni32	j;\n"
       "i32	.0;\n\n.0 = i + j;\n", SUCCESS);
  TEST("int i; int j; int k; i + j + k;", "i32	i;\ni32	j;\ni32	k;\n"
       "i32	.0;\ni32	.1;\n\n.0 = i + j;\n.1 = .0 + k;\n", SUCCESS);
  TEST("int i; int j; int k; int l;i + j + k +l;", "i32	i;\ni32	j;\ni32	k;\n"
       "i32	l;\ni32	.0;\ni32	.1;\ni32	.2;\n\n.0 = i + j;\n"
       ".1 = .0 + k;\n.2 = .1 + l;\n", SUCCESS);
}

static void	test_parse_soustractions(void)
{
  Options	opt;

  opt.dump_parse_p = true;

  TEST("-", "expected declaration or identifier\n", FAIL);
  TEST("int i; i", "expected operator or ';'\n", FAIL);
  TEST("int i; i -", "expected identifier or constant\n", FAIL);

  TEST("int i; int j; i - j;", "i32	i;\ni32	j;\n"
       "i32	.0;\n\n.0 = i - j;\n", SUCCESS);
  TEST("int i; int j; int k; int l;i - j - k - l;", "i32	i;"
       "\ni32	j;\ni32	k;\ni32	l;\ni32	.0;\ni32	.1;\ni32	.2;\n\n"
       ".0 = i - j;\n.1 = .0 - k;\n.2 = .1 - l;\n", SUCCESS);
}

static void	test_parse_multiplications(void)
{
  Options	opt;

  opt.dump_parse_p = true;

  TEST("*", "expected declaration or identifier\n", FAIL);
  TEST("int i; i *", "expected identifier or constant\n", FAIL);

  TEST("int i; int j; i * j;", "i32	i;\ni32	j;\ni32	.0;\n\n.0 = i * j;\n",
       SUCCESS);
  TEST("int i; int j; int k; int l;i * j * k * l;",
       "i32	i;\ni32	j;\ni32	k;\ni32	l;\ni32	.0;\ni32	.1;\n"
       "i32	.2;\n\n.0 = i * j;\n.1 = .0 * k;\n.2 = .1 * l;\n", SUCCESS);
}

static void	test_parse_divisions(void)
{
  Options	opt;

  opt.dump_parse_p = true;

  TEST("/", "expected declaration or identifier\n", FAIL);
  TEST("int i; i /", "expected identifier or constant\n", FAIL);

  TEST("int i; int j; i / j;", "i32	i;\ni32	j;\ni32	.0;\n\n.0 = i / j;\n",
       SUCCESS);
  TEST("int i; int j; int k; int l;i / j / k / l;",
       "i32	i;\ni32	j;\ni32	k;\ni32	l;\ni32	.0;\ni32	.1;\n"
       "i32	.2;\n\n.0 = i / j;\n.1 = .0 / k;\n.2 = .1 / l;\n", SUCCESS);
}

static void	test_parse_comments(void)
{
  Options	opt;

  opt.dump_parse_p = true;
  opt.c_dialect = C_dialect::C_89;

  TEST("/**/", "", SUCCESS);
  TEST("/*/", "unfinished c comment\n", FAIL);
  TEST("/*", "unfinished c comment\n", FAIL);
  TEST("/*int	i;*/", "", SUCCESS);
  TEST("// int	i;\n", "expected declaration or identifier\n", FAIL);


  opt.c_dialect = C_dialect::C_99;
  TEST("/**/", "", SUCCESS);
  TEST("/*/", "unfinished c comment\n", FAIL);
  TEST("/*", "unfinished c comment\n", FAIL);
  TEST("/*int	i;*/", "", SUCCESS);
  TEST("// int	i;\n", "", SUCCESS);
  TEST("/", "expected declaration or identifier\n", FAIL);
  TEST("// int	i;", "c++ style comment should be finished by a new-line\n",
       FAIL);
  TEST("//", "c++ style comment should be finished by a new-line\n", FAIL);
}

static void	test_parse_operator_priority(void)
{
  Options	opt;

  opt.dump_parse_p = true;
  TEST("int i; int j; int k; i * j + k;",
       "i32	i;\ni32	j;\ni32	k;\ni32	.0;\ni32	.1;\n"
       "\n.0 = i * j;\n.1 = .0 + k;\n", SUCCESS);
  TEST("int i; int j; int k; i + j * k;",
       "i32	i;\ni32	j;\ni32	k;\ni32	.0;\ni32	.1;\n"
       "\n.0 = j * k;\n.1 = i + .0;\n", SUCCESS);

  TEST("int i; int j; int k; i / j + k;",
       "i32	i;\ni32	j;\ni32	k;\ni32	.0;\ni32	.1;\n"
       "\n.0 = i / j;\n.1 = .0 + k;\n", SUCCESS);
  TEST("int i; int j; int k; i + j / k;",
       "i32	i;\ni32	j;\ni32	k;\ni32	.0;\ni32	.1;\n"
       "\n.0 = j / k;\n.1 = i + .0;\n", SUCCESS);

  TEST("int i; int j; int k; i * j / k;",
       "i32	i;\ni32	j;\ni32	k;\ni32	.0;\ni32	.1;\n"
       "\n.0 = i * j;\n.1 = .0 / k;\n", SUCCESS);

  TEST("int i; int j; int k; int l;i * j + k / l;",
       "i32	i;\ni32	j;\ni32	k;\ni32	l;\ni32	.0;\ni32	.1;\n"
       "i32	.2;\n\n.0 = i * j;\n.1 = k / l;\n.2 = .0 + .1;\n", SUCCESS);
}

static void	do_tests(void)
{
  test_parse_empty();
  test_parse_variable_declaration();
  test_parse_function_prototypes();
  test_parse_empty_function();
  test_parse_empty_scopes();
  test_parse_var_in_scopes();
  test_parse_additions();
  test_parse_soustractions();
  test_parse_multiplications();
  test_parse_divisions();
  test_parse_comments();
  test_parse_operator_priority();
}

int	main(void)
{
  do_tests();
  __builtin_printf("%llu tests\n", nb_test);
  __builtin_printf("%llu tests passed\n", nb_test - failed_test);
  __builtin_printf("%llu tests failed\n", failed_test);
  return !!failed_test;
}
