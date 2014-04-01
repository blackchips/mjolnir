#include <vector>
#include <string>

#include "mjolnir.hh"
#include "options.hh"
#include "parser.hh"
#include "dumper.hh"
#include "pass.hh"



Scope	*pass_manager(const File &file, Option &options)
{
  Scope	*scope = nullptr;

  temp_id_creator().reset();
  if (options.parse_p)
    {
      scope = parse(file, options);
      if (!scope)
	return nullptr;
      if (options.dump_parse_p)
	{
	  Dumper::dump(options.dump_stream, scope);
	}
    }
  else
    return nullptr;
  return scope;
}
