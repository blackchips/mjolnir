#ifndef		MJOLNIR_HH_
# define	MJOLNIR_HH_

#  define UNUSED		__attribute__((unused))
#  define USED			__attribute__((used))
#  define NORETURN		__attribute__((noreturn))

#  define ABORT()		__builtin_abort()

#  ifdef NULL
#   undef NULL
#  endif // NULL


typedef char			i8;
typedef unsigned char		ui8;
typedef short			i16;
typedef unsigned short		ui16;
typedef int			i32;
typedef unsigned int		ui32;
typedef long long int		u64;
typedef unsigned long long	ui64;

typedef float			f32;
typedef double			f64;


typedef	unsigned long long	PTR_SIZE;
typedef	unsigned long long	PTR_DIFF;

#  pragma GCC poison signed
#  pragma GCC poison unsigned
#  pragma GCC poison char
#  pragma GCC poison short
#  pragma GCC poison int
#  pragma GCC poison long

#  pragma GCC poison float
#  pragma GCC poison double

static inline void	mj_unreachable_fun(const i8 *f) NORETURN;
static inline void	mj_assert_fun(const bool cond, const i8 *fun_name);

#  define	mj_assert(expr) mj_assert_fun(expr, __FUNCTION__)


static inline void	mj_assert_fun(const bool cond, const i8 *fun_name)
{
  if (cond)
    return ;
  __builtin_printf("assert fail %s: %s %d\n", __FILE__, fun_name, __LINE__);
  ABORT();
}

static inline void	mj_unreachable_fun(const i8 *fun_name)
{
  __builtin_printf("unreachable path %s: %s %d\n", __FILE__, fun_name, __LINE__);
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
  ui64		counter;
};

inline TempId	&temp_id_creator(void)
{
  static TempId	a;
  return a;
}



#endif		// !MJOLNIR_HH_
