#include <string>
#include <vector>

#include "mjolnir.hh"
#include "options.hh"
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


static void	run_test(Option &option, const char *file, const size_t len_file, const char *expected, const Expected_result fail_p);
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



static void	run_test(Option &option, const char *file, const size_t len_file, const char *expected, const Expected_result fail_p)
{
  File	to_test("<test>", file, len_file);
  Scope	*res = pass_manager(to_test, option);
  const std::string stream_res(option.error_stream += option.dump_stream);

  if (((res && fail_p == SUCCESS) || (!res && fail_p == FAIL)) && option.error_stream == expected)
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
      __builtin_printf("got\t: %s%c", stream_res.c_str(), /* option.error_stream.empty() || */
		       stream_res.back() != '\n' ? '\n' : '\0');
      if (expected[0] && expected[__builtin_strlen(expected) - 1] != '\n')
	__builtin_printf("WARNING wrong format for expected output\n");
      __builtin_printf("expected: %s%c", expected, !expected[0] || expected[__builtin_strlen(expected) - 1] != '\n' ? '\n' : '\0');
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
  Option	opt;

  opt.dump_parse_p = true;
  TEST("", "", SUCCESS);
  TEST(";", "", SUCCESS);
  TEST(";;;;;;;;;;;;;;;", "", SUCCESS);
  TEST("\n \t\t\t     \n   \t\t", "", SUCCESS);
}

static void	test_parse_variable_declaration(void)
{
  Option	opt;

  opt.dump_parse_p = true;
  TEST("int", "expected identifier or '('\n", FAIL);
  TEST("int;int", "expected identifier or '('\n", FAIL);
  TEST("int, int", "expected identifier or '('\n", FAIL);
  TEST("int y", "expected ';' or '('\n", FAIL);


  TEST("int;", "int;\n", SUCCESS);
  TEST("int y;", "int	y;\n", SUCCESS);
  TEST("int;int;", "int;\nint;\n", SUCCESS);
  TEST("int;void;", "int;\nvoid;\n", SUCCESS);
}

static void	test_parse_function_prototypes(void)
{
  Option	opt;

  opt.dump_parse_p = true;
  TEST("int f()", "expected ';' or '{'\n", FAIL);
  TEST("int f() qwertyui", "expected ';' or '{'\n", FAIL);
  TEST("int()", "expected identifier or '('\n", FAIL);
  TEST("int f(void int);", "expected identifier or ','\n", FAIL);
  TEST("int f(,);", "expected params or ')'\n", FAIL);

  TEST("int f();",		"int	f();\n", SUCCESS);
  TEST("int f(void);",		"int	f(void);\n", SUCCESS);
  TEST("int f(void, int);",	"int	f(void, int);\n", SUCCESS);
  TEST("int f(int a, int b, int c);", "int	f(int a, int b, int c);\n", SUCCESS);
}

static void	test_parse_empty_function(void)
{
  Option	opt;

  opt.dump_parse_p = true;
  TEST("int f(){}", "int	f()\n{\n}\n", SUCCESS);
  TEST("int f(int){}", "int	f(int)\n{\n}\n", SUCCESS);
  TEST("int f(int i){}", "int	f(int i)\n{\n}\n", SUCCESS);


  TEST("int f()}", "expected ';' or '{'\n", FAIL);
  TEST("int f(){", "missing '}'\n", FAIL);
  TEST("int f(){}}", "too many '}'\n", FAIL);
  TEST("int f()q", "expected ';' or '{'\n", FAIL);

}

static void	test_parse_empty_scopes(void)
{
  Option	opt;

  opt.dump_parse_p = true;
  TEST("{}", "expected identifier\n", FAIL);
  TEST("int f(){{}{}{}{}{}{}{}", "missing '}'\n", FAIL);
  TEST("int f(){{}{}{}{}{}{}}}", "too many '}'\n", FAIL);

  TEST("int f(){{}}", "int	f()\n{\n{\n}\n}\n", SUCCESS);
  TEST("int f(){{}{}}", "int	f()\n{\n{\n}\n{\n}\n}\n", SUCCESS);
  TEST("int f(){{{}}}", "int	f()\n{\n{\n{\n}\n}\n}\n", SUCCESS);
}

static void	test_parse_var_in_scopes(void)
{
  Option	opt;

  opt.dump_parse_p = true;
  TEST("int f(){int i;{int j;}}", "int	f()\n{\nint	i;\n{\nint	j;\n}\n}\n", SUCCESS);
}


static void	test_parse_additions(void)
{
  Option	opt;

  opt.dump_parse_p = true;

  TEST("+", "expected declaration or identifier\n", FAIL);
  TEST("int i; i +", "expected identifier or constant\n", FAIL);

  TEST("int i; i;", "int	i;\n\ni;\n", SUCCESS);
  TEST("int i; int j; i + j;", "int	i;\nint	j;\nint	.0;\n\n.0 = i + j;\n", SUCCESS);
  TEST("int i; int j; int k; i + j + k;", "int	i;\nint	j;\nint	k;\nint	.0;\nint	.1;\n\n.0 = i + j;\n.1 = .0 + k;\n", SUCCESS);
  TEST("int i; int j; int k; int l;i + j + k +l;", "int	i;\nint	j;\nint	k;\nint	l;\nint	.0;\nint	.1;\nint	.2;\n\n.0 = i + j;\n.1 = .0 + k;\n.2 = .1 + l;\n", SUCCESS);
}

