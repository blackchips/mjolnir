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



struct Variable final {
  explicit inline Variable(void) : size(0), name("")
    { }
  explicit inline Variable(const Variable *other) :
    size(other->size), name(other->name)
    { }
  explicit inline Variable(const Variable &other) :
    size(other.size), name(other.name)
    { }


  unsigned long long		size;
  std::string	name;
};



struct Function_prototype final {
  explicit Function_prototype(void) : return_type(), params()
    { }

  Variable		return_type;
  std::vector<Variable>	params;
};


struct Binop {
  Variable	*left;
  Variable	*right;
  Variable	*result;
};

struct Unop {

  Variable	*result;
  Variable	*expr;
};

struct Expression final {
  enum Kind {
    ADDITION,
    SOUSTRACTION,
    MULTIPLICATION,
    DIVISION,
    NOOP,
  }		kind;

  union {
    Unop		unop;
    Binop		binop;
  };
};

struct Scope final
{
  explicit Scope(void) : proto(), var(), function_prototypes(),
    functions(), expr()
    { }

  Function_prototype			proto;
  std::vector<Variable*>		var;
  std::vector<Function_prototype>	function_prototypes;
  std::vector<Scope>			functions;
  std::vector<Expression>		expr;
};


Scope	*parse(const File &file, Options &options);

#endif		//!PARSER_HH_
