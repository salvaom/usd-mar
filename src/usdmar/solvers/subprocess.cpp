#include <iostream>

#include <pxr/base/tf/fileUtils.h>
#include <pxr/base/tf/pathUtils.h>
#include <pxr/base/tf/diagnosticLite.h>
#include <pxr/base/tf/stringUtils.h>

#include "usdmar/solvers/subprocess.h"
#include "usdmar/utils.h"
#include "usdmar/debug.h"
#include <subprocess.h>


SubprocessSubSolver::SubprocessSubSolver() {}
SubprocessSubSolver::~SubprocessSubSolver() = default;

const std::string SubprocessSubSolver::GetName() {	return _name;}

std::string SubprocessSubSolver::Resolve(std::string assetPath)
{
	// Create the replacement mapping. This will replace "$assetPath" with the input
	// asset path and "$token" with the current communicate token
	auto replacementMap = std::make_shared<dict>(dict{ {"assetPath", assetPath}, {"token", _communicateToken} });
	auto replacer = MapRegexReplacer(std::shared_ptr<dict>(replacementMap));
	std::regex argRegex("\\$(assetPath|token)");

	// Create a single vector with the full command for convenience
	std::vector<std::string> fullCommand;
	std::string executable = _executable;

#ifdef _WIN32
	executable += _windowsSuffix;
#endif // _WIN32

	fullCommand.push_back(ExpandString(executable));
	for (const auto& arg : _arguments) {
		// Replace the $assetPath, $token, then expand environment variables
		std::string value = replacer.Replace(arg, argRegex);
		fullCommand.push_back(ExpandString(value));
	}

	// Convert the vector to a char array to pass to subprocess.h
	std::vector<const char*> command;
	for (const std::string& s : fullCommand) {
		command.push_back(s.data());
	}
	command.push_back(nullptr);
	const char** commandArray = command.data();
	
	// Some more conveniences
	auto fullCommandStr = TfStringJoin(fullCommand, " ");
	USDMAR_DEBUG("[%s] Running command: %s\n", TF_FUNC_NAME().c_str(), fullCommandStr.c_str());
	
	// Create the actual subprocess
	struct subprocess_s subprocess;
	int proc_create_result = subprocess_create(
		commandArray,
		subprocess_option_inherit_environment 
		| subprocess_option_combined_stdout_stderr
		| subprocess_option_no_window,
		&subprocess
	);

	// Check if we could actually start the process
	if (proc_create_result != 0) {
		TF_WARN("Failed to launch process: %s", fullCommandStr.c_str());
		return assetPath;
	}

	// Wait for the process to finish
	int process_return;
	int proc_join_result = subprocess_join(&subprocess, &process_return);

	// Check if we could actually wait for the process
	if (proc_create_result != 0) {
		TF_WARN("Failed to wait for process: %s", fullCommandStr.c_str());
		return assetPath;
	}

	// Convert the stdout (which includes the stderr) to a nice string
	std::string output;
	FILE* p_stdout = subprocess_stdout(&subprocess);
	char c;
	while ((c = fgetc(p_stdout)) != EOF) {
		output.append(1, c);
	}

	// If we were able to launch and join the process, check if it finished successfully
	if (process_return != 0) {
		TF_WARN("Subprocess '%s' failed with code %d:\n%s\n", fullCommandStr.c_str(), process_return, output.c_str());
	}

	// Now we can get to parsing. We can look for any "<newline_or_beginning><token><result><newline_or_end>" pattern
	std::string _regex = "(?:^|\\r?\\n)" + _communicateToken + "(.+)(?:$|\\r?\\n)";
	std::regex communicateRegex(_regex);
	std::smatch match;
	auto hasMatch = std::regex_search(output, match, communicateRegex);
	if (!hasMatch) {
		TF_WARN("Subprocess '%s' did not produce expected output matching regex %s:\n%s\n", fullCommandStr.c_str(), _regex.c_str(), output.c_str());
		return assetPath;
	}
	return match[1].str();
	
}
void SubprocessSubSolver::ConfigureFromJsObject(const JsObject & object) {
	auto executablePtr = _GetJsObjectValue<std::string>(object, "executable");
	auto argumentsPtr = _GetJsObjectValue<JsArray>(object, "arguments");
	auto windowsSuffixPtr = _GetJsObjectValue<std::string>(object, "windowsSuffix");
	auto tokenPtr = _GetJsObjectValue<std::string>(object, "token");

	if (executablePtr != nullptr) {
		_executable = *executablePtr;
	}
	if (windowsSuffixPtr != nullptr) {
		_windowsSuffix = *windowsSuffixPtr;
	}
	if (tokenPtr != nullptr) {
		_communicateToken = *tokenPtr;
	}
	if (argumentsPtr != nullptr) {
		_arguments.clear();
		for (auto& arg : *argumentsPtr) {
			if (!arg.IsString()) {
				TF_WARN("Arguments must be strings");
				continue;
			}

			_arguments.push_back(arg.GetString());
		}
	}


}

