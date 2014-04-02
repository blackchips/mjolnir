#include <string>
#include <stdio.h>
#include <stdarg.h>

#include "mjolnir.hh"
#include "message.hh"

void	Message::error(std::string &stream, const char *message, ...)
{
  va_list	ap; /* For the va_args*/
  char		str[10000]; /*buff where everything will go*/

  va_start(ap, message);
  /*TODO make my own printf implementation*/
  vsnprintf(str, sizeof(str), message, ap);
  va_end(ap);
  stream.append(message);
}
