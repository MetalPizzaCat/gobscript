#include "Parser.hpp"

void consumeCharacter(char character, std::string::const_iterator &start, std::string::const_iterator end, std::string const &errorMessage)
{
    if (start == end || *start != character)
    {
        throwError(errorMessage);
    }
    start++;
}

bool expectString(std::string const &expected, std::string::const_iterator start, std::string::const_iterator end)
{
    for (size_t i = 0; i < expected.size(); i++)
    {
        if (start + i == end || expected[i] != *(start + i))
        {
            return false;
        }
    }
    // make sure there is a separator afterwards
    if ((start + expected.size()) != end)
    {
        char c = *(start + expected.size());
        if (c != ' ' && c != ')')
        {
            return false;
        }
    }
    return true;
}

void skipWhitespace(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    while (start != end && *start == ' ')
    {
        start++;
    }
}

std::string parseArgument(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string result = "";
    while (start != end && (std::isalnum(*start) || *start == '-' || *start == '='))
    {
        result += *start;
        start++;
    }
    return result;
}

std::unique_ptr<GetConstStringAction> parseConstString(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string result = "";
    for (; start != end && *start != ' ' && *start != '(' && *start != ')'; start++)
    {
        result += *start;
    }
    if (result == "")
    {
        return nullptr;
    }
    return std::make_unique<GetConstStringAction>(result);
}
std::optional<Operator> parseOperationType(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    for (std::pair<const std::string, Operator> const &pair : Operators)
    {
        if (expectString(pair.first, start, end))
        {
            start += pair.first.size();
            return pair.second;
        }
    }
    return {};
}

std::unique_ptr<Action> parseAction(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string::const_iterator it = start;
    if (*it != '(')
    {
        return nullptr;
    }
    it++;
    // check if any preexisting action
    if (std::optional<Operator> op = parseOperationType(it, end); op.has_value())
    {
        std::unique_ptr<BinaryOperationAction> binOp = parseBinaryOperation(op.value(), it, end);
        start = it;
        // call the op parser
        return binOp;
    }
    else if (std::unique_ptr<GetConstStringAction> action = parseConstString(it, end); action != nullptr)
    {
        std::unique_ptr<CommandCallAction> exec = parseCommandCall(std::move(action), it, end);
        start = it;
        return exec;
    }
    else
    {
        throwError("Expected an action or a constant");
    }
    return nullptr;
}

std::vector<std::unique_ptr<Action>> parseArguments(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::vector<std::unique_ptr<Action>> actions;
    while (start != end && *start != ')')
    {
        skipWhitespace(start, end);
        if (std::unique_ptr<Action> act = parseAction(start, end); act != nullptr)
        {
            actions.push_back(std::move(act));
        }
        else
        {

            actions.push_back(parseConstString(start, end));
        }
        skipWhitespace(start, end);
    }
    return actions;
}

std::unique_ptr<CommandCallAction> parseCommandCall(std::unique_ptr<Action> commandNameAction, std::string::const_iterator &start, std::string::const_iterator end)
{
    std::string::const_iterator it = start;
    std::vector<std::unique_ptr<Action>> args = parseArguments(it, end);
    std::unique_ptr<CommandCallAction> exec = std::make_unique<CommandCallAction>(std::move(commandNameAction), std::move(args));
    skipWhitespace(it, end);
    consumeCharacter(')', it, end, "Expected ')'");
    start = it;
    return exec;
}

std::unique_ptr<BinaryOperationAction> parseBinaryOperation(Operator op, std::string::const_iterator &start, std::string::const_iterator end)
{
    std::vector<std::unique_ptr<Action>> args = parseArguments(start, end);
    if (args.size() != 2)
    {
        throwError("Operator expected only two arguments");
        return nullptr;
    }
    skipWhitespace(start, end);
    consumeCharacter(')', start, end, "Expected ')'");
    return std::make_unique<BinaryOperationAction>(op, std::move(args));
}

std::unique_ptr<SequenceAction> parseSequence(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::vector<std::unique_ptr<Action>> acts;

    while (start != end && *start != ')')
    {
        skipWhitespace(start, end);
        std::unique_ptr<Action> act = parseAction(start, end);
        if (act == nullptr)
        {
            break;
        }
        acts.push_back(std::move(act));
    }
    return std::make_unique<SequenceAction>(std::move(acts));
}