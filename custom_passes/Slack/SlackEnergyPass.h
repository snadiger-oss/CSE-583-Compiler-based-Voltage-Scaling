#ifndef SLACK_ENERGY_PASS_H
#define SLACK_ENERGY_PASS_H

#include "llvm/IR/PassManager.h"

class SlackEnergyPass : public llvm::PassInfoMixin<SlackEnergyPass> {
public:
    llvm::PreservedAnalyses run(llvm::Function &F,
                                llvm::FunctionAnalysisManager &AM);
};

#endif
