#ifndef		MJOLNIR_HH_
# define	MJOLNIR_HH_

#  define UNUSED		__attribute__((unused))
#  define USED			__attribute__((used))
#  define NORETURN		__attribute__((noreturn))

#  define ABORT()		__builtin_abort()
#  define PRINTF(...)		__attribute__((format(printf, __VA_ARGS__)))

#  ifdef NULL
#   undef NULL
#  endif // NULL


#include <stddef.h>



static inline void	mj_unreachable_fun(const char *f) NORETURN;
static inline void	mj_assert_fun(const bool cond, const char *fun_name);

#  define	mj_assert(expr) mj_assert_fun(expr, __FUNCTION__)


static inline void	mj_assert_fun(const bool cond, const char *fun_name)
{
  if (cond)
    return ;
  __builtin_printf("assert fail %s: %s %d\n", __FILE__, fun_name, __LINE__);
  ABORT();
}

static inline void	mj_unreachable_fun(const char *fun_name)
{
  __builtin_printf("unreachable path %s: %s %d\n",
		   __FILE__, fun_name, __LINE__);
  ABORT();
}

# define	mj_unreachable() mj_unreachable_fun(__FUNCTION__)


# define SIZEOF_VOID	1
# define SIZEOF_INT	4
# define BIT_PER_BYTE	8



class TempId
{
public:
  explicit inline TempId(void) : counter(0) {}

  inline const std::string	new_temp(void)
  {
    return std::to_string(this->counter++);
  }

  inline const std::string	new_temp(const std::string &begin)
  {
    return begin + std::to_string(this->counter++);
  }
  inline void		reset(void)
  {
    this->counter = 0;
  }
private:
  unsigned long long		counter;
};

inline TempId	&temp_id_creator(void)
{
  static TempId	a;
  return a;
}



#endif		// !MJOLNIR_HH_
