#ifndef		VARIABLE_HH_
# define	VARIABLE_HH_

/*
TODO: rename file
 */


/*cannot set scope const because we might modify some of it's content later*/
Variable	*get_var_with_name(Scope &scope, const std::string &name);
Variable	*create_temp_var(Scope &scope, const Variable *var, const std::string &name);

void		clear_scope(Scope *scope);


#endif		// !VARIABLE_HH_