static void	test_parse_soustractions(void)
{
  Option	opt;

  opt.dump_parse_p = true;

  TEST("-", "expected declaration or identifier\n", FAIL);
  TEST("int i; i", "expected operator or ';'\n", FAIL);
  TEST("int i; i -", "expected identifier or constant\n", FAIL);

  TEST("int i; int j; i - j;", "int	i;\nint	j;\nint	.0;\n\n.0 = i - j;\n", SUCCESS);
  TEST("int i; int j; int k; int l;i - j - k - l;", "int	i;\nint	j;\nint	k;\nint	l;\nint	.0;\nint	.1;\nint	.2;\n\n.0 = i - j;\n.1 = .0 - k;\n.2 = .1 - l;\n", SUCCESS);
}

static void	test_parse_multiplications(void)
{
  Option	opt;

  opt.dump_parse_p = true;

  TEST("*", "expected declaration or identifier\n", FAIL);
  TEST("int i; i *", "expected identifier or constant\n", FAIL);

  TEST("int i; int j; i * j;", "int	i;\nint	j;\nint	.0;\n\n.0 = i * j;\n", SUCCESS);
  TEST("int i; int j; int k; int l;i * j * k * l;", "int	i;\nint	j;\nint	k;\nint	l;\nint	.0;\nint	.1;\nint	.2;\n\n.0 = i * j;\n.1 = .0 * k;\n.2 = .1 * l;\n", SUCCESS);
}

static void	test_parse_divisions(void)
{
  Option	opt;

  opt.dump_parse_p = true;

  TEST("/", "expected declaration or identifier\n", FAIL);
  TEST("int i; i /", "expected identifier or constant\n", FAIL);

  TEST("int i; int j; i / j;", "int	i;\nint	j;\nint	.0;\n\n.0 = i / j;\n", SUCCESS);
  TEST("int i; int j; int k; int l;i / j / k / l;", "int	i;\nint	j;\nint	k;\nint	l;\nint	.0;\nint	.1;\nint	.2;\n\n.0 = i / j;\n.1 = .0 / k;\n.2 = .1 / l;\n", SUCCESS);
}

static void	test_parse_comments(void)
{
  Option	opt;

  opt.dump_parse_p = true;
  opt.c_dialect = C_dialect::C_89;

  TEST("/**/", "", SUCCESS);
  TEST("/*/", "unfinished c comment\n", FAIL);
  TEST("/*", "unfinished c comment\n", FAIL);
  TEST("/*int	i;*/", "", SUCCESS);
  TEST("// int	i;\n", "expected declaration or identifier\n", FAIL);
  //

  opt.c_dialect = C_dialect::C_99;
  TEST("/**/", "", SUCCESS);
  TEST("/*/", "unfinished c comment\n", FAIL);
  TEST("/*", "unfinished c comment\n", FAIL);
  TEST("/*int	i;*/", "", SUCCESS);
  TEST("// int	i;\n", "", SUCCESS);
  TEST("/", "expected declaration or identifier\n", FAIL);
  TEST("// int	i;", "c++ style comment should be finished by a new-line\n", FAIL);
  TEST("//", "c++ style comment should be finished by a new-line\n", FAIL);
}

static void	test_parse_operator_priority(void)
{
  Option	opt;

  opt.dump_parse_p = true;
  TEST("int i; int j; int k; i * j + k;", "int	i;\nint	j;\nint	k;\nint	.0;\nint	.1;\n\n.0 = i * j;\n.1 = .0 + k;\n", SUCCESS);
  TEST("int i; int j; int k; i + j * k;", "int	i;\nint	j;\nint	k;\nint	.0;\nint	.1;\n\n.0 = j * k;\n.1 = i + .0;\n", SUCCESS);

  TEST("int i; int j; int k; i / j + k;", "int	i;\nint	j;\nint	k;\nint	.0;\nint	.1;\n\n.0 = i / j;\n.1 = .0 + k;\n", SUCCESS);
  TEST("int i; int j; int k; i + j / k;", "int	i;\nint	j;\nint	k;\nint	.0;\nint	.1;\n\n.0 = j / k;\n.1 = i + .0;\n", SUCCESS);

  TEST("int i; int j; int k; i * j / k;", "int	i;\nint	j;\nint	k;\nint	.0;\nint	.1;\n\n.0 = i * j;\n.1 = .0 / k;\n", SUCCESS);

  TEST("int i; int j; int k; int l;i * j + k / l;", "int	i;\nint	j;\nint	k;\nint	l;\nint	.0;\nint	.1;\nint	.2;\n\n.0 = i * j;\n.1 = k / l;\n.2 = .0 + .1;\n", SUCCESS);
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
