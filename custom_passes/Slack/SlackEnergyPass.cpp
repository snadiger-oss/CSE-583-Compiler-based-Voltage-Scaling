#include "SlackEnergyPass.h"

#include "llvm/IR/Function.h"
#include "llvm/IR/Attributes.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormatVariadic.h"

#include <fstream>
#include <string>
#include "json.hpp"

using namespace llvm;
using nlohmann_json = nlohmann::json;

namespace {

static const std::string MCADir = "../benchmarks/mca/";

bool loadMCA(const std::string &Path,
             double &TotalCycles,
             double &TotaluOps,
             double &RegDepCycles,
             double &IPC)
{
    std::ifstream f(Path);
    if (!f.is_open())
        return false;

    nlohmann_json j;
    f >> j;

    auto &BA = j["CodeRegions"][0]["BottleneckAnalysis"];
    auto &SV = j["CodeRegions"][0]["SummaryView"];

    TotalCycles  = BA["TotalCycles"];
    RegDepCycles = BA["RegisterDependencyCycles"];
    TotaluOps    = SV["TotaluOps"];
    IPC          = SV["IPC"];

    return true;
}

} // namespace

PreservedAnalyses SlackEnergyPass::run(Function &F,
                                       FunctionAnalysisManager &)
{
    std::string FN = F.getName().str();

    // Ignore main()
    if (FN == "main")
        return PreservedAnalyses::all();

    std::string Path = MCADir + FN + "_mca.json";

    double Cyc = 0, UOps = 0, Reg = 0, IPC = 0;

    if (!loadMCA(Path, Cyc, UOps, Reg, IPC)) {
        errs() << "[SlackEnergy] JSON missing for " << FN
               << " (expected " << Path << ")\n";
        return PreservedAnalyses::all();
    }

    double DDR = Reg / Cyc;

    // Classification from SlackPass
    std::string Class = "UNKNOWN";
    if (F.hasFnAttribute("SlackClass"))
        Class = F.getFnAttribute("SlackClass").getValueAsString().str();
    else
        Class = (DDR < 0.5 ? "MEMORY_BOUND" : "COMPUTE_BOUND");

    bool MB = (Class == "MEMORY_BOUND");

    // Energy model
    const double Alpha = 1.0;
    const double Beta  = 0.2;

    double ENom  = Alpha * Cyc + Beta * UOps;
    double Scale = MB ? 0.49 : 1.0;
    double EDVFS = ENom * Scale;
    double Saved = ENom - EDVFS;
    double Pct   = (ENom > 0 ? (Saved / ENom) * 100.0 : 0.0);

    outs() << "\n===== SlackEnergy Report =====\n";
    outs() << "Function          : " << FN << "\n";
    outs() << "SlackClass        : " << Class << "\n";

    outs() << "TotalCycles       : " << formatv("{0:F4}", Cyc) << "\n";
    outs() << "TotaluOps         : " << formatv("{0:F4}", UOps) << "\n";
    outs() << "DDR (RegDep/Cyc)  : " << formatv("{0:F4}", DDR) << "\n";
    outs() << "IPC               : " << formatv("{0:F4}", IPC) << "\n";

    outs() << "Energy (nominal)  : " << formatv("{0:F4}", ENom) << "\n";
    outs() << "Energy (with DVFS): " << formatv("{0:F4}", EDVFS) << "\n";
    outs() << "Energy saved      : " 
           << formatv("{0:F4}", Saved)
           << " (" << formatv("{0:F2}", Pct) << " %)\n";

    outs() << "================================\n";

    return PreservedAnalyses::all();
}
