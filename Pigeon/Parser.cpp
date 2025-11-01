#include "Parser.hpp"
#include <limits>
#include <algorithm>
#include "Function.hpp"
void consumeCharacter(char character, std::string::const_iterator &start, std::string::const_iterator end, std::string const &errorMessage)
{
    if (start == end || *start != character)
    {
        throwParsingError(start, errorMessage);
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
    while (start != end && (*start == ' ' || *start == '\n'))
    {
        start++;
    }
}

std::unique_ptr<GetConstStringAction> parseConstString(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string result = "";
    std::string::const_iterator it = start;
    bool expectedClosingMark = *start == '"';
    if (expectedClosingMark)
    {
        it++;
    }
    bool hitClosingMark = false;
    for (; it != end && *it != ' ' && *it != '(' && *it != ')' && !(expectedClosingMark && *it == '"'); it++)
    {
        result += *it;
    }
    if (expectedClosingMark)
    {
        if (*it != '"')
        {
            throwParsingError(it, "expected closing '\"'");
        }
        else
        {
            it++;
        }
    }
    if (result == "" || (!expectedClosingMark && std::find(Keywords.begin(), Keywords.end(), result) != Keywords.end()))
    {
        return nullptr;
    }
    start = it;
    return std::make_unique<GetConstStringAction>(result);
}
std::optional<std::string> parseVariableName(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::string::const_iterator it = start;
    std::string result = "";
    for (; it != end && (std::isalnum(*it) || *it == '_' || *it == '-'); it++)
    {
        result += *it;
    }
    if (result == "")
    {
        return {};
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
        throwParsingError(start, "Constant number is too large, valid range is " +
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

std::unique_ptr<FunctionDeclarationAction> parseUserFunctionDeclaration(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    if (*start != '(')
    {
        return nullptr;
    }
    std::string::const_iterator it = start;
    it++;
    skipWhitespace(it, end);
    if (!expectString("func", it, end))
    {
        return nullptr;
    }
    it += 4;
    skipWhitespace(it, end);
    std::optional<std::string> name = parseVariableName(it, end);
    if (!name.has_value())
    {
        throwParsingError(it, "Expected function name");
    }
    skipWhitespace(it, end);
    consumeCharacter('(', it, end, "Expected argument block");
    std::vector<std::string> argumentNames;
    while (it != end && *it != ')')
    {
        skipWhitespace(it, end);
        if (std::optional<std::string> argName = parseVariableName(it, end); argName.has_value())
        {
            argumentNames.push_back(argName.value());
        }
        skipWhitespace(it, end);
    }
    consumeCharacter(')', it, end, "Expected ')'");
    skipWhitespace(it, end);
    std::unique_ptr<Action> body = parseFunction(it, end);
    if (body == nullptr)
    {
        throwParsingError(it, "Expected function body");
    }
    skipWhitespace(it, end);
    consumeCharacter(')', it, end, "Expected ')'");
    start = it;
    return std::make_unique<FunctionDeclarationAction>(name.value(), std::move(body), argumentNames);
}

std::unique_ptr<FunctionCallAction> parseUserFunctionCall(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    std::unique_ptr<Action> functionAccess = parseFunction(start, end);
    if (functionAccess == nullptr)
    {
        throwParsingError(start, "Expected function name");
    }
    std::vector<std::unique_ptr<Action>> args = parseArguments(start, end);
    std::unique_ptr<FunctionCallAction> exec = std::make_unique<FunctionCallAction>(std::move(functionAccess), std::move(args));
    skipWhitespace(start, end);
    return exec;
}

std::unique_ptr<Action> parseAction(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    std::string::const_iterator it = start;

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
    else if (expectString("exec", it, end))
    {
        it += 4;
        std::unique_ptr<CommandCallAction> var = parseExplicitCommandCall(it, end);
        start = it;
        return var;
    }
    else if (expectString("call", it, end))
    {
        it += 4;
        std::unique_ptr<FunctionCallAction> var = parseUserFunctionCall(it, end);
        start = it;
        return var;
    }
    else if (expectString("array", it, end))
    {
        it += 5;
        std::unique_ptr<CreateArrayAction> var = parseArrayCreation(it, end);
        start = it;
        return var;
    }
    else if (expectString("seq", it, end))
    {
        it += 3;
        std::unique_ptr<SequenceAction> var = parseSequence(it, end);
        start = it;
        return var;
    }
    else if (expectString("while", it, end))
    {
        it += 5;
        std::unique_ptr<WhileLoopAction> var = parseWhileLoop(it, end);
        start = it;
        return var;
    }
    else if (expectString("for", it, end))
    {
        it += 3;
        std::unique_ptr<ForLoopAction> var = parseForLoop(it, end);
        start = it;
        return var;
    }
    else if (std::unique_ptr<SystemFunctionCallFunction> func = parseSystemFunction(it, end); func != nullptr)
    {
        start = it;
        return func;
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
    return nullptr;
}

std::unique_ptr<Action> parseArgument(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    if (std::unique_ptr<VariableAccessAction> varAcc = parseVariableAccess(start, end); varAcc != nullptr)
    {
        return varAcc;
    }
    if (std::unique_ptr<FunctionAccessAction> funcAcc = parseFunctionAccess(start, end); funcAcc != nullptr)
    {
        return funcAcc;
    }
    else if (std::unique_ptr<GetConstNumberAction> num = parseConstNumber(start, end); num != nullptr)
    {
        return num;
    }
    else if (std::unique_ptr<Action> act = parseAction(start, end); act != nullptr)
    {
        return act;
    }

    else if (std::unique_ptr<Action> getStr = parseConstString(start, end); getStr != nullptr)
    {
        skipWhitespace(start, end);
        return getStr;
    }
    return nullptr;
}

std::unique_ptr<Action> parseFunction(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    if (*start == '(')
    {
        start++;
        skipWhitespace(start, end);
        if (*start == ')')
        {
            start++;
            return std::make_unique<GetConstNumberAction>(0);
        }
        std::unique_ptr<Action> func = parseFunction(start, end);
        skipWhitespace(start, end);
        consumeCharacter(')', start, end, "Expected ')'");
        return func;
    }
    std::unique_ptr<Action> body = parseArgument(start, end);
    if (body == nullptr)
    {
        return nullptr;
    }

    return body;
}

std::unique_ptr<VariableAccessAction> parseVariableAccess(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    if (*start != '$')
    {
        return nullptr;
    }
    start++;
    if (std::optional<std::string> name = parseVariableName(start, end); name.has_value())
    {
        return std::make_unique<VariableAccessAction>(name.value());
    }
    throwParsingError(start, "Expected variable name");
    return nullptr;
}

std::unique_ptr<FunctionAccessAction> parseFunctionAccess(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    if (*start != ':')
    {
        return nullptr;
    }
    start++;
    if (std::optional<std::string> name = parseVariableName(start, end); name.has_value())
    {
        return std::make_unique<FunctionAccessAction>(name.value());
    }
    throwParsingError(start, "Expected function name");
    return nullptr;
}

std::vector<std::unique_ptr<Action>> parseArguments(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::vector<std::unique_ptr<Action>> actions;
    while (start != end && *start != ')')
    {
        actions.push_back(parseFunction(start, end));
    }
    return actions;
}

std::unique_ptr<CommandCallAction> parseCommandCall(std::unique_ptr<Action> commandNameAction, std::string::const_iterator &start, std::string::const_iterator end)
{
    std::string::const_iterator it = start;
    std::vector<std::unique_ptr<Action>> args = parseArguments(it, end);
    std::unique_ptr<CommandCallAction> exec = std::make_unique<CommandCallAction>(std::move(commandNameAction), std::move(args));
    skipWhitespace(it, end);
    start = it;
    return exec;
}

std::unique_ptr<CommandCallAction> parseExplicitCommandCall(std::string::const_iterator &start, std::string::const_iterator end)
{
    skipWhitespace(start, end);
    std::unique_ptr<GetConstStringAction> action = parseConstString(start, end);
    if (action == nullptr)
    {
        throwParsingError(start, "Expected command name");
    }
    return parseCommandCall(std::move(action), start, end);
}

std::unique_ptr<BinaryOperationAction> parseBinaryOperation(Operator op, std::string::const_iterator &start, std::string::const_iterator end)
{
    std::vector<std::unique_ptr<Action>> args = parseArguments(start, end);
    if (args.size() != 2)
    {
        throwParsingError(start, "Operator expected only two arguments");
        return nullptr;
    }
    skipWhitespace(start, end);
    return std::make_unique<BinaryOperationAction>(op, std::move(args));
}

std::unique_ptr<AssignOperationAction> parseBinaryAssignmentOperation(Operator op, std::string::const_iterator &start, std::string::const_iterator end)
{
    skipWhitespace(start, end);
    std::optional<std::string> name = parseVariableName(start, end);
    if (!name.has_value())
    {
        throwParsingError(start, "Expected variable name");
    }
    skipWhitespace(start, end);
    std::unique_ptr<Action> val = parseFunction(start, end);
    if (val == nullptr)
    {
        throwParsingError(start, "Expected value");
    }
    skipWhitespace(start, end);
    return std::make_unique<AssignOperationAction>(op, name.value(), std::move(val));
}

std::unique_ptr<BranchAction> parseBranch(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    std::unique_ptr<Action> condition = parseFunction(start, end);
    skipWhitespace(start, end);
    if (condition == nullptr)
    {
        throwParsingError(start, "Expected condition");
    }
    skipWhitespace(start, end);
    std::unique_ptr<Action> thenBranch = parseFunction(start, end);
    if (thenBranch == nullptr)
    {
        throwParsingError(start, "Expected body");
    }
    skipWhitespace(start, end);
    bool hasElse = false;
    if (expectString("else", start, end))
    {
        hasElse = true;
        start += 4;
    }
    skipWhitespace(start, end);
    std::unique_ptr<Action> elseBranch = parseFunction(start, end);
    if (elseBranch == nullptr && hasElse)
    {
        throwParsingError(start, "Expected else body");
    }
    skipWhitespace(start, end);
    return std::make_unique<BranchAction>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<SequenceAction> parseSequence(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::vector<std::unique_ptr<Action>> acts;

    while (start != end && *start != ')')
    {
        skipWhitespace(start, end);
        std::unique_ptr<Action> act = parseFunction(start, end);
        if (act == nullptr)
        {
            break;
        }
        acts.push_back(std::move(act));
    }
    if (acts.empty())
    {
        return nullptr;
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
        std::optional<std::string> name = parseVariableName(start, end);
        if (!name.has_value())
        {
            throwParsingError(start, "Expected variable name");
        }
        if (variables.count(name.value()))
        {
            throwParsingError(start, "Variable with name " + name.value() + " is already present in this block declaration");
        }
        else if (name == "")
        {
            throwParsingError(start, "Expected variable name");
        }
        skipWhitespace(start, end);
        std::unique_ptr<Action> defaultValue = parseFunction(start, end);
        if (defaultValue == nullptr)
        {
            throwParsingError(start, "Expected default value for variable");
        }
        consumeCharacter(')', start, end, "Expected ')'");
        skipWhitespace(start, end);
        variables[name.value()] = std::move(defaultValue);
    }
    consumeCharacter(')', start, end, "Expected ')'");

    std::unique_ptr<Action> act = parseSequence(start, end);
    if (act == nullptr)
    {
        throwParsingError(start, "Expected body");
    }
    return std::make_unique<VariableBlockAction>(std::move(variables), std::move(act));
}

std::unique_ptr<CreateArrayAction> parseArrayCreation(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    std::vector<std::unique_ptr<Action>> values = parseArguments(start, end);
    skipWhitespace(start, end);
    return std::make_unique<CreateArrayAction>(std::move(values));
}

std::vector<std::unique_ptr<Action>> parseTopLevelDeclarations(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::vector<std::unique_ptr<Action>> acts;

    while (start != end && *start != ')')
    {
        skipWhitespace(start, end);
        if (std::unique_ptr<Action> func = parseUserFunctionDeclaration(start, end); func != nullptr)
        {
            acts.push_back(std::move(func));
        }
        else if (std::unique_ptr<Action> act = parseFunction(start, end); act != nullptr)
        {
            acts.push_back(std::move(act));
        }
        else
        {
            break;
        }
        skipWhitespace(start, end);
    }
    return acts;
}

std::unique_ptr<ForLoopAction> parseForLoop(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    consumeCharacter('(', start, end, "Expected '(' at loop header");
    skipWhitespace(start, end);
    std::unique_ptr<Action> init = parseFunction(start, end);
    if (init == nullptr)
    {
        throwParsingError(start, "Expected init for while loop");
    }
    skipWhitespace(start, end);
    std::unique_ptr<Action> cond = parseFunction(start, end);
    if (cond == nullptr)
    {
        throwParsingError(start, "Expected condition for while loop");
    }
    skipWhitespace(start, end);
    std::unique_ptr<Action> iter = parseFunction(start, end);
    if (iter == nullptr)
    {
        throwParsingError(start, "Expected iteration for while loop");
    }
    skipWhitespace(start, end);
    consumeCharacter(')', start, end, "Expected ')' at loop header");
    skipWhitespace(start, end);
    std::unique_ptr<Action> body = parseFunction(start, end);
    if (body == nullptr)
    {
        throwParsingError(start, "Expected body for while loop");
    }
    return std::make_unique<ForLoopAction>(std::move(init), std::move(cond), std::move(iter), std::move(body));
}

std::unique_ptr<WhileLoopAction> parseWhileLoop(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    skipWhitespace(start, end);
    std::unique_ptr<Action> cond = parseFunction(start, end);
    if (cond == nullptr)
    {
        throwParsingError(start, "Expected condition for while loop");
    }
    skipWhitespace(start, end);
    std::unique_ptr<Action> body = parseFunction(start, end);
    if (body == nullptr)
    {
        throwParsingError(start, "Expected body for while loop");
    }
    return std::make_unique<WhileLoopAction>(std::move(cond), std::move(body));
}

std::unique_ptr<SystemFunctionCallFunction> parseSystemFunction(std::string::const_iterator &start, std::string::const_iterator const &end)
{
    std::optional<StandardFunctionInfo> info = {};
    std::string name;
    for (auto const &func : StandardFunctionIds)
    {
        if (expectString(func.first, start, end))
        {
            info = func.second;
            name = func.first;
            start += func.first.size();
        }
    }
    if (!info.has_value())
    {
        return nullptr;
    }
    skipWhitespace(start, end);
    std::vector<std::unique_ptr<Action>> args = parseArguments(start, end);
    if (args.size() != info.value().argumentCount && info.value().argumentCount != -1)
    {
        throwParsingError(start, "Function '" + name + "' expects " + std::to_string(info.value().argumentCount) + " arguments, but got " + std::to_string(args.size()));
    }
    return std::make_unique<SystemFunctionCallFunction>(info.value().functionId, std::move(args));
}
