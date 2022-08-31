#pragma once

#include <iostream>
#include "usdmar/solvers/subsolver.h"


class SymlinkSubSolver : public SubSolver {

public:
	SymlinkSubSolver();
	~SymlinkSubSolver();

	virtual const std::string GetName() override;
	virtual std::string Resolve(std::string assetPath) override;
	virtual void ConfigureFromJsObject(const JsObject& object) override;

	virtual std::shared_ptr<SubSolver> CreateNew() const {
		return std::shared_ptr<SymlinkSubSolver>(new SymlinkSubSolver());
	};

private:
	std::string _name = "symlink";
};
