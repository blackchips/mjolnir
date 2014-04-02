#ifndef		VARIABLE_HH_
# define	VARIABLE_HH_

/*
TODO: rename file
 */

/*
** recursivly search in scope and its subscopes for a variable with name
** If one is found returns it otherwise nullptr is returned
**
**
*/
Variable	*get_var_with_name(const Scope &scope, const std::string &name);

/*
** create a new Variable with the type of var
** set its name to name and add it to the scope
** return the resulting var
*/
Variable	*create_temp_var(Scope &scope, const Variable *var,
				 const std::string &name);


/*
** Clear a scope and all it's subscopes.
** Freeing all the memory, ...
*/
void		clear_scope(Scope *scope);


#endif		// !VARIABLE_HH_
