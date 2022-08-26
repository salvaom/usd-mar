#include <iostream>

#include <pxr/base/tf/diagnostic.h>
#include <pxr/base/js/json.h>

#include "usdmar/solvers/format.h"
#include "usdmar/utils.h"
#include "usdmar/debug.h"



FormatterSubSolver::FormatterSubSolver() {}
FormatterSubSolver::~FormatterSubSolver() = default;

// Name
const std::string FormatterSubSolver::GetName() {
	return _name;
}

std::string FormatterSubSolver::Resolve(std::string assetPath)
{
	auto iter = _formatters.begin();
	std::regex templateRegex("\\{(\\d+)\\}");
	std::string result = assetPath;

	// Iterate through all the format regex/template pairs
	while (iter != _formatters.end()) {
		const std::string& regexStr = iter->first;
		const std::string& templateStr = iter->second;

		std::regex regex(regexStr);
		std::smatch matches;
		
		// If we don't have a match, continue to the next one
		if (!std::regex_search(assetPath, matches, regex)) {
			++iter;
			continue;
		}

		// Store here all the matches in the proper order
		std::vector<std::string> matchList;
		for (size_t i = 1; i < matches.size(); ++i) {
			std::string foo = matches[i].str();
			matchList.push_back(foo);
		}

		// Now we look for all the {\d} on the template and
		// replace them with the value
		std::smatch templateMatch;
		std::string replacement;

		result = templateStr;

		while (std::regex_search(result, templateMatch, templateRegex)) {
			const std::string::size_type pos = templateMatch[0].first - result.begin();
			const std::string::size_type count = templateMatch[0].second - templateMatch[0].first;

			// Find the index on the array, default to an empty string
			replacement = "";
			int index = stoi(templateMatch[1].str());
			if (index <= matchList.size()) {
				replacement = matchList[index];
			}
			result.replace(pos, count, replacement);

		}

		return result;
		
	}

	USDMAR_DEBUG("No matches found\n");
	return result;
}

void FormatterSubSolver::ConfigureFromJsObject(const JsObject & object) {

	// The regex/format pairs should be in the form of:
	// "pairs": ["<regex>", "<format>"]
	auto pairsPtr = _GetJsObjectValue<JsArray>(object, "pairs");

	if (pairsPtr == nullptr) {
		return;
	}

	auto& pairs = *pairsPtr;

	std::string errorMessage = "'pairs' must be an array of two strings";

	for (auto& pair : pairs) {

		if (!pair.IsArray()) {
			TF_WARN(errorMessage);
			continue;
		}
		auto& regexPair = pair.GetJsArray();
		if (regexPair.size() != 2) {
			TF_WARN(errorMessage);
			continue;
		}


		if (!regexPair[0].IsString() || !regexPair[1].IsString()) {
			TF_WARN(errorMessage);
			continue;
		}

		std::string regex = regexPair[0].GetString();
		std::string formatTemplate = regexPair[1].GetString();

		_formatters.insert({regex, formatTemplate});

	}

}

