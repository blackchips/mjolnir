#ifndef		MJOLNIR_HH_
# define	MJOLNIR_HH_

/*
 * Define for some attributes
 *
 * TODO: add some guards to see if the compiler support them
 */
#  define UNUSED		__attribute__((unused))
#  define NORETURN		__attribute__((noreturn))
#  define PRINTF(...)		__attribute__((format(printf, __VA_ARGS__)))


/*
** Define some common builtins
*/
#  define UNREACHABLE()		__builtin_unreachable()

/*
** definition of a macro for unreachable who calls a function
** If it is made as a macro we have to use {} to gard between ifs
** If it is made as a function __FUNCTION__ won't be set properly
*/
static inline void	mj_unreachable_fun(const char *f) NORETURN;

static inline void	mj_unreachable_fun(const char *fun_name)
{
  __builtin_printf("unreachable path %s: %s %d\n",
		   __FILE__, fun_name, __LINE__);
  UNREACHABLE();
}

# define	mj_unreachable() mj_unreachable_fun(__FUNCTION__)




/*
** definition of a macro for assert who calls a function
** If it is made as a macro we have to use {} to gard between ifs
** If it is made as a function __FUNCTION__ won't be set properly
*/
static inline void	mj_assert_fun(const bool cond, const char *fun_name);

static inline void	mj_assert_fun(const bool cond, const char *fun_name)
{
  if (cond)
    return ;
  __builtin_printf("assert fail %s: %s %d\n", __FILE__, fun_name, __LINE__);
  UNREACHABLE();
}

#  define	mj_assert(expr) mj_assert_fun(expr, __FUNCTION__)



/*
** define somme usefull macro for the targeted host
*/

# define SIZEOF_VOID	1
# define SIZEOF_INT	4
# define BIT_PER_BYTE	8





/*
  Include to have the correct definition of size_t
 */
#include <stddef.h>

#endif		// !MJOLNIR_HH_
