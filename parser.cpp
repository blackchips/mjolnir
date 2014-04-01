#include <string>
#include <vector>
#include <array>

#include "mjolnir.hh"
#include "options.hh"
#include "parser.hh"
#include "message.hh"
#include "variable.hh"
#include "lexer.hh"



class Parser final
{
public:
  explicit inline Parser(Options &options, const File &file) :
    options_(options), scope_level_(0), scope_(new Scope()),
    lexer_(file, options)
  { }
  Parser(void) = delete;
  void	operator=(const Parser &) = delete;
  Parser(const Parser &parser) = delete;
public:
  Scope			*parse(void);


private:
  bool			parse_identifier(Variable &var);
  bool			parse_type(Variable &var) const;
  bool			parse_function_prototype(const Variable &var,
						 Function_prototype &fun_proto);
  bool			parse_variable(Variable &var);
  bool			parse_expression(Scope &scope);
  bool			parse_scope(Scope &ast);


  static void		add_var_to_scope(Scope &scope, const Variable &var);
  bool			leave_scope_p(void);
  bool			handle_operation(Scope &scope, std::array<std::string, 3> &name,
					 std::array<Token, 2> &op);
  static bool		prioritary_operator_p(const Token &token);

 private:
  Options		&options_;
  int			scope_level_;
  Scope			*scope_;
  Lexer			lexer_;
};


bool		Parser::parse_variable(Variable &var)
{
  if (!this->parse_type(var))
    return false;
  return this->parse_identifier(var);
}

bool		Parser::parse_type(Variable &var) const
{
  switch (this->lexer_.peek_token())
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
  this->lexer_.advance_token();
  switch (this->lexer_.peek_token())
    {
    case Token::SEMI_COLON: case Token::COMA: case Token::CLOSE_PARENT:
      break ;
    case Token::WORD:
      var.name.assign(this->lexer_.identifier());
      this->lexer_.advance_token();
      break ;
    case Token::ERROR:
      return false;
    default:
      Message::error(this->options_.error_stream,
		     "expected identifier or '('\n");
      return false;
    }
  return true;
}

void	Parser::add_var_to_scope(Scope &scope, const Variable &var)
{
  Variable	*tmp = new Variable(var);

  scope.var.push_back(tmp);
}


bool		Parser::parse_function_prototype(const Variable &var,
						 Function_prototype &fun_proto)
{
  fun_proto.return_type = var;
  while (this->lexer_.peek_token() != Token::CLOSE_PARENT)
    {
      Variable	param;

      this->lexer_.advance_token();
      if (this->parse_type(param))
	{
	  this->lexer_.advance_token();
	  if (this->lexer_.peek_token() == Token::WORD)
	    {
	      param.name = this->lexer_.identifier();
	      this->lexer_.advance_token();
	    }
	  fun_proto.params.push_back(param);
	  switch (this->lexer_.peek_token())
	    {
	    case Token::COMA:
	      break;
	    case Token::CLOSE_PARENT:
	      return true;
	    case Token::ERROR:
	      return false;
	    default:
	      Message::error(this->options_.error_stream,
			     "expected identifier or ','\n");
	      return false;
	    }
	}
      else if (this->lexer_.peek_token() == Token::CLOSE_PARENT)
	return true;
      else
	{
	  Message::error(this->options_.error_stream, "expected params or ')'\n");
	  return false;
	}
    }
  return true;
}

bool	Parser::leave_scope_p(void)
{
  if (this->scope_level_)
    {
      this->scope_level_--;
      return true;
    }
  else
    {
      Message::error(this->options_.error_stream, "too many '}'\n");
      return false;
    }
}

bool	Parser::prioritary_operator_p(const Token &tok)
{
  return tok == Token::STAR || tok == Token::DIVISION;
}

bool	Parser::handle_operation(Scope &scope, std::array<std::string, 3> &name,
				 std::array<Token, 2> &op)
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
	  Message::error(this->options_.error_stream,
			 "could not find the var %s\n", name[0].c_str());
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
      expr.binop.right = get_var_with_name(*this->scope_,
					   name[idx_operator + 1u]);
      if (!expr.binop.left)
	{
	  Message::error(this->options_.error_stream,
			 "could not find the var %s\n",
			 name[idx_operator].c_str());
	  return false;
	}
      if (!expr.binop.right)
	{
	  Message::error(this->options_.error_stream,
			 "could not find the var %s\n",
			 name[idx_operator + 1u].c_str());
	  return false;
	}
      expr.binop.result = create_temp_var(scope, expr.binop.left,
					  temp_id_creator().new_temp("."));
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
	name[0] = this->lexer_.identifier();
      else if (name[1].empty())
	name[1] = this->lexer_.identifier();
      else if (name[2].empty())
	name[2] = this->lexer_.identifier();
      else
	mj_unreachable();
      switch (this->lexer_.advance_token())
	{
	case Token::PLUS: case Token::MINUS:
	case Token::STAR: case Token::DIVISION:
	  if (operations[0] == Token::EOF_)
	    operations[0] = this->lexer_.peek_token();
	  else if (operations[1] == Token::EOF_)
	    operations[1] = this->lexer_.peek_token();
	  else
	    {
	      if (!this->handle_operation(scope, name, operations))
		return false;
	      operations[1] = this->lexer_.peek_token();
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
	    Message::error(this->options_.error_stream,
			   "expected operator or ';'\n");
	    return false;
	  }
	}
      if (this->lexer_.advance_token() != Token::WORD)
	{
	  Message::error(this->options_.error_stream,
			 "expected identifier or constant\n");
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

      switch (this->lexer_.advance_token())
	{
	case Token::SEMI_COLON:
	  continue ;
	case Token::EOF_:
	  return true;
	case Token::OPEN_CURLY_BRACE:
	  {
	    if (!this->scope_level_)
	      {
		Message::error(this->options_.error_stream,
			       "expected identifier\n");
		goto error;
	      }
	    else
	      {
		Scope	tmp_scope;
		bool	success_p;

		this->scope_level_++;
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
	      switch (this->lexer_.peek_token())
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
		    switch (this->lexer_.advance_token())
		      {
		      case Token::SEMI_COLON:
			scope.function_prototypes.push_back(fun_proto);
			break;
		      case Token::OPEN_CURLY_BRACE:
			{
			  bool	success_p;
			  Scope	tmp_scope;

			  this->scope_level_++;
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
			Message::error(this->options_.error_stream, "expected ';' or '{'\n");
			goto error;
		      }
		    break;
		  }
		default:
		  if (var.name.empty())
		    Message::error(this->options_.error_stream,
				   "expected identifier or '('\n");
		  else
		    Message::error(this->options_.error_stream, "expected ';' or '('\n");
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
	  Message::error(this->options_.error_stream,
			 "expected declaration or identifier\n");
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
      if (this->scope_level_)
	{
	  Message::error(this->options_.error_stream, "missing '}'\n");
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

Scope	*parse(const File &file, Options &options)
{
  Parser	parser(options, file);

  return parser.parse();
}
