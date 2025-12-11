#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Analysis/BlockFrequencyInfo.h"

using namespace llvm;

namespace {

struct DVFSPass : public PassInfoMixin<DVFSPass> {

    void insertCall(IRBuilder<> &B, Module *M, StringRef Name) {
        FunctionCallee Fn =
            M->getOrInsertFunction(Name, FunctionType::getVoidTy(M->getContext()));
        B.CreateCall(Fn);
    }

    PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM) {
        StringRef FuncName = F.getName();

        // 1. Read SlackClass attribute set by SlackPass
        if (!F.hasFnAttribute("SlackClass")) {
            outs() << "[DVFSPass] " << FuncName
                   << " has no SlackClass attribute, skipping.\n";
            return PreservedAnalyses::all();
        }

        Attribute A = F.getFnAttribute("SlackClass");
        StringRef Class = A.getValueAsString();
        bool IsMemoryBound = (Class == "MEMORY_BOUND");

        outs() << "[DVFSPass] " << FuncName
               << " SlackClass = " << Class << "\n";

        // 2. If compute-bound → always skip DVFS
        if (!IsMemoryBound) {
            outs() << "    POLICY: compute-bound → no DVFS.\n\n";
            return PreservedAnalyses::all();
        }

        // 3. Use BFI to decide hot/cold
        BlockFrequencyInfo &BFI = FAM.getResult<BlockFrequencyAnalysis>(F);
        BasicBlock &Entry = F.getEntryBlock();
        uint64_t EntryFreq = BFI.getBlockFreq(&Entry).getFrequency();

        // Simple hotness heuristic; tweak if needed
        const uint64_t HotThreshold = 100;
        bool IsHot = (EntryFreq > HotThreshold);

        outs() << "    EntryFreq=" << EntryFreq
               << " → " << (IsHot ? "HOT" : "COLD") << "\n";

        // Memory-bound but cold → skip DVFS
        if (!IsHot) {
            outs() << "    POLICY: memory-bound but COLD → skip DVFS.\n\n";
            return PreservedAnalyses::all();
        }

        // 4. Memory-bound + hot → insert DVFS calls
        outs() << "    POLICY: memory-bound + HOT → instrument with DVFS.\n";

        Module *M = F.getParent();

        // scale_down at entry
        IRBuilder<> EntryBuilder(&*Entry.getFirstInsertionPt());
        insertCall(EntryBuilder, M, "__dvfs_scale_down");

        // scale_up at all returns
        for (BasicBlock &BB : F) {
            if (auto *RI = dyn_cast<ReturnInst>(BB.getTerminator())) {
                IRBuilder<> B(RI);
                insertCall(B, M, "__dvfs_scale_up");
            }
        }

        outs() << "    DVFS instrumentation complete.\n\n";
        return PreservedAnalyses::none();
    }
};

} // namespace

extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "DVFS", "v3.0",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "DVFS") {
                        FPM.addPass(DVFSPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}
