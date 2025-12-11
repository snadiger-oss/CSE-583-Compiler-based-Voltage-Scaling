#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Function.h"

#include <fstream>
#include <string>
#include "json.hpp"

#include "SlackEnergyPass.h"   // <-- correct include

using namespace llvm;
using nlohmann_json = nlohmann::json;

namespace {

class SlackPass : public PassInfoMixin<SlackPass> {
    std::string MCADir = "../benchmarks/mca/";

    bool loadMCA(const std::string &Path,
                 double &TotalCycles,
                 double &RegDepCycles)
    {
        std::ifstream f(Path);
        if (!f.is_open())
            return false;

        nlohmann_json j;
        f >> j;

        auto &BA = j["CodeRegions"][0]["BottleneckAnalysis"];
        TotalCycles  = BA["TotalCycles"];
        RegDepCycles = BA["RegisterDependencyCycles"];
        return true;
    }

public:
    PreservedAnalyses run(Function &F,
                          FunctionAnalysisManager &) 
    {
        std::string FN = F.getName().str();

        // Ignore main()
        if (FN == "main")
            return PreservedAnalyses::all();

        std::string Path = MCADir + FN + "_mca.json";

        double Cyc = 0, Reg = 0;
        if (!loadMCA(Path, Cyc, Reg)) {
            errs() << "[SlackPass] JSON missing for " << FN
                   << " (expected " << Path << ")\n";
            return PreservedAnalyses::all();
        }

        double DDR = Reg / Cyc;
        bool MB = (DDR < 0.5);
        const char *Label = MB ? "MEMORY_BOUND" : "COMPUTE_BOUND";

        outs() << "[SlackPass] " << FN
               << " DDR=" << DDR
               << " → " << Label << "\n";

        F.addFnAttr("SlackClass", Label);

        return PreservedAnalyses::none();
    }
};

} // namespace

// Plugin entry point – ONLY here
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK
llvmGetPassPluginInfo() {
    return {
        LLVM_PLUGIN_API_VERSION, "Slack", "v1.0",
        [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name,
                   FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
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
