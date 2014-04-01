#include <string>
#include <vector>
#include <array>

#include "mjolnir.hh"
#include "options.hh"
#include "parser.hh"
#include "message.hh"
#include "variable.hh"


enum Token
  {
    ERROR,
    EOF_,

    SEMI_COLON,
    COMA,

    OPEN_PARENT,
    CLOSE_PARENT,
    OPEN_CURLY_BRACE,
    CLOSE_CURLY_BRACE,

    PLUS,
    MINUS,
    STAR,
    DIVISION,
    WORD,
    INT,
    VOID,
  };


class FileIt final
{
  File		file;
  size_t	pos;

 public:
  explicit inline FileIt(const File &file_) :
    file(file_), pos(0ul) {}
  bool	eof_p(void) const
  {
    return this->pos == this->file.length();
  }
  char	peek_and_advance(void)
  {
    const char	c(this->peek());

    this->advance();
    return c;
  }
  void	advance(void)
  {
    mj_assert(!this->eof_p());
    this->pos++;
  }
  char	peek(void) const
  {
    return this->file.peek(this->pos);
  }
};



class Parser final
{
public:
  explicit inline Parser(Option &options_, const File &file_) :
    it(file_), identifier(), token(Token::ERROR), options(options_), scope_level(0), scope_(new Scope())
  {}
  Parser(const Parser &parser) = delete;
public:
  Scope			*parse(void);


private:
  bool			eof_p(void) const;
  bool			parse_scope(Scope &ast);
  char			peek_and_advance_char(void);
  Token			parse_word(const char c);
  void			advance_char(void);
  char			peek_char(void) const;
  Token			string_to_token(void) const;


  Token			advance_token(void);
  Token			advance_token_2(void);
  Token			peek_token(void) const;


  bool			parse_identifier(Variable &var);
  bool			parse_type(Variable &var);
  bool			parse_function_prototype(const Variable &var, Function_prototype &fun_proto);
  bool			parse_variable(Variable &var);
  bool			parse_expression(Scope &scope);


  static void		add_var_to_scope(Scope &scope, const Variable &var);
  bool			leave_scope_p(void);
  bool			handle_operation(Scope &scope, std::array<std::string, 3> &name, std::array<Token, 2> &op);
  static bool		prioritary_operator_p(const Token &token);
  bool			skip_c_comment(void);
  bool			skip_cpp_comment(void);

 private:
  FileIt		it;
  std::string		identifier;
  Token			token;
  Option		&options;
  int			scope_level;
  Scope			*scope_;
};

Token		Parser::peek_token(void) const
{
  return this->token;
}

void	Parser::advance_char(void)
{
  this->it.advance();
}

char	Parser::peek_char(void) const
{
  return this->it.peek();
}


bool	Parser::eof_p(void) const
{
  return this->it.eof_p();
}


char	Parser::peek_and_advance_char(void)
{
  return this->it.peek_and_advance();
}

Token	Parser::string_to_token(void) const
{
  const struct {
    const char		*str;
    const Token		tok;
  }	map_keyword_token [] =
	  {
	    {"int", Token::INT},
	    {"void", Token::VOID}
	  };

  for (auto elem : map_keyword_token)
    {
      if (elem.str == this->identifier)
	return elem.tok;
    }
  return Token::WORD;
}

Token	Parser::parse_word(const char c)
{
  this->identifier.push_back(c);
  while (!this->eof_p())
    {
      const char	c2(this->peek_char());

      if ((c2 >= 'a' && c2 <= 'z') || (c2 >= 'A' && c2 <= 'Z') || c2 == '_' || (c2 >= '0' && c2 <= '9'))
	{
	  this->identifier.push_back(c2);
	  this->advance_char();
	}
      else
	break;
    }
  return this->string_to_token();
}

bool	Parser::skip_cpp_comment(void)
{
  while (1)
    {
      this->advance_char();
      if (this->peek_char() == '\n')
	return true;
      else if (this->eof_p())
	{
	  Message::error(this->options.error_stream, "c++ style comment should be finished by a new-line\n");
	  return false;
	}
    }
}

