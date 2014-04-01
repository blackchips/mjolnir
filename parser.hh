#ifndef		PARSER_HH_
# define	PARSER_HH_

class File final
{
  const char	*name;
  const char	*content;
  const size_t	length_;

 public:
  explicit inline File(const char *name_, const char *content_,
		       const size_t &length__) :
    name(name_), content(content_), length_(length__)
  { }
  explicit inline File(const File &file_) :
    name(file_.name), content(file_.content), length_(file_.length())
    { }

  size_t		length(void) const
  {
    return this->length_;
  }
  char			peek(const size_t &pos) const
  {
    return this->content[pos];
  }
};




Scope	*parse(const File &file, Options &options);

#endif		//!PARSER_HH_
