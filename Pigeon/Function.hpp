#pragma once
#include <memory>
#include <vector>
#include <string>

class Action;

struct Function
{
    explicit Function(Action const* body, std::vector<std::string> const &arguments);

    Action const *body;
    std::vector<std::string> arguments;
};
