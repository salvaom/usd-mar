#include <pxr/base/js/json.h>

#include "usdmar/utils.h"
#include "usdmar/solverStack.h"
#include "usdmar/solvers/rest.h"
#include "usdmar/solvers/env.h"
#include "usdmar/solvers/format.h"
#include "usdmar/debug.h"
#include "usdmar/registry.h"




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
	subsolverIter = stackDefiniton.find("resolvers");

	if (subsolverIter == stackDefiniton.end()) {
		USDMAR_DEBUG("WARNING: No 'resolvers' key specified\n");
		return nullptr;
	}
	
	
	std::shared_ptr<SubSolver> solverPtr = nullptr;

	for (const JsValue& subsolverDefValue : subsolverIter->second.GetJsArray()) {
		const JsObject& subsolverDef = subsolverDefValue.GetJsObject();

		std::unique_ptr<std::string> typePtr = _GetJsObjectValue<std::string>(subsolverDef, "type");

		if (typePtr == nullptr) {
			USDMAR_DEBUG("Key 'type' could not be found\n");
			continue;
		}
		std::string& type = *typePtr;

		Registry& registry = Registry::GetInstance();
		auto solverPtr = registry.GetSubSolver(type);

		if (solverPtr == nullptr) {
			TF_WARN("SubSolver with type '%s' is not supported", type.c_str());
			continue;
		}

		// Make a copy
		solverPtr = solverPtr->CreateNew();
		solverPtr->ConfigureFromJsObject(subsolverDef);
		stack->AddSubSolver(solverPtr);
	}

	return stack;

	
}
