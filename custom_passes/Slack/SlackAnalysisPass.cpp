#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormatVariadic.h"
#include "llvm/IR/Function.h"
#include <fstream>
#include <string>
#include "json.hpp"
#include "SlackEnergyPass.h"
using namespace llvm;
namespace {
class SlackPass : public PassInfoMixin<SlackPass> {
    std::string MCADir = "../benchmarks/mca/";
    bool loadMCA(const std::string &Path,
                 double &TotalCycles,
                 double &RegDepCycles,
                 double &IPC)
    {
        std::ifstream f(Path);
        if (!f.is_open())
            return false;
        nlohmann::json j;
        f >> j;
        auto &BA = j["CodeRegions"][0]["BottleneckAnalysis"];
        auto &SV = j["CodeRegions"][0]["SummaryView"];
        TotalCycles  = BA["TotalCycles"];
        RegDepCycles = BA["RegisterDependencyCycles"];
        IPC          = SV["IPC"];
        return true;
    }
public:
    PreservedAnalyses run(Function &F,
                          FunctionAnalysisManager &)
    {
        std::string FN = F.getName().str();
        // Ignore main
        if (FN == "main")
            return PreservedAnalyses::all();
        std::string Path = MCADir + FN + "_mca.json";
        double Cyc = 0, Reg = 0, IPC = 0;
        if (!loadMCA(Path, Cyc, Reg, IPC)) {
            errs() << "[SlackPass] JSON missing for " << FN
                   << " (expected " << Path << ")\n";
            return PreservedAnalyses::all();
        }
        double DDR = Reg / Cyc;
        // Final heuristic
        bool MemoryBound =
            (DDR < 0.4) &&
            (IPC > 0.5);
        const char *Label =
            MemoryBound ? "MEMORY_BOUND" : "COMPUTE_BOUND";
        outs() << "[SlackPass] " << FN
               << " DDR=" << formatv("{0:F4}", DDR)
               << " IPC=" << formatv("{0:F4}", IPC)
               << " → " << Label << "\n";
        F.addFnAttr("SlackClass", Label);
        return PreservedAnalyses::none();
    }
};
} // namespace
extern "C" LLVM_ATTRIBUTE_WEAK
::llvm::PassPluginLibraryInfo llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION,
        "Slack",
        "v2.1",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                   FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>) {
                    if (Name == "slack-pass") {
                        FPM.addPass(SlackPass());
                        return true;
                    }
                    if (Name == "slack-energy") {
                        FPM.addPass(SlackEnergyPass());
                        return true;
                    }
                    return false;
                });
        }
    };
}

