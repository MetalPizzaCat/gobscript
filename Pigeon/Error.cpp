#include "Error.hpp"

void throwError(std::string const &errorMessage)
{
    // TODO: REPLACE WITH PROPER ERROR HANDLING PLEASE
    std::cerr << errorMessage << std::endl;
    exit(EXIT_FAILURE);
}

void throwParsingError(std::string::const_iterator const &pos, std::string const &errorMessage)
{
    throw ParsingError(pos, errorMessage);
}

const char *ParsingError::what() const throw()
{
    return m_message.c_str();
}

ParsingError::ParsingError(std::string::const_iterator const &pos, std::string const &msg) : m_it(pos), m_message(msg)
{
}
