#include "repl.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include <iostream>

namespace REPL
{

REPL::REPL() : evaluator(std::make_unique<monkey::Evaluator>()), 
               jit(std::make_unique<JIT::Compiler>()), 
               useJIT(false)
{
}

void REPL::Start()
{
    std::cout << "Monkey Programming Language\n";
    std::cout << "Type 'jit' to toggle JIT compilation (currently " 
              << (useJIT ? "enabled" : "disabled") << ")\n";
    std::cout << "Type 'exit' to exit\n";

    std::string line;
    while (true)
    {
        std::cout << ">> ";
        if (!std::getline(std::cin, line))
            break;

        if (line == "exit")
            break;

        if (line == "jit")
        {
            toggleJIT();
            continue;
        }

        auto lexer = std::make_unique<Lexer::Lexer>(line);
        Parser::Parser parser(std::move(lexer));

        auto program = parser.ParseProgram();
        if (!program || !parser.Errors().empty())
        {
            printParserErrors(parser.Errors());
            continue;
        }

        if (useJIT)
        {
            executeWithJIT(*program);
        }
        else
        {
            executeWithInterpreter(*program);
        }
    }
}

void REPL::toggleJIT()
{
    useJIT = !useJIT;
    std::cout << "JIT compilation " << (useJIT ? "enabled" : "disabled") << "\n";
}

void REPL::executeWithJIT(const AST::Program& program)
{
    try
    {
        jit->compile(program);
        std::cout << "Generated LLVM IR:\n" << jit->getIR() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cout << "JIT compilation failed: " << e.what() << std::endl;
    }
}

void REPL::executeWithInterpreter(const AST::Program& program)
{
    auto evaluated = evaluator->eval(&program);
    if (evaluated)
    {
        std::cout << evaluated->inspect() << std::endl;
    }
}

void REPL::printParserErrors(const std::vector<std::string>& errors)
{
    std::cout << "Parser errors:\n";
    for (const auto& error : errors)
    {
        std::cout << "\t" << error << "\n";
    }
}

} // namespace REPL
