#include "Error.hpp"

void throwError(std::string const &errorMessage)
{
    // TODO: REPLACE WITH PROPER ERROR HANDLING PLEASE
    std::cerr << errorMessage << std::endl;
    exit(EXIT_FAILURE);
}