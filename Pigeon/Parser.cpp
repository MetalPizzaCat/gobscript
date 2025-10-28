#include "Parser.hpp"
#include <limits>
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

std::unique_ptr<GetConstStringAction> parseConstString(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string result = "";
    bool expectedClosingMark = *start == '"';
    if (expectedClosingMark)
    {
        start++;
    }
    bool hitClosingMark = false;
    for (; start != end && *start != ' ' && *start != '(' && *start != ')' && !(expectedClosingMark && *start == '"'); start++)
    {
        result += *start;
    }
    if (expectedClosingMark)
    {
        if (*start != '"')
        {
            throwError("expected closing '\"'");
        }
        else
        {
            start++;
        }
    }
    if (result == "")
    {
        return nullptr;
    }
    return std::make_unique<GetConstStringAction>(result);
}
std::string parseVariableName(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string::const_iterator it = start;
    std::string result = "";
    for (; it != end && (std::isalnum(*it) || *it == '_' || *it == '-'); it++)
    {
        result += *it;
    }
    start = it;
    return result;
}
std::unique_ptr<GetConstNumberAction> parseConstNumber(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    if (!std::isdigit(*start))
    {
        return nullptr;
    }
    std::string num;
    size_t offset = 0;
    while ((start + offset) != end && std::isdigit(*(start + offset)))
    {
        num.push_back(*(start + offset));
        offset++;
    }
    int64_t numVal;
    try
    {
        numVal = std::stol(num);
    }
    catch (std::invalid_argument const &e)
    {
        return nullptr;
    }
    catch (std::out_of_range const &e)
    {
        throwError("Constant number is too large, valid range is " +
                   std::to_string(std::numeric_limits<int32_t>::min()) +
                   "< x < " +
                   std::to_string(std::numeric_limits<int32_t>::max()));
    }
    start += offset;
    return std::make_unique<GetConstNumberAction>(numVal);
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

std::optional<Operator> parseAssignOperationType(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    for (std::pair<const std::string, Operator> const &pair : AssignOperators)
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
    skipWhitespace(start, end);
    std::string::const_iterator it = start;
    if (std::unique_ptr<VariableAccessAction> varAcc = parseVariableAccess(it, end); varAcc != nullptr)
    {
        start = it;
        return varAcc;
    }
    if (*it != '(')
    {
        return nullptr;
    }
    it++;
    if (expectString("if", it, end))
    {
        it += 2;
        std::unique_ptr<BranchAction> branch = parseBranch(it, end);
        start = it;
        return branch;
    }
    else if (expectString("let", it, end))
    {
        it += 3;
        std::unique_ptr<VariableBlockAction> var = parseVariableBlock(it, end);
        start = it;
        return var;
    }
    // check if any preexisting action
    else if (std::optional<Operator> op = parseOperationType(it, end); op.has_value())
    {
        std::unique_ptr<BinaryOperationAction> binOp = parseBinaryOperation(op.value(), it, end);
        start = it;
        // call the op parser
        return binOp;
    }
    else if (std::optional<Operator> op = parseAssignOperationType(it, end); op.has_value())
    {
        std::unique_ptr<AssignOperationAction> binOp = parseBinaryAssignmentOperation(op.value(), it, end);
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

std::unique_ptr<Action> parseArgument(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    if (std::unique_ptr<Action> act = parseAction(start, end); act != nullptr)
    {
        return act;
    }
    else if (std::unique_ptr<GetConstNumberAction> num = parseConstNumber(start, end); num != nullptr)
    {
        return std::move(num);
    }
    else
    {
        return parseConstString(start, end);
    }
    skipWhitespace(start, end);
}

std::unique_ptr<VariableAccessAction> parseVariableAccess(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    if (*start != '$')
    {
        return nullptr;
    }
    start++;
    std::string name = parseVariableName(start, end);
    return std::make_unique<VariableAccessAction>(name);
}

std::vector<std::unique_ptr<Action>> parseArguments(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::vector<std::unique_ptr<Action>> actions;
    while (start != end && *start != ')')
    {
        actions.push_back(parseArgument(start, end));
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

std::unique_ptr<AssignOperationAction> parseBinaryAssignmentOperation(Operator op, std::string::const_iterator &start, std::string::const_iterator end)
{
    skipWhitespace(start, end);
    std::string name = parseVariableName(start, end);
    skipWhitespace(start, end);
    std::unique_ptr<Action> val = parseArgument(start, end);
    if (val == nullptr)
    {
        throwError("Expected value");
    }
    skipWhitespace(start, end);
    consumeCharacter(')', start, end, "Expected ')'");
    return std::make_unique<AssignOperationAction>(op, name, std::move(val));
}

std::unique_ptr<BranchAction> parseBranch(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    std::unique_ptr<Action> condition = parseArgument(start, end);
    skipWhitespace(start, end);
    if (condition == nullptr)
    {
        throwError("Expected condition");
    }
    skipWhitespace(start, end);
    std::unique_ptr<SequenceAction> thenBranch = parseSequence(start, end);
    skipWhitespace(start, end);
    if (expectString("else", start, end))
    {
        start += 4;
    }
    skipWhitespace(start, end);
    std::unique_ptr<SequenceAction> elseBranch = parseSequence(start, end);
    skipWhitespace(start, end);
    consumeCharacter(')', start, end, "Expected ')'");
    return std::make_unique<BranchAction>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
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

std::unique_ptr<VariableBlockAction> parseVariableBlock(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    std::map<std::string, std::unique_ptr<Action>> variables;
    consumeCharacter('(', start, end, "Expected '('");
    while (start != end && *start != ')')
    {
        skipWhitespace(start, end);
        consumeCharacter('(', start, end, "Expected '('");
        skipWhitespace(start, end);
        std::string name = parseVariableName(start, end);
        if (variables.count(name))
        {
            throwError("Variable with name " + name + " is already present in this block declaration");
        }
        else if (name == "")
        {
            throwError("Expected variable name");
        }
        skipWhitespace(start, end);
        std::unique_ptr<Action> defaultValue = parseArgument(start, end);
        if (defaultValue == nullptr)
        {
            throwError("Expected default value for variable");
        }
        consumeCharacter(')', start, end, "Expected ')'");
        skipWhitespace(start, end);
        variables[name] = std::move(defaultValue);
    }
    consumeCharacter(')', start, end, "Expected ')'");

    std::unique_ptr<Action> act = parseSequence(start, end);
    if (act == nullptr)
    {
        throwError("Expected body");
    }
    consumeCharacter(')', start, end, "Expected ')'");
    return std::make_unique<VariableBlockAction>(std::move(variables), std::move(act));
}
