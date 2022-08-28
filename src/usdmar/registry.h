#pragma once
#include <iostream>
#include <map>

#include "usdmar/solverStack.h"
#include "usdmar/solvers/subsolver.h"

class Registry
{
public:
    static Registry& GetInstance()
    {
        static Registry instance;
        return instance;
    }

    void RegisterStack(const std::string name, SubSolverStack stack);
    void RegisterStacksFromJsObject(const JsObject& jsonDict);
    std::shared_ptr<SubSolverStack> GetStack(std::string name);

    void RegisterSubSolver(const std::shared_ptr<SubSolver> subsolver);
    std::shared_ptr<SubSolver> GetSubSolver(std::string name);

private:
    Registry() {}
    std::map<std::string, std::shared_ptr<SubSolverStack>> _stackRegistry;
    std::map<std::string, std::shared_ptr<SubSolver>> _subsolverRegistry;
};

