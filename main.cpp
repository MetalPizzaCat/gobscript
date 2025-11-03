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

#include <fstream>
#include <sstream>

#include <filesystem>

/*
(func f1 (a) (print $a $a))
(func action (callback a)
    (let ((i 0))
        (for (() (< $i 10) (+= i 1))
             (call $callback $a)
        )
    )
)
(func even (val) (if (== 0 (% $val 2)) (1) else (0)))
(call :action :f1 3)
(print (filter (array 1 2 3 4 5 6) :even))

*/

/*
(func is_correct_file (path ext)
    (and )
    (== (get_suffix path ) ext)
)
(let ((root_folder "./1997 - Fallout (The Soundtrack)") (dest_folder "./dest") (dir_contents 0))
    (= dir_contents )
)
(print (listdir ./))
*/

/*
(func is_correct_file (path)
    (and
        (is_file $path)
        (== (filename_suffix $path) ".flac")
    )
)
(let (
        (root_folder "./1997 - Fallout (The Soundtrack)/")
        (dest_folder "./dest/")
        (dir_contents 0)
        (i 0)
    )
    (seq
        (= dir_contents (filter (listdir $root_folder) :is_correct_file))
        (print $dir_contents)
        (print (listdir $root_folder))

        (for (() (< $i (len $dir_contents)) (+= i 1))
            (seq
                (print (filename (at $dir_contents $i)))
                (exec ffmpeg
                    -i (+ $root_folder (filename (at $dir_contents $i)))
                    -map_metadata 0
                    -id3v2_version 3
                    (+ (+ $dest_folder (filename_stem (at $dir_contents $i)) ) ".mp3") )
            )
        )
    )
)
*/

/// @brief Create a script state object that can be used for gsh
/// @return
State prepareScriptState()
{
    using namespace GobScriptHelper;
    return State({nativePrintLineFunction,
                  nativeLenFunction,
                  nativeArrayFilter,
                  nativeMapArray,
                  nativeListDirectory,
                  nativeGetFileNameSuffix,
                  nativeIsDirectory,
                  nativeIsFile,
                  nativeAppend,
                  nativeAt,
                  nativeGetFileName,
                  nativeGetFileNameStem});
}

void runInteractiveMode()
{
    std::string codeToRun;
    std::cin >> codeToRun;
}
std::vector<std::string> getLines(std::string const &str)
{
    std::stringstream ss(str);
    std::vector<std::string> lines;
    std::string to;
    while (std::getline(ss, to, '\n'))
    {
        lines.push_back(to + '\n');
    }
    return lines;
}

void displayError(size_t errorPos, std::string const &code, std::string const &message)
{
    std::vector<std::string> lines = getLines(code);
    size_t lineId = 0;
    size_t currentSymbolCount = 0;
    size_t remaining = errorPos;
    for (; lineId < lines.size() && currentSymbolCount < errorPos; lineId++)
    {
        currentSymbolCount += lines[lineId].size();
    }
    std::cerr << "Error at line " << lineId + 1 << " column " << currentSymbolCount - errorPos + 1 << ": " << message << std::endl;
    std::cerr << lines[lineId] << std::endl;
}
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
    return EXIT_SUCCESS;
}
int main(int argc, char **argv)
{
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

    return EXIT_SUCCESS;
}
