#include "repl.hpp"
#include <iostream>
#include <string>

namespace monkey
{

const std::string REPL::PROMPT = ">> ";

void REPL::Start(std::istream &in, std::ostream &out)
{
    std::string line;
    Evaluator evaluator;

    while (true)
    {
        out << PROMPT;
        if (!std::getline(in, line))
        {
            return;
        }

        if (line == "exit" || line == "quit")
        {
            break;
        }

        auto lexer = std::make_unique<Lexer::Lexer>(line);
        Parser::Parser parser(std::move(lexer));

        auto program = parser.ParseProgram();
        if (!program)
        {
            printParserErrors(out, parser.Errors());
            continue;
        }

        if (parser.Errors().size() != 0)
        {
            printParserErrors(out, parser.Errors());
            continue;
        }

        auto evaluated = evaluator.eval(program.get());
        if (evaluated)
        {
            out << evaluated->inspect() << std::endl;
        }

        // 定期的にガベージコレクションを実行
        evaluator.collectGarbage();
    }
}

void REPL::printParserErrors(std::ostream &out, const std::vector<std::string> &errors)
{
    out << "parser errors:\n";
    for (const auto &error : errors)
    {
        out << "\t" << error << "\n";
    }
}

} // namespace monkey
