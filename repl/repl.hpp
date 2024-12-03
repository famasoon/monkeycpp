#pragma once
#include "../evaluator/evaluator.hpp"
#include "../jit/jit.hpp"
#include <memory>
#include <string>

namespace REPL
{

class REPL
{
private:
    std::unique_ptr<monkey::Evaluator> evaluator;
    std::unique_ptr<JIT::Compiler> jit;
    bool useJIT;

public:
    REPL();
    void Start();
    void toggleJIT();
    bool isJITEnabled() const { return useJIT; }

private:
    void printParserErrors(const std::vector<std::string>& errors);
    void executeWithJIT(const AST::Program& program);
    void executeWithInterpreter(const AST::Program& program);
};

} // namespace REPL
