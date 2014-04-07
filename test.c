#include "mjolnir.h"
#include "test.h"

# define ADD_TEST_SUCCESS(file)			\
  do						\
    {						\
    }						\
  while (0);

# define ADD_TEST_FAIL(file)			\
  do						\
    {						\
    }						\
  while (0);

_Bool	run_tests(void)
{
  ADD_TEST_SUCCESS("empty.c");
  ADD_TEST_FAIL("dummy.c");
  return true;
}
