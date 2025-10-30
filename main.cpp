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
    //    std::string program = "(let ((var1 (array lmao 1 \"hehehe\"))) (exec /bin/echo (if ((== (array lmao 1 \"hehehe\") $var1)) (seq (23) (12) ($var1)) else 23)))";
    std::string program = "(func f1 (a b c) (+ (+ $a $b) $c)) (func f2 () (7)) (exec /bin/echo (:f1 1 (:f2) 3))";
    // std::string program = "(func f1 (a b c) (+ (+ $a $c) (- $b $b))) (func f2 () (7))  (exec echo (:f1 (:f2) 5 3))";
    //  (let ((a 0) (b 3) (c 3)) (exec echo (get a) (get b) (get c))
    std::string::const_iterator it = program.begin();
    try
    {
        State state;
        std::vector<std::unique_ptr<Action>> prog = parseTopLevelDeclarations(it, program.end());
        for (auto const &act : prog)
        {
            act->execute(state);
        }
    }
    catch (ParsingError e)
    {
        std::cerr << "Code error at symbol " << (e.getIterator() - program.begin()) << " :" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
