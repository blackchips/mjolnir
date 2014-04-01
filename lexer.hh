#ifndef		LEXER_HH_
# define	LEXER_HH_



enum Token
  {
    ERROR,
    EOF_,

    SEMI_COLON,
    COMA,

    OPEN_PARENT,
    CLOSE_PARENT,
    OPEN_CURLY_BRACE,
    CLOSE_CURLY_BRACE,

    PLUS,
    MINUS,
    STAR,
    DIVISION,
    WORD,
    INT,
    VOID,
  };

class FileIt final
{
  File		file;
  size_t	pos;

 public:
  explicit inline FileIt(const File &file_) :
    file(file_), pos(0ul) {}
  bool	eof_p(void) const
  {
    return this->pos == this->file.length();
  }
  char	peek_and_advance(void)
  {
    const char	c(this->peek());

    this->advance();
    return c;
  }
  void	advance(void)
  {
    mj_assert(!this->eof_p());
    this->pos++;
  }
  char	peek(void) const
  {
    return this->file.peek(this->pos);
  }
};

class Lexer final
{
 public:
  explicit inline Lexer(const File &file, Options &opt) :
    token_(Token::ERROR), identifier_(), it_(file), options_(opt) {}

 public:
  Token			peek_token(void) const;
  Token			advance_token(void);
  const std::string	&identifier(void) const;

 private:
  bool			eof_p(void) const;
  char			peek_and_advance_char(void);
  Token			parse_word(const char c);
  void			advance_char(void);
  char			peek_char(void) const;
  Token			string_to_token(void) const;
  Token			advance_token_2(void);
  bool			skip_c_comment(void);
  bool			skip_cpp_comment(void);

 private:
  Token			token_;
  std::string		identifier_;
  FileIt		it_;
  Options		&options_;
};


#endif		// !LEXER_HH_
