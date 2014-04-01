#include <vector>
#include <string>

#include "mjolnir.hh"
#include "options.hh"
#include "parser.hh"
#include "dumper.hh"

static void	dump_type(std::string &stream, const Variable *var);
static void	dump_variable(std::string &stream, const Variable *var,
			      bool tab_p);
static void	dump_function_prototype(std::string &stream,
					const Function_prototype &proto);
static void	dump_scope(std::string &stream, const Scope *scope,
			   unsigned int level);
static void	dump_expression(std::string &stream, const Expression &expr);
static const char	*get_string_for_operator(Expression::Kind ope);


static void	dump_type(std::string &stream, const Variable *var)
{
  if (var->size == 8)
    stream.append("void");
  else
    {
      stream.append("i");
      stream.append(std::to_string(var->size));
    }
}

static void	dump_variable(std::string &stream, const Variable *var,
			      bool tab_p)
{
  dump_type(stream, var);
  if (!var->name.empty())
    {
      stream.append(tab_p ? "\t" : " ");
      stream.append(var->name);
    }
}

static void	dump_function_prototype(std::string &stream,
					const Function_prototype &proto)
{
  // It's the glabal namespace
  if (proto.return_type.name == "")
    return ;
  else
    {
      dump_variable(stream, &proto.return_type, true /*tab_p*/);
      stream.append("(");
      if (!proto.params.empty())
	{
	  for (unsigned int i = 0; i < proto.params.size() - 1; i++)
	    {
	      dump_variable(stream, &proto.params[i], false /*tab_p*/);
	      stream.append(", ");
	    }
	  if (!proto.params.empty())
	    dump_variable(stream, &proto.params[proto.params.size() - 1],
			  false /*tab_p*/);
	}
      stream.append(")");
    }
}

static const char	*get_string_for_operator(Expression::Kind ope)
{
  if (ope == Expression::Kind::ADDITION)
    return " + ";
  else if (ope == Expression::Kind::SOUSTRACTION)
    return " - ";
  else if (ope == Expression::Kind::MULTIPLICATION)
    return " * ";
  else if (ope == Expression::Kind::DIVISION)
    return " / ";
  else
    mj_unreachable();
}

static void	dump_expression(std::string &stream, const Expression &expr)
{
  switch (expr.kind)
    {
    case Expression::Kind::ADDITION: case Expression::Kind::SOUSTRACTION:
    case Expression::Kind::MULTIPLICATION: case Expression::Kind::DIVISION:
      stream.append(expr.binop.result->name);
      stream.append(" = ");
      stream.append(expr.binop.left->name);
      stream.append(get_string_for_operator(expr.kind));
      stream.append(expr.binop.right->name);
      stream.append(";\n");
      break;

    case Expression::Kind::NOOP:
      {
	stream.append(expr.unop.expr->name);
	stream.append(";\n");
	break;
      }
    }
}


static void	dump_scope(std::string &stream, const Scope *scope,
			   unsigned int level)
{
  dump_function_prototype(stream, scope->proto);
  if (level == 1)
    stream.append("\n{\n");
  else if (level)
    stream.append("{\n");
  for (auto proto : scope->function_prototypes)
    {
      dump_function_prototype(stream, proto);
      stream.append(";\n");
    }
  for (auto var : scope->var)
    {
      dump_variable(stream, var, true/*tab_p*/);
      stream.append(";\n");
    }
  for (auto function : scope->functions)
    {
      dump_scope(stream, &function, level + 1);
    }
  if ((!scope->var.empty() || !scope->function_prototypes.empty()) &&
      !scope->expr.empty())
    {
      stream.append("\n");
    }
  for (auto expr : scope->expr)
    {
      dump_expression(stream, expr);
    }
  if (level)
    stream.append("}\n");
}

void	Dumper::dump(std::string &stream, const Scope *scope)
{
  dump_scope(stream, scope, 0);
}
