#pragma once
#include <string>
#include <iostream>

class ParsingError : public std::exception
{
public:
    const char *what() const throw() override;
    ParsingError(std::string::const_iterator const &pos, std::string const &msg);

    std::string::const_iterator const &getIterator() const { return m_it; }

private:
    std::string::const_iterator m_it;
    std::string m_message;
};

/// @brief Throw general purpose error with given message. Wrapper around whatever error handling system the project uses
/// @param errorMessage Message to display
void throwError(std::string const &errorMessage);

/// @brief Throw error describing position in the code string and error message.  Wrapper around whatever error handling system the project uses
/// @param pos Position in the original code string
/// @param errorMessage Message to display
void throwParsingError(std::string::const_iterator const &pos, std::string const &errorMessage);