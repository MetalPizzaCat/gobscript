#pragma once

#include <string>

namespace GobScriptHelper::Interactive
{
    bool isCompleteCodeString(std::string::const_iterator begin, std::string::const_iterator end);

    /// @brief Run the code in an infinite loop where user can update the single global state by running independent code snippets
    /// @return Supposed exit code. Do note that the interactive mode never returns by itself and exit using "exit" function is handled using unix function
    int runInteractiveMode();
} // namespace GobScriptHelper::Interactive
