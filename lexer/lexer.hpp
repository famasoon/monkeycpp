#include <string>
namespace lexer
{
  class Lexer
  {
    std::string input;
    int position;
    int readPosition;
    char ch;

    Lexer(std::string input) : input(input), position(0), readPosition(0), ch('\0') {}

    inline char NextChar()
    {
      if (readPosition >= input.length())
      {
        return '\0';
      }
      return input[readPosition];
    }

    inline std::string ReadIdentifier()
    {
      int pos = position;
      while (isalpha(ch) || ch == '_')
      {
        NextChar();
      }
      return input.substr(pos, position - pos);
    }

    inline std::string ReadNumber()
    {
      int pos = position;
      while (isdigit(ch))
      {
        NextChar();
      }
      return input.substr(pos, position - pos);
    }

    inline std::string PeekChar()
    {
      if (readPosition >= input.length())
      {
        return std::string(1, '\0');
      }
      return std::string(1, input[readPosition]);
    }

    inline void SkipWhitespace()
    {
      while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
      {
        NextChar();
      }
    }

    inline std::string ReadString()
    {
      int pos = position;
      NextChar(); // skip opening quote

      while (ch != '"' && ch != '\0')
      {
        NextChar();
      }

      if (ch == '"')
      {
        NextChar(); // skip closing quote
      }

      return input.substr(pos + 1, position - pos - 2);
    }

    inline std::string ReadChar()
    {
      std::string str = std::string(1, ch);
      NextChar();
      return str;
    }

  public:
    Lexer(std::string input);
    char NextChar();
    std::string ReadIdentifier();
    std::string ReadNumber();
    std::string PeekChar();

    void SkipWhitespace();
    std::string ReadString();
    std::string ReadChar();
  };
}
