#ifndef		TREE_HH_
# define	TREE_HH_


/*
** This struct is for storing the maximum amount of datas
** about Variables
*/
struct Variable final {
  explicit inline Variable(void) : size(0), name("")
    { }
  explicit inline Variable(const Variable *other) :
    size(other->size), name(other->name)
    { }
  explicit inline Variable(const Variable &other) :
    size(other.size), name(other.name)
    { }

  /* The size in bits taken by the variable */
  unsigned long long	size;
  /*name of the variable*/
  std::string		name;
};


/*
** Store a function prototype
** For the return type, the name of the variable is the name of the function
*/
struct Function_prototype final {
  explicit Function_prototype(void) : return_type(), params()
    { }

  /*Return type and name of the function*/
  Variable		return_type;
  /*List of all the function params from the left to the right*/
  std::vector<Variable>	params;
};


/*
** Operation with two operands, for exemple:
** result = left + right;
*/
struct Binop {
  /*Left operand*/
  Variable	*left;
  /*right operand*/
  Variable	*right;
  /*result of the operation*/
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
