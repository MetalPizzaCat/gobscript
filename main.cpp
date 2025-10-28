#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <regex>
#include <map>
#include <variant>
#include <string.h>
#include <optional>

#include "Pigeon/State.hpp"
#include "Pigeon/Parser.hpp"

int main(int, char **)
{
    std::string currentPath = "./";
    // (program arg1 arg2 arg3)
    std::string program = "(let ((var1 1)) (/bin/echo $var1) (= var1 8) (/bin/echo $var1))";
    // (let ((a 0) (b 3) (c 3)) (exec echo (get a) (get b) (get c))
    std::string::const_iterator it = program.begin();
    std::unique_ptr<Action> c = parseSequence(it, program.end());
    State state;
    c->execute(state);
    return EXIT_SUCCESS;
}
