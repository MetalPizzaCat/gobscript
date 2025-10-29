#pragma once
#include "Error.hpp"
#include <optional>
#include <vector>
#include <memory>
#include "Action.hpp"
#include "Operation.hpp"
#include "Error.hpp"

void consumeCharacter(char character, std::string::const_iterator &start, std::string::const_iterator end, std::string const &errorMessage);

bool expectString(std::string const &expected, std::string::const_iterator start, std::string::const_iterator end);

void skipWhitespace(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<GetConstStringAction> parseConstString(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Parse name of a variable. Variable name can only have letters, digits, underscores and dashes. Name is only considered valid if it can reach separating character
/// @param start 
/// @param end 
/// @return 
std::string parseVariableName(std::string::const_iterator &start, std::string::const_iterator const &end);

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

/**
 * @brief Attempt to parse keyword values, constants and variable access
 * 
 * @param start 
 * @param end 
 * @return std::unique_ptr<Action> 
 */
std::unique_ptr<Action> parseAction(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<Action> parseArgument(std::string::const_iterator &start, std::string::const_iterator const &end);

/**
 * @brief Function is anything contained within `()`. This means that each layer of brackets simply creates a layer of functions that just return their contents
 * 
 * @param start 
 * @param end 
 * @return std::unique_ptr<Action> 
 */
std::unique_ptr<Action> parseFunction(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<VariableAccessAction> parseVariableAccess(std::string::const_iterator &start, std::string::const_iterator const &end);

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
std::unique_ptr<CommandCallAction> parseExplicitCommandCall( std::string::const_iterator &start, std::string::const_iterator end);


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
 * @brief Parse a collection of operations one after another, until reaching a closing bracket
 *
 * @param start
 * @param end
 * @return std::unique_ptr<SequenceAction>
 */
std::unique_ptr<SequenceAction> parseSequence(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<VariableBlockAction> parseVariableBlock(std::string::const_iterator &start, std::string::const_iterator const &end);

/// @brief Parse a sequence of values that will be created like a creation of an array
/// @param start 
/// @param end 
/// @return 
std::unique_ptr<CreateArrayAction> parseArrayCreation(std::string::const_iterator &start, std::string::const_iterator const &end);