bool	Parser::skip_c_comment(void)
{
  while (1)
    {
      this->advance_char();
    redo_no_advance:
      if (this->peek_char() == '*')
	{
	  this->advance_char();
	  if (this->peek_char() == '/')
	    {
	      this->advance_char();
	      return true;
	    }
	  else
	    goto redo_no_advance;
	}
      else if (this->eof_p())
	{
	  Message::error(this->options.error_stream, "unfinished c comment\n");
	  return false;
	}
    }
}

Token	Parser::advance_token_2(void)
{
  this->identifier.clear();
  while (true)
    {
      if (this->eof_p())
	return Token::EOF_;
      const char c(this->peek_and_advance_char());

      switch (c)
	{
	case '\n':
	  break ;
	case ';':
	  return Token::SEMI_COLON;
	case ' ': case '\t':
	  break ;
	case ',':
	  return Token::COMA;
	case '(':
	  return Token::OPEN_PARENT;
	case ')':
	  return Token::CLOSE_PARENT;
	case '{':
	  return Token::OPEN_CURLY_BRACE;
	case '}':
	  return Token::CLOSE_CURLY_BRACE;
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':
	case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
	case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I':
	case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
	case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
	case '_':
	  return this->parse_word(c);

	case '+':
	  {
	    const char	n(this->peek_char());

	    if (n == '+')
	      mj_unreachable();
	    else if (n == '=')
	      mj_unreachable();
	    else
	      return Token::PLUS;
	  }
	case '-':
	  {
	    const char	n(this->peek_char());

	    if (n == '-')
	      mj_unreachable();
	    else if (n == '=')
	      mj_unreachable();
	    else if (n == '>')
	      mj_unreachable();
	    else
	      return Token::MINUS;
	  }

	case '*':
	  {
	    const char	n(this->peek_char());

	    if (n == '=')
	      mj_unreachable();
	    else
	      return Token::STAR;
	  }

	case '/':
	  {
	    const char	n(this->peek_char());

	    if (n == '=')
	      mj_unreachable();
	    else if (n == '*')
	      {
		if (!this->skip_c_comment())
		  return Token::ERROR;
		break;
	      }
	    else if (n == '/')
	      {
		if (this->options.c_dialect == C_dialect::C_89)
		  return Token::DIVISION;
		else
		  {
		    if (!this->skip_cpp_comment())
		      return Token::ERROR;
		  }
		break;
	      }
	    else
		return Token::DIVISION;
	  }
	default:
	  mj_unreachable();
	}
    }
}

Token	Parser::advance_token(void)
{
  this->token = advance_token_2();
  return this->token;
}

bool		Parser::parse_variable(Variable &var)
{
  if (!this->parse_type(var))
    return false;
  return this->parse_identifier(var);
}

bool		Parser::parse_type(Variable &var)
{
  switch (this->peek_token())
    {
    case Token::INT:
      var.size = SIZEOF_INT * BIT_PER_BYTE;
      break;
    case Token::VOID:
      var.size = SIZEOF_VOID * BIT_PER_BYTE;
      break;
    default:
      return false;
    }
  return true;
}

bool		Parser::parse_identifier(Variable &var)
{
  this->advance_token();
  switch (this->peek_token())
    {
    case Token::SEMI_COLON: case Token::COMA: case Token::CLOSE_PARENT:
      break ;
    case Token::WORD:
      var.name.assign(this->identifier);
      this->advance_token();
      break ;
    case Token::ERROR:
      return false;
    default:
      Message::error(this->options.error_stream, "expected identifier or '('\n");
      return false;
    }
  return true;
}

void	Parser::add_var_to_scope(Scope &scope, const Variable &var)
{
  Variable	*tmp = new Variable(var);

  scope.var.push_back(tmp);
}


