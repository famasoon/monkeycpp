#include "repl.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include <string>

namespace Repl
{
  const std::string PROMPT = ">> ";
  const std::string EXIT_COMMAND = "exit";

  void Start(std::istream &in, std::ostream &out)
  {
    std::string line;

    out << "Type '" << EXIT_COMMAND << "' to exit\n";

    while (true)
    {
      out << PROMPT;

      if (!std::getline(in, line))
      {
        return;
      }

      // exitコマンドのチェック
      if (line == EXIT_COMMAND)
      {
        out << "Goodbye!\n";
        return;
      }

      // 空行をスキップ
      if (line.empty())
      {
        continue;
      }

      // セミコロンが無い場合は自動的に追加
      if (line.back() != ';')
      {
        line += ";";
      }

      Lexer::Lexer lexer(line);
      Parser::Parser parser(lexer);

      auto program = parser.ParseProgram();
      if (!program)
      {
        continue;
      }

      const auto& errors = parser.Errors();
      if (!errors.empty())
      {
        // exitコマンドに関するエラーは表示しない
        if (line == EXIT_COMMAND + ";")
        {
          out << "Goodbye!\n";
          return;
        }

        for (const auto& err : errors)
        {
          out << "\tparser error: " << err << "\n";
        }
        continue;
      }

      out << program->String() << "\n";
    }
  }

} // namespace Repl
