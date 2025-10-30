#pragma once
#include <memory>
#include <vector>

class Action;

struct Function
{
    explicit Function(Action const* body, std::vector<std::string> const &arguments);

    Action const *body;
    std::vector<std::string> arguments;
};
