#include <string>
#include <stdio.h>
#include <stdarg.h>

#include "mjolnir.hh"
#include "message.hh"

void	Message::error(std::string &stream, const char *message, ...)
{
  va_list	ap;
  char		str[10000];

  va_start(ap, message);
  vsnprintf(str, sizeof(str), message, ap);
  va_end(ap);
  stream += message;
}
