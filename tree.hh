#ifndef		TREE_HH_
# define	TREE_HH_


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


#endif		// !TREE_HH_
