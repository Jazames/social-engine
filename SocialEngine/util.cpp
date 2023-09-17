#include "util.h"

#include <map>
#include <regex>

std::string sanitize_naughty_words(const std::string& str)
{
    std::map<std::string, std::string> replacements{
        {"cock", "cactus"},
        {"dick", "cactus"},
        {"penis", "cactus"},
        {"wang", "cactus"},
        {"crotch", "cactus"},
        {"groin", "cactus"},
//        {"balls", "nuts"},
        {"testicles", "sack"},
        {"gay", "dumb"},
        {"homo", "dumb"},
        {"homosexual", "dumb"},
        {"lesbian", "dumb"},
        {"fuck", "screw"},
        {"retarded", "dumb"},
        {"bitch", "dog"}
    };

    std::string result = str;
    for (const auto& pair : replacements) {
        std::string pattern = "\\b" + pair.first + "\\b";
        std::regex r(pattern);
        result = std::regex_replace(result, r, pair.second);
    }

    return result;
}