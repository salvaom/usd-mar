#include <iostream>
#include <regex>
#include <fstream>

#include <pxr/pxr.h>
#include <pxr/base/arch/env.h>
#include <pxr/base/js/json.h>
#include <functional>
#include "utils.h"


PXR_NAMESPACE_USING_DIRECTIVE


std::string ExpandString(const std::string& value)
{
    _EnvRegexReplacer replacer;
    static std::regex regex("\\$\\{([^}]+)\\}");
    return replacer.Replace(value, regex);
}

_RegexReplacer::_RegexReplacer() {}

std::string _RegexReplacer::Replace(const std::string& value, const std::regex& regex)
{
    std::string result = value;
    std::smatch match;

    while (std::regex_search(result, match, regex)) {
        // NOTE: This is taken out of pxr/base/arch/env.cpp
        // NOTE: g++'s standard library's replace() wants non-const iterators
        //       in violation of the standard.  We work around this by using
        //       indexes.
        const std::string::size_type pos = match[0].first - result.begin();
        const std::string::size_type count = match[0].second - match[0].first;
        result.replace(pos, count, GetReplacement(match[1].str()));
    }

    return result;
}

std::string _EnvRegexReplacer::GetReplacement(const std::string& key)
{
    return ArchGetEnv(key);
}

std::string MapRegexReplacer::GetReplacement(const std::string& key)
{
    auto pos = _mappingPtr->find(key);
    if (pos == _mappingPtr->end()) {
        return std::string();
    }
    else {
        return pos->second;
    }
}
