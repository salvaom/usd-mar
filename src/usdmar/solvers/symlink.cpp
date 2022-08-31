#include <iostream>

#include <pxr/base/tf/fileUtils.h>
#include <pxr/base/tf/pathUtils.h>
#include <pxr/base/tf/diagnosticLite.h>

#include "usdmar/solvers/symlink.h"
#include "usdmar/utils.h"

SymlinkSubSolver::SymlinkSubSolver() {}
SymlinkSubSolver::~SymlinkSubSolver() = default;

const std::string SymlinkSubSolver::GetName() {	return _name;}

std::string SymlinkSubSolver::Resolve(std::string assetPath)
{
	std::string error;
	if (TfPathExists(assetPath) && TfIsLink(assetPath)) {
		auto result = TfRealPath(assetPath, false, &error);
		if (error.size()) {
			TF_WARN("Failed to expand symlink of '%s': '%s'", assetPath.c_str(), error.c_str());
		}
		else {
			return result; 
		}
	}
	return assetPath;
}
void SymlinkSubSolver::ConfigureFromJsObject(const JsObject & object) {}

