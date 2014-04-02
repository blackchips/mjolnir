#ifndef		TEMP_FACTORY_HH
# define	TEMP_FACTORY_HH


/*
**
** class to create temporary names (or anything else
** Only one should be used per compilation unit and
** one should not be shared with other compilations units.
**
*/
class Temp_factory
{
public:
  /*
  ** initialize the factory.
  ** This is the only constructor who should be used
  */
  explicit inline Temp_factory(void) : counter(0)
  { }

  /*
  ** create a new temp_name starting with begin
  **
  */
  inline const std::string	new_temp(const std::string &begin)
  {
    return begin + std::to_string(this->counter++);
  }

  /*
  ** reset the instance of the factory
  ** so it can be used again
  */
  inline void		reset(void)
  {
    this->counter = 0;
  }
private:
  /*counter to create uniq id*/
  unsigned long long		counter;
};

inline Temp_factory	&temp_factory(void)
{
  static Temp_factory	tmp;
  return tmp;
}


#endif		//!TEMP_FACTORY_HH
