#include <iostream>

#include <pxr/base/arch/fileSystem.h>

#include "usdmar/solvers/env.h"
#include "usdmar/utils.h"

EnvSubSolver::EnvSubSolver() {}
EnvSubSolver::~EnvSubSolver() = default;

const std::string EnvSubSolver::GetName() {	return _name;}

std::string EnvSubSolver::Resolve(std::string assetPath)
{
	std::string result = ExpandString(assetPath);
	if (_normalize) {
		result = ArchNormPath(result);
	}
	return result;
}
void EnvSubSolver::ConfigureFromJsObject(const JsObject & object) {

	auto normalizePtr = _GetJsObjectValue<bool>(object, "normalize");
	if (normalizePtr != nullptr) {
		_normalize = *normalizePtr;
	}

}

