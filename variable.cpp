#include <vector>
#include <string>

#include "mjolnir.hh"
#include "options.hh"
#include "parser.hh"
#include "variable.hh"


Variable	*get_var_with_name(Scope &scope, const std::string &name)
{

  if (!scope.functions.empty())
    {
      Variable	*tmp = get_var_with_name(scope.functions.back(), name);

      if (tmp)
	return tmp;
    }
  for (std::size_t i = 0; i < scope.var.size(); i++)
    {
      if (name == scope.var[i]->name)
	{
	  return scope.var.data()[i];
	}
    }
  return nullptr;
}

Variable	*create_temp_var(Scope &scope, const Variable *var,
				 const std::string &name)
{
  Variable	*temp = new Variable(var);

  temp->name.assign(name);
  scope.var.emplace_back(temp);
  return scope.var.data()[scope.var.size() - 1];
}


void		clear_scope(Scope *scope)
{
  for (auto sc : scope->functions)
    clear_scope(&sc);
  for (auto var : scope->var)
    delete var;
}
