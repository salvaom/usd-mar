#include <pxr/base/js/json.h>

#include "usdmar/utils.h"
#include "usdmar/solverStack.h"
#include "usdmar/solvers/rest.h"
#include "usdmar/solvers/env.h"
#include "usdmar/solvers/format.h"
#include "usdmar/debug.h"

PXR_NAMESPACE_USING_DIRECTIVE

SubSolverStack::SubSolverStack() {}
std::string SubSolverStack::Resolve(const std::string& assetPath)
{
	std::string result = assetPath;
	std::string debugText = "Resolving " + result + ":\n";

	for (const auto& solver : _solvers) {
		auto solverName = solver->GetName();
		while (solverName.length() < 10) solverName += ' ';

		result = solver->Resolve(result);
		debugText += "\t" + solverName + "-> " + result + "\n";
		
	}
	USDMAR_DEBUG(+ debugText);

	return result;
}

void SubSolverStack::AddSubSolver(std::shared_ptr<SubSolver> solver)
{
	_solvers.push_back(solver);
}

/* Static */
std::unique_ptr<SubSolverStack> SubSolverStack::CreateFromJsObject(const JsObject& stackDefiniton)
{

	std::unique_ptr<SubSolverStack> stack = std::make_unique<SubSolverStack>();

	JsObject::const_iterator subsolverIter;
	subsolverIter = stackDefiniton.find("stack");

	if (subsolverIter == stackDefiniton.end()) {
		USDMAR_DEBUG("WARNING: No stack specified\n");
		return nullptr;
	}
	
	
	std::shared_ptr<SubSolver> solver = nullptr;

	for (const JsValue& subsolverDefValue : subsolverIter->second.GetJsArray()) {
		const JsObject& subsolverDef = subsolverDefValue.GetJsObject();

		std::unique_ptr<std::string> typePtr = _GetJsObjectValue<std::string>(subsolverDef, "type");

		if (typePtr == nullptr) {
			USDMAR_DEBUG("Key 'type' could not be found\n");
			continue;
		}

		USDMAR_DEBUG("Found subsolver with type %s\n", typePtr->c_str());
		std::string& type = *typePtr;

		// This is kind of horrible, I wish I knew how to do it better
		if (type == "rest") {
			solver = std::make_shared<RESTSubSolver>();
		}
		else if (type == "env") {
			solver = std::make_shared<EnvSubSolver>();
		}
		else if (type == "format") {
			solver = std::make_shared<FormatterSubSolver>();
		}
		else {
			USDMAR_DEBUG("Subsolver type '%s' unsupported\n", type.c_str());
			solver = nullptr;
		}

		if (solver != nullptr) {
			solver->ConfigureFromJsObject(subsolverDef);
			stack->AddSubSolver(solver);
		}
	}

	return stack;

	
}
