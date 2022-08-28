#include <iostream>
#include <regex>
#include <fstream>

#include <pxr/pxr.h>
#include <pxr/base/arch/env.h>
#include <pxr/base/js/json.h>
#include <functional>


PXR_NAMESPACE_USING_DIRECTIVE


std::string RegexReplace(const std::string& value, const std::regex& regex, std::function<std::string(const std::string&)> func) {
    std::string result = value;
    std::smatch match;

    while (std::regex_search(result, match, regex)) {
        // NOTE: This is taken out of pxr/base/arch/env.cpp
        // NOTE: g++'s standard library's replace() wants non-const iterators
        //       in violation of the standard.  We work around this by using
        //       indexes.
        const std::string::size_type pos = match[0].first - result.begin();
        const std::string::size_type count = match[0].second - match[0].first;
        result.replace(pos, count, func(match[1].str()));
    }

    return result;
}

std::string ExpandString(const std::string& value)
{
    static std::regex regex("\\$\\{([^}]+)\\}");
    return RegexReplace(value, regex, ArchGetEnv);
}

std::string JoinString(const std::string& sep, const std::vector<std::string>& strings) {
    std::string result;

    for (auto& string : strings) {
        result += string + sep;
    }

    if (result != "") {
        result.erase(result.end() - 1);
    }

    return result;
    
}