bool		Parser::parse_function_prototype(const Variable &var, Function_prototype &fun_proto)
{
  fun_proto.return_type = var;
  while (this->peek_token() != Token::CLOSE_PARENT)
    {
      Variable	param;

      this->advance_token();
      if (this->parse_type(param))
	{
	  this->advance_token();
	  if (this->peek_token() == Token::WORD)
	    {
	      param.name = this->identifier;
	      this->advance_token();
	    }
	  fun_proto.params.push_back(param);
	  switch (this->peek_token())
	    {
	    case Token::COMA:
	      break;
	    case Token::CLOSE_PARENT:
	      return true;
	    case Token::ERROR:
	      return false;
	    default:
	      Message::error(this->options.error_stream, "expected identifier or ','\n");
	      return false;
	    }
	}
      else if (this->peek_token() == Token::CLOSE_PARENT)
	return true;
      else
	{
	  Message::error(this->options.error_stream, "expected params or ')'\n");
	  return false;
	}
    }
  return true;
}

bool	Parser::leave_scope_p(void)
{
  if (this->scope_level)
    {
      this->scope_level--;
      return true;
    }
  else
    {
      Message::error(this->options.error_stream, "too many '}'\n");
      return false;
    }
}

bool	Parser::prioritary_operator_p(const Token &tok)
{
  return tok == Token::STAR || tok == Token::DIVISION;
}

bool	Parser::handle_operation(Scope &scope, std::array<std::string, 3> &name, std::array<Token, 2> &op)
{
  if (name[0].empty())
    mj_unreachable();
  else if (name[1].empty())
    {
      Expression	expr;

      expr.kind = Expression::Kind::NOOP;
      expr.unop.result = nullptr;
      expr.unop.expr = get_var_with_name(*this->scope_, name[0]);
      if (!expr.unop.expr)
	{
	  Message::error(this->options.error_stream, "could not find the var %s\n", name[0].c_str());
	  return false;
	}
      scope.expr.push_back(expr);
      name[0].assign("");
    }
  else
    {
      Expression	expr;
      bool		idx_operator = 0;

      if (this->prioritary_operator_p(op[0]))
	idx_operator = 0;
      else if (this->prioritary_operator_p(op[1]))
	idx_operator = 1;
      else
	idx_operator = 0;

      switch (op[idx_operator])
	{
	case Token::PLUS:
	  expr.kind = Expression::Kind::ADDITION;
	  break ;
	case Token::MINUS:
	  expr.kind = Expression::Kind::SOUSTRACTION;
	  break;
	case Token::STAR:
	  expr.kind = Expression::Kind::MULTIPLICATION;
	  break;
	case Token::DIVISION:
	  expr.kind = Expression::Kind::DIVISION;
	  break;
	default:
	  mj_unreachable();
	}
      expr.binop.left = get_var_with_name(*this->scope_, name[idx_operator]);
      expr.binop.right = get_var_with_name(*this->scope_, name[idx_operator + 1]);
      if (!expr.binop.left)
	{
	  Message::error(this->options.error_stream, "could not find the var %s\n", name[idx_operator].c_str());
	  return false;
	}
      if (!expr.binop.right)
	{
	  Message::error(this->options.error_stream, "could not find the var %s\n", name[idx_operator + 1].c_str());
	  return false;
	}
      expr.binop.result = create_temp_var(scope, expr.binop.left, temp_id_creator().new_temp("."));
      scope.expr.push_back(expr);
      if (idx_operator == 0)
	{
	  name[0].assign(expr.binop.result->name);
	  name[1].assign(name[2]);
	  op[0] = op[1];
	}
      else /*idx_operator == 1*/
	name[1].assign(expr.binop.result->name);
      name[2].assign("");
      op[1] = Token::EOF_;
    }
  return true;
}

