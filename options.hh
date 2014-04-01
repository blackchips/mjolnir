#ifndef		OPTIONS_H_
# define	OPTIONS_H_

# define ADD_PASS(name)				\
  bool	dump_##name##_p;			\
  bool	do_##name##_p;



enum C_dialect {
  C_89,
  C_99,
  };

struct Option
{
  inline explicit Option(void) : error_stream(""), dump_stream(""), parse_p(true), dump_parse_p(false), c_dialect(C_99) {}
  std::string	error_stream;
  std::string	dump_stream;
  bool		parse_p;
  bool		dump_parse_p;
  ui64		: 16;
  C_dialect	c_dialect;

};

#endif		//OPTIONS_H_
