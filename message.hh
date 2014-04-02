#ifndef		MESSAGE_HH_
# define	MESSAGE_HH_

/*
** Namespace for all the message stuff
**
*/
namespace Message {
  /*
  ** write an error message into the designated stream
  ** Uses the same printf attribute than printf
  */
  void	error(std::string &stream, const char *message, ...) PRINTF(2,3);
}

#endif		// MESSAGE_HH_
