#include "repl/repl.hpp"
#include <iostream>

int main()
{
    std::cout << "Hello! This is the Monkey programming language!" << std::endl;
    std::cout << "Feel free to type in commands" << std::endl;

    monkey::REPL::Start(std::cin, std::cout);

    return 0;
}
