#pragma once

#include <iostream>
#include "usdmar/solvers/subsolver.h"


class SubprocessSubSolver : public SubSolver {

public:
	SubprocessSubSolver();
	~SubprocessSubSolver();

	virtual const std::string GetName() override;
	virtual std::string Resolve(std::string assetPath) override;
	virtual void ConfigureFromJsObject(const JsObject& object) override;

	virtual std::shared_ptr<SubSolver> CreateNew() const {
		return std::shared_ptr<SubprocessSubSolver>(new SubprocessSubSolver());
	};

private:
	std::string _name = "subprocess";
	std::string _executable = "";
	std::vector<std::string> _arguments;
	std::string _windowsSuffix;
	std::string _communicateToken = "USDMAR_RESULT:";
	std::string _fullCommand;
};
