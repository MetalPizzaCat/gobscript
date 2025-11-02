#pragma once
#include "Error.hpp"
#include <optional>
#include <vector>
#include <memory>
#include <map>
#include "Action.hpp"
#include "Operation.hpp"
#include "Error.hpp"
#include "StandardFunctions.hpp"

/// @brief List of all keywords that should not be confused for strings
static const std::vector<std::string> Keywords = {"if", "else", "exec", "print", "array", "seq", "len"};

struct SpecialCharacter
{
    const char *sequence;
    char character;
};

static const std::vector<SpecialCharacter> SpecialCharacters = {
    SpecialCharacter{.sequence = "\\n", .character = '\n'},
    SpecialCharacter{.sequence = "\\\"", .character = '\"'},
    SpecialCharacter{.sequence = "\\'", .character = '\''},
    SpecialCharacter{.sequence = "\\t", .character = '\t'},
    SpecialCharacter{.sequence = "\\\\", .character = '\\'}};

/// @brief Try character sequence and see if it's any of the special characters such as \\n or \t
/// @param start 
/// @param end 
/// @return 
std::optional<SpecialCharacter> tryParseSpecialCharacter(std::string::const_iterator &start, std::string::const_iterator end);

void consumeCharacter(char character, std::string::const_iterator &start, std::string::const_iterator end, std::string const &errorMessage);

bool expectString(std::string const &expected, std::string::const_iterator start, std::string::const_iterator end);

void skipWhitespace(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Attempt to parse a const string access
/// @param start
/// @param end
/// @return Action that returns constant string on success or nullptr if string is empty or is a reserved keyword
std::unique_ptr<GetConstStringAction> parseConstString(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Parse name of a variable. Variable name can only have letters, digits, underscores and dashes. Name is only considered valid if it can reach separating character
/// @param start
/// @param end
/// @return
std::optional<std::string> parseVariableName(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<GetConstNumberAction> parseConstNumber(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Try to parse a string containing type of the operator, excluding assignment operators
/// @param start
/// @param end
/// @return
std::optional<Operator> parseOperationType(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief  Try to parse a string containing type of the operator, excluding non-assignment operators
/// @param start
/// @param end
/// @return
std::optional<Operator> parseAssignOperationType(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Parse declaration of a function following the `(func () )` approach
/// @param start
/// @param end
/// @return
std::unique_ptr<FunctionDeclarationAction> parseUserFunctionDeclaration(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Parse call to a function following `(call name arg arg arg)`
/// @param start
/// @param end
/// @return
std::unique_ptr<FunctionCallAction> parseUserFunctionCall(std::string::const_iterator &start, std::string::const_iterator const &end);

/**
 * @brief Attempt to parse keyword values, constants and variable access
 *
 * @param start
 * @param end
 * @return std::unique_ptr<Action>
 */
std::unique_ptr<Action> parseAction(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<Action> parseArgument(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief  Parse list of arguments separated by space characters and ending with closing bracket. Bracket will not be consumed
/// @param start
/// @param end
/// @return
std::vector<std::unique_ptr<Action>> parseArguments(std::string::const_iterator &start, std::string::const_iterator const &end);

/**
 * @brief Function is anything contained within `()`. This means that each layer of brackets simply creates a layer of functions that just return their contents
 *
 * @param start
 * @param end
 * @return std::unique_ptr<Action>
 */
std::unique_ptr<Action> parseFunction(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<VariableAccessAction> parseVariableAccess(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<FunctionAccessAction> parseFunctionAccess(std::string::const_iterator &start, std::string::const_iterator const &end);

/**
 * @brief Parse a system call. Written as any other operation but all arguments will be passed to the called program
 *
 * @param commandNameAction Action returning name of the program
 * @param start
 * @param end
 * @return std::unique_ptr<CommandCallAction>
 */
std::unique_ptr<CommandCallAction> parseCommandCall(std::unique_ptr<Action> commandNameAction, std::string::const_iterator &start, std::string::const_iterator end);

/**
 * @brief Parse a system call, unlike `parseCommandCall` this expected `exec` at the start
 *
 * @param commandNameAction Action returning name of the program
 * @param start
 * @param end
 * @return std::unique_ptr<CommandCallAction>
 */
std::unique_ptr<CommandCallAction> parseExplicitCommandCall(std::string::const_iterator &start, std::string::const_iterator end);

/// @brief Parse binary operation that doesn't modify the environment. Always expects two arguments
/// @param op
/// @param start
/// @param end
/// @return
std::unique_ptr<BinaryOperationAction> parseBinaryOperation(Operator op, std::string::const_iterator &start, std::string::const_iterator end);

/// @brief Parse binary operation that modifies the value of a given variable. The first argument is always a variable name
/// @param op
/// @param start
/// @param end
/// @return
std::unique_ptr<AssignOperationAction> parseBinaryAssignmentOperation(Operator op, std::string::const_iterator &start, std::string::const_iterator end);

std::unique_ptr<BranchAction> parseBranch(std::string::const_iterator &start, std::string::const_iterator const &end);

/**
 * @brief Parse a collection of operations one after another, until reaching a closing bracket or end of the string.
 *
 * @param start
 * @param end
 * @return std::unique_ptr<SequenceAction> Parsed sequence or `nullptr` if no actions were parsed
 */
std::unique_ptr<SequenceAction> parseSequence(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<VariableBlockAction> parseVariableBlock(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Parse a sequence of values that will be created like a creation of an array
/// @param start
/// @param end
/// @return
std::unique_ptr<CreateArrayAction> parseArrayCreation(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Special function that will go over all the code and parse function declarations and actions into an array of actions
/// @param start
/// @param end
/// @return
std::vector<std::unique_ptr<Action>> parseTopLevelDeclarations(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<ForLoopAction> parseForLoop(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<WhileLoopAction> parseWhileLoop(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Parse a call to system function
/// @param start
/// @param end
/// @return Action that calls a system function from the state of nullptr if no action is found by name
std::unique_ptr<SystemFunctionCallFunction> parseSystemFunction(std::string::const_iterator &start, std::string::const_iterator const &end);