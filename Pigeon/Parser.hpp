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

std::string parseArgument(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<GetConstStringAction> parseConstString(std::string::const_iterator &start, std::string::const_iterator const &end);

std::optional<Operator> parseOperationType(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<Action> parseAction(std::string::const_iterator &start, std::string::const_iterator const &end);

std::vector<std::unique_ptr<Action>> parseArguments(std::string::const_iterator &start, std::string::const_iterator const &end);

std::unique_ptr<CommandCallAction> parseCommandCall(std::unique_ptr<Action> commandNameAction, std::string::const_iterator &start, std::string::const_iterator end);

std::unique_ptr<BinaryOperationAction> parseBinaryOperation(Operator op, std::string::const_iterator &start, std::string::const_iterator end);

std::unique_ptr<SequenceAction> parseSequence(std::string::const_iterator &start, std::string::const_iterator const &end);
