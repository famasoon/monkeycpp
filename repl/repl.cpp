#include "repl.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include <string>
#include <iomanip>

namespace Repl
{
  const std::string PROMPT = ">> ";
  const std::string EXIT_COMMAND = "exit";
  const std::string HELP_COMMAND = "help";
  const std::string DEBUG_COMMAND = "debug";

  void printHelp(std::ostream &out)
  {
    out << "\nMonkey Programming Language - Help\n";
    out << "==================================\n";
    out << "Commands:\n";
    out << "  help  - Show this help message\n";
    out << "  exit  - Exit the REPL\n";
    out << "  debug - Toggle debug mode\n\n";
    out << "Examples:\n";
    out << "  let x = 5;\n";
    out << "  let add = fn(x, y) { x + y; };\n";
    out << "  add(2, 3);\n";
    out << "  let max = fn(x, y) { if (x > y) { x } else { y } };\n\n";
    out << "Operators:\n";
    out << "  +, -, *, /, ==, !=, <, >, !\n\n";
  }

  void printWelcome(std::ostream &out)
  {
    out << "Monkey Programming Language - REPL\n";
    out << "=================================\n";
    out << "Type 'help' for more information\n";
    out << "Type 'exit' to quit\n\n";
  }

  void printAst(const AST::Node *node, std::ostream &out, int indent = 0)
  {
    std::string indentation(indent * 2, ' ');

    if (auto program = dynamic_cast<const AST::Program *>(node))
    {
      out << indentation << "Program:\n";
      for (const auto &stmt : program->statements)
      {
        printAst(stmt.get(), out, indent + 1);
      }
    }
    else if (auto letStmt = dynamic_cast<const AST::LetStatement *>(node))
    {
      out << indentation << "Let Statement:\n";
      out << indentation << "  Name: " << letStmt->name->value << "\n";
      out << indentation << "  Value:\n";
      printAst(letStmt->value.get(), out, indent + 2);
    }
    else if (auto fnLiteral = dynamic_cast<const AST::FunctionLiteral *>(node))
    {
      out << indentation << "Function:\n";
      out << indentation << "  Parameters: ";
      for (const auto &param : fnLiteral->parameters)
      {
        out << param->value << " ";
      }
      out << "\n";
      out << indentation << "  Body:\n";
      printAst(fnLiteral->body.get(), out, indent + 2);
    }
    else
    {
      out << indentation << node->String() << "\n";
    }
  }

  void Start(std::istream &in, std::ostream &out)
  {
    std::string line;
    bool debugMode = false;

    printWelcome(out);

    while (true)
    {
      out << PROMPT;

      if (!std::getline(in, line))
      {
        return;
      }

      // 空行をスキップ
      if (line.empty())
      {
        continue;
      }

      // コマンドの処理
      if (line == EXIT_COMMAND)
      {
        out << "Goodbye!\n";
        return;
      }
      else if (line == HELP_COMMAND)
      {
        printHelp(out);
        continue;
      }
      else if (line == DEBUG_COMMAND)
      {
        debugMode = !debugMode;
        out << "Debug mode: " << (debugMode ? "ON" : "OFF") << "\n";
        continue;
      }

      // セミコロンが無い場合は自動的に追加
      if (line.back() != ';')
      {
        line += ";";
      }

      Lexer::Lexer lexer(line);
      Parser::Parser parser(lexer);
      parser.setDebugMode(debugMode);
      parser.setDebugOutput(out);

      auto program = parser.ParseProgram();
      if (!program)
      {
        continue;
      }

      const auto &errors = parser.Errors();
      if (!errors.empty())
      {
        for (const auto &err : errors)
        {
          out << "\tparser error: " << err << "\n";
        }
        continue;
      }

      // 通常の出力
      out << "Result: " << program->String() << "\n";

      // デバッグモードの場合はASTも表示
      if (debugMode)
      {
        out << "\nAST Structure:\n";
        printAst(program.get(), out);
        out << "\n";
      }
    }
  }

} // namespace Repl
