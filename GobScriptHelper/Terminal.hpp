#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

std::vector<std::string> getLines(std::string const &str);

void displayError(size_t errorPos, std::string const &code, std::string const &message);