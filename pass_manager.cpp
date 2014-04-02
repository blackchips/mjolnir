#include <vector>
#include <string>

#include "mjolnir.hh"
#include "options.hh"
#include "tree.hh"
#include "parser.hh"
#include "dumper.hh"
#include "pass.hh"
#include "temp_factory.hh"


Scope	*pass_manager(const File &file, Options &options)
{
  Scope	*scope = nullptr;   /* The compilation unit*/

  temp_factory().reset();
  if (options.parse_p)
    {
      scope = parse(file, options);
      if (!scope)
	return nullptr;
      if (options.dump_parse_p)
	Dumper::dump(options.dump_stream, scope);
    }
  else
    return nullptr;
  return scope;
}