bool	Parser::parse_expression(Scope &scope)
{
  std::array<std::string, 3>	name;
  std::array<Token, 2>		operations;

  operations.fill(Token::EOF_);
  name.fill("");
  while (true)
    {
      if (name[0].empty())
	name[0] = this->identifier;
      else if (name[1].empty())
	name[1] = this->identifier;
      else if (name[2].empty())
	name[2] = this->identifier;
      else
	mj_unreachable();
      switch (this->advance_token())
	{
	case Token::PLUS: case Token::MINUS: case Token::STAR: case Token::DIVISION:
	  if (operations[0] == Token::EOF_)
	    operations[0] = this->peek_token();
	  else if (operations[1] == Token::EOF_)
	    operations[1] = this->peek_token();
	  else
	    {
	      if (!this->handle_operation(scope, name, operations))
		return false;
	      operations[1] = this->peek_token();
	    }
	  break;
	case Token::SEMI_COLON:
	  {
	    if (!this->handle_operation(scope, name, operations))
	      return false;
	    else if (name[1].empty())
	      return true;
	    else
	      return this->handle_operation(scope, name, operations);
	  }
	case Token::ERROR:
	  return false;
	default:
	  {
	    Message::error(this->options.error_stream, "expected operator or ';'\n");
	    return false;
	  }
	}
      if (this->advance_token() != Token::WORD)
	{
	  Message::error(this->options.error_stream, "expected identifier or constant\n");
	  return false;
	}
    }
}

bool	Parser::parse_scope(Scope &scope)
{
  while (true)
    {
      Variable			var;
      Function_prototype	fun_proto;

      switch (this->advance_token())
	{
	case Token::SEMI_COLON:
	  continue ;
	case Token::EOF_:
	  return true;
	case Token::OPEN_CURLY_BRACE:
	  {
	    if (!this->scope_level)
	      {
		Message::error(this->options.error_stream, "expected identifier\n");
		goto error;
	      }
	    else
	      {
		Scope	tmp_scope;
		bool	success_p;

		this->scope_level++;
		success_p = this->parse_scope(tmp_scope);
		if (success_p)
		  {
		    scope.functions.push_back(tmp_scope);
		    break;
		  }
		else
		  goto error;
	      }
	  }
	case Token::CLOSE_CURLY_BRACE:
	  if (!this->leave_scope_p())
	    goto error;
	  else
	    return true;
	case Token::INT: case Token::VOID:
	  {
	    if (!this->parse_variable(var))
	      goto error;
	    {
	      switch (this->peek_token())
		{
		case Token::SEMI_COLON:
		  {
		    this->add_var_to_scope(scope, var);
		    break;
		  }
		case Token::OPEN_PARENT:
		  {
		    if (!this->parse_function_prototype(var, fun_proto))
		      goto error;
		    switch (this->advance_token())
		      {
		      case Token::SEMI_COLON:
			scope.function_prototypes.push_back(fun_proto);
			break;
		      case Token::OPEN_CURLY_BRACE:
			{
			  bool	success_p;
			  Scope	tmp_scope;

			  this->scope_level++;
			  success_p = this->parse_scope(tmp_scope);
			  if (!success_p)
			    goto error;
			  else
			    {
			      tmp_scope.proto = fun_proto;
			      scope.functions.push_back(tmp_scope);
			      break;
			    }
			}
		      default:
			Message::error(this->options.error_stream, "expected ';' or '{'\n");
			goto error;
		      }
		    break;
		  }
		default:
		  if (var.name.empty())
		    Message::error(this->options.error_stream, "expected identifier or '('\n");
		  else
		    Message::error(this->options.error_stream, "expected ';' or '('\n");
		  goto error;
		}
	    }
	    break ;
	  }
	case Token::WORD:
	  {
	    if (this->parse_expression(scope))
	      break;
	    else
	      goto error;
	  }
	case Token::ERROR:
	  return false;
	default:
	  Message::error(this->options.error_stream, "expected declaration or identifier\n");
	  goto error;
	}
    }
 error:
  return false;
}

Scope	*Parser::parse(void)
{
  bool		success_p;

  success_p = this->parse_scope(*this->scope_);
  if (success_p)
    {
      if (this->scope_level)
	{
	  Message::error(this->options.error_stream, "missing '}'\n");
	  goto fail;
	}
      else
	return this->scope_;
    }
  else
    goto fail;
 fail:
  clear_scope(this->scope_);
  delete this->scope_;
  return nullptr;
}

Scope	*parse(const File &file, Option &options)
{
  Parser	parser(options, file);

  return parser.parse();
}
