#ifndef		MESSAGE_HH_
# define	MESSAGE_HH_

namespace Message {
  void	error(std::string &stream, const char *message, ...) PRINTF(2,3);
}

#endif		// MESSAGE_HH_
