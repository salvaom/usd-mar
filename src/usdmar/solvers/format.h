#pragma once

#include <iostream>
#include <regex>

#include "usdmar/solvers/subsolver.h"

class FormatterSubSolver : public SubSolver {

public:
	FormatterSubSolver();
	~FormatterSubSolver();

	virtual const std::string GetName() override;
	virtual std::string Resolve(std::string assetPath) override;
	virtual void ConfigureFromJsObject(const JsObject& object) override;

private:
	std::string _name = "format";
	std::map<std::string, std::string> _formatters;
};
