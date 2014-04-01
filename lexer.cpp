#include <string>
#include <vector>

#include "mjolnir.hh"
#include "options.hh"
#include "message.hh"
#include "parser.hh"
#include "lexer.hh"


Token		Lexer::peek_token(void) const
{
  return this->token_;
}

void	Lexer::advance_char(void)
{
  this->it_.advance();
}

char	Lexer::peek_char(void) const
{
  return this->it_.peek();
}


bool	Lexer::eof_p(void) const
{
  return this->it_.eof_p();
}


char	Lexer::peek_and_advance_char(void)
{
  return this->it_.peek_and_advance();
}

Token	Lexer::string_to_token(void) const
{
  const struct {
    const char		*str;
    const Token		tok;
  }	map_keyword_token [] =
	  {
	    {"int", Token::INT},
	    {"void", Token::VOID}
	  };

  for (auto elem : map_keyword_token)
    {
      if (elem.str == this->identifier_)
	return elem.tok;
    }
  return Token::WORD;
}

# define isnum(c) (c >= '0' && c <= '9')
# define islower(c) (c >= 'a' && c <= 'z')
# define isupper(c) (c >= 'A' && c <= 'Z')
# define isalpha(c) (islower(c) || isupper(c))
# define isalnum(c)  (isalpha(c) || isnum(c))

Token	Lexer::parse_word(const char c)
{
  this->identifier_.push_back(c);
  while (!this->eof_p())
    {
      const char	c2(this->peek_char());

      if (isalnum(c2) || c2 == '_')
	{
	  this->identifier_.push_back(c2);
	  this->advance_char();
	}
      else
	break;
    }
  return this->string_to_token();
}

bool	Lexer::skip_cpp_comment(void)
{
  while (1)
    {
      this->advance_char();
      if (this->peek_char() == '\n')
	return true;
      else if (this->eof_p())
	{
	  Message::error(this->options_.error_stream,
			 "c++ style comment should be finished by a new-line\n");
	  return false;
	}
    }
}

bool	Lexer::skip_c_comment(void)
{
  while (1)
    {
      this->advance_char();
    redo_no_advance:
      if (this->peek_char() == '*')
	{
	  this->advance_char();
	  if (this->peek_char() == '/')
	    {
	      this->advance_char();
	      return true;
	    }
	  else
	    goto redo_no_advance;
	}
      else if (this->eof_p())
	{
	  Message::error(this->options_.error_stream, "unfinished c comment\n");
	  return false;
	}
    }
}

Token	Lexer::advance_token_2(void)
{
  this->identifier_.clear();
  while (true)
    {
      if (this->eof_p())
	return Token::EOF_;
      const char c(this->peek_and_advance_char());

      switch (c)
	{
	case '\n':
	  break ;
	case ';':
	  return Token::SEMI_COLON;
	case ' ': case '\t':
	  break ;
	case ',':
	  return Token::COMA;
	case '(':
	  return Token::OPEN_PARENT;
	case ')':
	  return Token::CLOSE_PARENT;
	case '{':
	  return Token::OPEN_CURLY_BRACE;
	case '}':
	  return Token::CLOSE_CURLY_BRACE;
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
	case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
	case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
	case 'v': case 'w': case 'x': case 'y': case 'z':
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
	case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
	case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
	case 'V': case 'W': case 'X': case 'Y': case 'Z':
	case '_':
	  return this->parse_word(c);

	case '+':
	  {
	    const char	n(this->peek_char());

	    if (n == '+')
	      mj_unreachable();
	    else if (n == '=')
	      mj_unreachable();
	    else
	      return Token::PLUS;
	  }
	case '-':
	  {
	    const char	n(this->peek_char());

	    if (n == '-')
	      mj_unreachable();
	    else if (n == '=')
	      mj_unreachable();
	    else if (n == '>')
	      mj_unreachable();
	    else
	      return Token::MINUS;
	  }

	case '*':
	  {
	    const char	n(this->peek_char());

	    if (n == '=')
	      mj_unreachable();
	    else
	      return Token::STAR;
	  }

	case '/':
	  {
	    const char	n(this->peek_char());

	    if (n == '=')
	      mj_unreachable();
	    else if (n == '*')
	      {
		if (!this->skip_c_comment())
		  return Token::ERROR;
		break;
	      }
	    else if (n == '/')
	      {
		if (this->options_.c_dialect == C_dialect::C_89)
		  return Token::DIVISION;
		else
		  {
		    if (!this->skip_cpp_comment())
		      return Token::ERROR;
		  }
		break;
	      }
	    else
		return Token::DIVISION;
	  }
	default:
	  mj_unreachable();
	}
    }
}

Token	Lexer::advance_token(void)
{
  this->token_ = advance_token_2();
  return this->token_;
}


const std::string	&Lexer::identifier(void) const
{
  return this->identifier_;
}
