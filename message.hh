#ifndef		MESSAGE_HH_
# define	MESSAGE_HH_

namespace Message {
  void	error(std::string &stream, const i8 *message, ...) __attribute__((format(printf, 2, 3)));
}

#endif		// MESSAGE_HH_