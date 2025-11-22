#include "Interactive.hpp"
#include <iostream>
#include "../Pigeon/State.hpp"
#include "../Pigeon/Parser.hpp"
#include "StandardFunctions.hpp"
#include "Terminal.hpp"

bool GobScriptHelper::Interactive::isCompleteCodeString(std::string::const_iterator begin, std::string::const_iterator end)
{
    int bracketCount = 0;
    bool isInString = false;
    for (std::string::const_iterator it = begin; it != end; it++)
    {
        if (!isInString)
        {
            if (*it == '(')
            {
                bracketCount++;
            }
            else if (*it == ')')
            {
                bracketCount--;
            }
        }
        if (*it == '\\' && it + 1 != end && *(it + 1) == '"')
        {
            it++;
        }
        else if (*it == '"')
        {
            isInString = !isInString;
        }
    }
    return bracketCount == 0;
}

int GobScriptHelper::Interactive::runInteractiveMode()
{
    std::cout << "Goblin Script Helper v" << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_VERSION_PATCH << std::endl;
    std::cout << "Interactive mode" << std::endl;

    using namespace Pigeon;
    State state = prepareScriptState();
    std::string program;
    // only an "exit" function can break this

    while (true)
    {
        try
        {
            std::cout << ">>";
            // get input string
            do
            {
                std::string temp;
                std::cout << "> ";
                std::getline(std::cin, temp);
                program += temp;
            } while (!isCompleteCodeString(program.begin(), program.end()));
            program += '\n';
            loadString(program)->execute(state);
            std::cout << std::endl;
        }
        // we don't exit on error, because we are supposed to run the code forever
        // this is more so to mimic how python one works
        catch (ParsingError e)
        {
            displayError(e.getIterator() - program.begin(), program, e.what());
        }
        catch (RuntimeError e)
        {
            displayError(e.getIterator() - program.begin(), program, e.what());
        }
        program.clear();
    }
    return EXIT_SUCCESS;
}
