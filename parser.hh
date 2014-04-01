#pragma once

class File final
{
  const i8		*name;

  const i8		*content;
  const PTR_SIZE	length_;

 public:
  explicit inline File(const i8 *name_, const i8 *content_, const ui64 &length__) :
    name(name_), content(content_), length_(length__){}
  explicit inline File(const File &file_) :
    name(file_.name), content(file_.content), length_(file_.length()){}
  PTR_SIZE		length(void) const
  {
    return this->length_;
  }
  i8			peek(const PTR_SIZE &pos) const
  {
    return this->content[pos];
  }
};



struct Variable final {
  explicit inline Variable(void) : size(0), name(""){}
  explicit inline Variable(const Variable *other) : size(other->size), name(other->name){}
  inline Variable(const Variable &other) : size(other.size), name(other.name){}
  ui64		size;
  std::string	name;
};



struct Function_prototype final {
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
  Function_prototype			proto;
  std::vector<Variable*>		var;
  std::vector<Function_prototype>	function_prototypes;
  std::vector<Scope>			functions;
  std::vector<Expression>		expr;
};


Scope	*parse(const File &file, Option &options);
