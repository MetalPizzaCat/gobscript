#include "Terminal.hpp"

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
    size_t column = errorPos;
    do
    {
        currentSymbolCount += lines[lineId].size();
        if (errorPos < currentSymbolCount)
        {
            break;
        }
        lineId++;
    } while (lineId < lines.size());
    if (lineId > 0)
    {
        column = errorPos - (currentSymbolCount - lines[lineId].size());
    }

    std::cerr << "\033[31mError at line " << lineId + 1 << " column " << column + 1 << ": " << message << "\033[0m" << std::endl;
    if (lineId > 0)
    {
        std::cerr << lineId << ": " << lines[lineId - 1];
    }
    std::string lineIdStr = std::to_string(lineId + 1);
    std::cerr << lineId + 1 << ": " << lines[lineId];
    std::cerr << std::string(((column > 3) ? column - 3 : column) + lineIdStr.size() + 2, ' ') << "\033[31m~~~\033[0m" << std::endl;
    if (lineId + 1 < lines.size())
    {
        std::cerr << lineId + 2 << ": " << lines[lineId + 1];
    }
    std::cerr << std::endl;
}
