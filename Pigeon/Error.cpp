#include "Error.hpp"

void throwRuntimeError(std::string::const_iterator const &pos, std::string const &errorMessage)
{
    throw RuntimeError(pos, errorMessage);
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

const char *RuntimeActionExecutionError::what() const throw()
{
    return m_message.c_str();
}

RuntimeActionExecutionError::RuntimeActionExecutionError(std::string const &msg) : m_message(msg)
{
}

const char *RuntimeError::what() const throw()
{
    return m_message.c_str();
}

RuntimeError::RuntimeError(std::string::const_iterator const &pos, std::string const &msg) : m_it(pos), m_message(msg)
{
}
