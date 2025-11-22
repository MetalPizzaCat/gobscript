#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <regex>
#include <map>
#include <variant>
#include <string.h>
#include <optional>

#include "Pigeon/State.hpp"
#include "Pigeon/Parser.hpp"

#include "GobScriptHelper/StandardFunctions.hpp"
#include "GobScriptHelper/Interactive.hpp"
#include "GobScriptHelper/Terminal.hpp"

#include <fstream>
#include <sstream>

#include <filesystem>




int runFileMode(std::string const &filepath)
{
    using namespace GobScriptHelper;
    if (!std::filesystem::exists(filepath))
    {
        std::cerr << "Unable to open " << filepath << ". File not found" << std::endl;
        return EXIT_FAILURE;
    }
    std::ifstream codeFile(filepath);

    std::string program = std::string{std::istreambuf_iterator<char>(codeFile), std::istreambuf_iterator<char>()};

    try
    {
        State state = prepareScriptState();
        loadString(program)->execute(state);
    }
    catch (ParsingError e)
    {
        displayError(e.getIterator() - program.begin(), program, e.what());
        // std::cerr << "Code error at symbol " << (e.getIterator() - program.begin()) << " :" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (RuntimeError e)
    {
        displayError(e.getIterator() - program.begin(), program, e.what());
        // std::cerr << "Code error at symbol " << (e.getIterator() - program.begin()) << " :" << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


int main(int argc, char **argv)
{
    //return runFileMode("./test.gsh");
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++)
    {
        args.push_back(std::string(argv[i]));
    }
    std::vector<std::string> VersionArgs = {"-v", "--version"};
    std::vector<std::string> HelpArgs = {"-h", "--help"};
    std::vector<std::string> FileArgs = {"-i", "--input"};

    std::vector<std::string>::iterator verIt = std::find_first_of(args.begin(), args.end(), VersionArgs.begin(), VersionArgs.end());
    if (verIt != args.end())
    {
        std::cout << "Goblin Script Helper v" << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_VERSION_PATCH << std::endl;
        return EXIT_SUCCESS;
    }

    verIt = std::find_first_of(args.begin(), args.end(), HelpArgs.begin(), HelpArgs.end());
    if (verIt != args.end())
    {
        std::cout << "Goblin Script Helper v" << APP_VERSION_MAJOR << "." << APP_VERSION_MINOR << "." << APP_VERSION_PATCH << std::endl;
        std::cout << "A simple scripting tool meant to automate tasks using LISP inspired syntax" << std::endl;
        std::cout << "Usage: gsh [-i file | -h | -v]" << std::endl;
        std::cout << "Options" << std::endl;
        std::cout << "-v | --version    : Display version of the interpreter" << std::endl;
        std::cout << "-h | --help       : View help about the interpreter" << std::endl;
        std::cout << "-i | --input      : Run code from file in a given location" << std::endl;
        return EXIT_SUCCESS;
    }

    verIt = std::find_first_of(args.begin(), args.end(), FileArgs.begin(), FileArgs.end());
    if (verIt != args.end())
    {
        if (verIt + 1 == args.end())
        {
            std::cerr << "Missing file path after file flag" << std::endl;
            return EXIT_FAILURE;
        }
        return runFileMode(*(verIt + 1));
    }

    return GobScriptHelper::Interactive::runInteractiveMode();
}
