#pragma once

#include <iostream>
#include "usdmar/solvers/subsolver.h"


class EnvSubSolver : public SubSolver {

public:
	EnvSubSolver();
	~EnvSubSolver();

	virtual const std::string GetName() override;
	virtual std::string Resolve(std::string assetPath) override;
	virtual void ConfigureFromJsObject(const JsObject& object) override;

	virtual std::shared_ptr<SubSolver> CreateNew() const {
		return std::shared_ptr<EnvSubSolver>(new EnvSubSolver());
	};

private:
	std::string _name = "env";
	bool _normalize = true;
};
