#include <string>
#include <stdio.h>
#include <stdarg.h>

#include "mjolnir.hh"
#include "message.hh"

void	Message::error(std::string &stream, const i8 *message, ...)
{
  va_list	ap;
  i8		str[10000];

  va_start(ap, message);
  vsnprintf(str, sizeof(str), message, ap);
  va_end(ap);
  stream += message;
}
