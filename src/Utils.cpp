# include "../inc/Utils.hpp"

std::string utils::trimSpaces(std::string s)
{
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos) {
        // The string is entirely whitespace or empty
        return "";
    }
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}


bool utils::fileExists(const std::string &name)
{
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}