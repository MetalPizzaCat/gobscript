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



   /**
     * @brief Static array containing info about all operators used in the Codegen
     *
     */
    // static const std::vector<OperatorData> Operators = {
    //     // logic operators
    //     OperatorData{.symbol = "==", .op = Operator::Equals, .priority = 5, .operation = Operation::Equals},
    //     OperatorData{.symbol = ">=", .op = Operator::LessEq, .priority = 5, .operation = Operation::MoreOrEq},
    //     OperatorData{.symbol = "<=", .op = Operator::MoreEq, .priority = 5, .operation = Operation::LessOrEq},
    //     // assignment
    //     OperatorData{.symbol = "=", .op = Operator::Assign, .priority = 1, .operation = Operation::Set},
    //     // bool operators
    //     OperatorData{.symbol = "!=", .op = Operator::NotEqual, .priority = 5, .operation = Operation::NotEq},
    //     OperatorData{.symbol = "!", .op = Operator::Not, .priority = 5, .operation = Operation::Not},
    //     OperatorData{.symbol = ">>=", .op = Operator::BitRightShiftAssign, .priority = 1, .operation = Operation::ShiftRight},
    //     OperatorData{.symbol = "<<=", .op = Operator::BitLeftShiftAssign, .priority = 1, .operation = Operation::ShiftLeft},
    //     OperatorData{.symbol = ">>", .op = Operator::BitRightShift, .priority = 2, .operation = Operation::ShiftRight},
    //     OperatorData{.symbol = "<<", .op = Operator::BitLeftShift, .priority = 2, .operation = Operation::ShiftLeft},
    //     OperatorData{.symbol = "<", .op = Operator::Less, .priority = 5, .operation = Operation::Less},
    //     OperatorData{.symbol = ">", .op = Operator::More, .priority = 5, .operation = Operation::More},
    //     // math
    //     OperatorData{.symbol = "+=", .op = Operator::AddAssign, .priority = 1, .operation = Operation::Add},
    //     OperatorData{.symbol = "-=", .op = Operator::SubAssign, .priority = 1, .operation = Operation::Sub},
    //     OperatorData{.symbol = "*=", .op = Operator::MulAssign, .priority = 1, .operation = Operation::Mul},
    //     OperatorData{.symbol = "/=", .op = Operator::DivAssign, .priority = 1, .operation = Operation::Div},
    //     OperatorData{.symbol = "+", .op = Operator::Add, .priority = 6, .operation = Operation::Add},
    //     OperatorData{.symbol = "-", .op = Operator::Sub, .priority = 6, .operation = Operation::Sub},
    //     OperatorData{.symbol = "*", .op = Operator::Mul, .priority = 7, .operation = Operation::Mul},
    //     OperatorData{.symbol = "/", .op = Operator::Div, .priority = 7, .operation = Operation::Div},
    //     // boolean operators
    //     OperatorData{.symbol = "&&", .op = Operator::And, .priority = 4, .operation = Operation::And},
    //     OperatorData{.symbol = "||", .op = Operator::Or, .priority = 3, .operation = Operation::Or},
    //     OperatorData{.symbol = "and", .op = Operator::And, .priority = 4, .operation = Operation::And},
    //     OperatorData{.symbol = "or", .op = Operator::Or, .priority = 3, .operation = Operation::Or},
    //     // bit math
    //     OperatorData{.symbol = "&=", .op = Operator::BitAndAssign, .priority = 1, .operation = Operation::BitAnd},
    //     OperatorData{.symbol = "~=", .op = Operator::BitNotAssign, .priority = 1, .operation = Operation::BitNot},
    //     OperatorData{.symbol = "^=", .op = Operator::BitXorAssign, .priority = 1, .operation = Operation::BitXor},
    //     OperatorData{.symbol = "%=", .op = Operator::ModuloAssign, .priority = 1, .operation = Operation::Modulo},
    //     OperatorData{.symbol = "|", .op = Operator::BitOr, .priority = 2, .operation = Operation::BitOr},
    //     OperatorData{.symbol = "&", .op = Operator::BitAnd, .priority = 2, .operation = Operation::BitAnd},
    //     OperatorData{.symbol = "~", .op = Operator::BitNot, .priority = 2, .operation = Operation::BitNot},
    //     OperatorData{.symbol = "^", .op = Operator::BitXor, .priority = 2, .operation = Operation::BitXor},
    //     OperatorData{.symbol = "%", .op = Operator::Modulo, .priority = 6, .operation = Operation::Modulo},
    // };
int main(int, char **)
{
    std::string currentPath = "./";
    // (program arg1 arg2 arg3)
    std::string program = "(/bin/echo 1) (/bin/echo 2 ) (/bin/echo banana)";
    // (let ((a 0) (b 3) (c 3)) (exec echo (get a) (get b) (get c))
    std::string::const_iterator it = program.begin();
    std::unique_ptr<Action> c = parseSequence(it, program.end());
    State state;
    c->execute(state);
    return EXIT_SUCCESS;
}
