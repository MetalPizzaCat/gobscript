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
    //(exec \"/bin/echo\" \"not\" $a)
    // std::string program = "(let ((a (array 1 2 3 4 5))) (((exec /bin/echo $a))) (= a lmao) (if (== $a lmao)  (exec \"/bin/echo\" $a) else (exec \"/bin/echo\" \"not\" $a)))";
    std::string program = "(exec /bin/echo (if ((== (array 1) (array 1))) (seq (23) (12) (19)) else 23))";
    // (let ((a 0) (b 3) (c 3)) (exec echo (get a) (get b) (get c))
    std::string::const_iterator it = program.begin();
    try
    {
        std::unique_ptr<Action> c = parseSequence(it, program.end());

        State state;
        c->execute(state);
    }
    catch (ParsingError e)
    {
        std::cerr << "Code error at symbol " << (e.getIterator() - program.begin()) << " :" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}