#pragma once
#include <iostream>

#include <pxr/pxr.h>
#include <pxr/base/js/types.h>

#include "usdmar/solvers/subsolver.h"

PXR_NAMESPACE_USING_DIRECTIVE

class SubSolverStack {
public:
	SubSolverStack();
	~SubSolverStack() = default;

	std::string Resolve(const std::string& assetPath);
	void AddSubSolver(std::shared_ptr<SubSolver> solver);

	static std::unique_ptr<SubSolverStack> CreateFromJsObject(const JsObject& stackDefiniton);

private:
	std::vector<std::shared_ptr<SubSolver>> _solvers;
};
