#include "llvm/Analysis/BlockFrequencyInfo.h"
#include "llvm/Analysis/BranchProbabilityInfo.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopIterator.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Scalar/LoopPassManager.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/SSAUpdater.h"

using namespace llvm;

namespace
{
    struct FusionPass : public PassInfoMixin<FusionPass>
    {

        struct LoopIterator {
            llvm::ConstantInt* start;
            llvm::ConstantInt* end;
            llvm::ConstantInt* increment;
            llvm::Value* ptr;
        };
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &FAM)
        {
            llvm::BlockFrequencyAnalysis::Result &bfi = FAM.getResult<BlockFrequencyAnalysis>(F);
            llvm::BranchProbabilityAnalysis::Result &bpi = FAM.getResult<BranchProbabilityAnalysis>(F);
            llvm::LoopAnalysis::Result &li = FAM.getResult<LoopAnalysis>(F);
            
            // Grab all loops in the program
            std::unordered_map<Loop*, std::vector<BasicBlock*>> loops;
            for (auto &bb : F) {
                if (li.getLoopFor(&bb))
                {
                    auto loop = li.getLoopFor(&bb);
                    if (loops.find(loop) == loops.end())
                    {
                        loops[loop] = std::vector<BasicBlock*>();
                    }
                    loops[loop].push_back(&bb);
                }
            }

            // Grab all number-based iterators
            std::unordered_map<llvm::Value *, llvm::Value *> storeinsts;
            std::unordered_map<llvm::Loop*, LoopIterator> iterators;
            for (auto &bb : F)
            {
                for (auto &loop : loops)
                {
                    if (&bb == loop.first->getLoopPreheader())
                    {
                        // Grab all stores in preheaders
                        for (auto &inst : bb)
                        {
                            llvm::StoreInst* ptr = nullptr;
                            if (ptr = llvm::dyn_cast<llvm::StoreInst>(&inst))
                            {
                                storeinsts[ptr->getPointerOperand()] = ptr->getValueOperand();
                                errs() << *ptr << "\n";
                            }
                        }
                    }
                    
                }
                // Check if stored values in preheader are used as iterators
                llvm::LoadInst* candidate_ptr = nullptr;
                if (li.isLoopHeader(&bb)) 
                {
                    for (auto &inst: bb)
                    {
                        llvm::LoadInst* load_ptr;
                        llvm::ICmpInst* cmp_ptr;
                        if (load_ptr = llvm::dyn_cast<LoadInst>(&inst))
                        {
                            candidate_ptr = load_ptr;
                        }
                        else if (cmp_ptr = llvm::dyn_cast<ICmpInst>(&inst))
                        {
                            errs() << *candidate_ptr << '\n' << *(cmp_ptr->getOperand(0)) << '\n';
                            if (cmp_ptr->getOperand(0) == candidate_ptr)
                            {
                                LoopIterator iter;
                                iter.start = llvm::dyn_cast<llvm::ConstantInt>(storeinsts[candidate_ptr->getPointerOperand()]);
                                iter.increment = llvm::dyn_cast<llvm::ConstantInt>(storeinsts[candidate_ptr->getPointerOperand()]);
                                iter.end = llvm::dyn_cast<llvm::ConstantInt>(cmp_ptr->getOperand(1));
                                iter.ptr = llvm::dyn_cast<llvm::Value>(candidate_ptr->getPointerOperand());

                                
                                iterators[li.getLoopFor(&bb)] = iter;
                                errs() << *(cmp_ptr->getOperand(1)) << '\n';
                            }
                        }
                    }
                }
            }
            for (auto &loop: loops)
            {
                llvm::SmallVector<llvm::LoopBase<llvm::BasicBlock, llvm::Loop>::Edge> exit_edge;
                loop.first->getExitEdges(exit_edge);
                if (exit_edge.size() > 1)
                    continue;
                
                llvm::Loop* fusable = nullptr;
                // Find a loop that immediately succeeds this one, if any
                for (auto &i : loops)
                {
                    if (exit_edge[0].second == i.first->getLoopPreheader())
                    {
                        fusable = i.first;
                        break;
                    }
                }
                if (!fusable) continue;
                // Verify bounds of iterators
                auto iter1 = iterators[loop.first];
                auto iter2 = iterators[fusable];
                if (iter1.start->getValue() != iter2.start->getValue()) continue;
                if (iter1.end->getValue() != iter2.end->getValue()) continue;
                if (iter1.increment->getValue() != iter2.increment->getValue()) continue;

                // log all stores used in the first loop
                std::vector<llvm::StoreInst*> gepstores;
                std::vector<llvm::StoreInst*> nongepstores;
                for (auto &bb : loop.second)
                {
                    for (auto &inst : *bb)
                    {
                        llvm::StoreInst* ptr = nullptr;
                        if (ptr = llvm::dyn_cast<llvm::StoreInst>(&inst))
                        {
                            llvm::GetElementPtrInst* gep = nullptr;
                            if (gep = llvm::dyn_cast<llvm::GetElementPtrInst>(ptr->getPointerOperand()))
                            {
                                errs() << "First loop stores at " << *gep << '\n';
                                gepstores.push_back(ptr);
                            } else{
                                nongepstores.push_back(ptr);
                            }
                        }
                    }

                    
                }
                // Now check loads in second loop
                bool dependent_on_completion = false;
                for (auto &bb : loops[fusable])
                {
                    for (auto &inst: *bb)
                    {
                        llvm::LoadInst* ptr = nullptr;
                        if (ptr = llvm::dyn_cast<llvm::LoadInst>(&inst))
                        {
                            llvm::GetElementPtrInst *gep = nullptr;
                            if (gep = llvm::dyn_cast<llvm::GetElementPtrInst>(ptr->getPointerOperand()))
                            {
                                for (auto &i : gepstores)
                                {
                                    errs() << "Second loop loads at " << *gep << '\n';
                                    auto first_gep = llvm::dyn_cast<llvm::GetElementPtrInst>(i->getPointerOperand());
                                    llvm::LoadInst* fload_ptr = nullptr;
                                    llvm::SExtInst* fsext_ptr = nullptr;
                                    llvm::LoadInst *sload_ptr = nullptr;
                                    llvm::SExtInst *ssext_ptr = nullptr;
                                    if (!(fload_ptr = llvm::dyn_cast<llvm::LoadInst>(first_gep->getOperand(2))))
                                    {
                                        if(!(fsext_ptr = llvm::dyn_cast<llvm::SExtInst>(first_gep->getOperand(2))))
                                        {
                                            errs() << "Loop1 operand does pointer arithmetic 64 bit\n";
                                            dependent_on_completion = true;
                                            break;
                                        }
                                        if (!(fload_ptr = llvm::dyn_cast<llvm::LoadInst>(fsext_ptr->getOperand(0))))
                                        {
                                            errs() << "Loop1 operand does pointer arithmetic 32 bit\n";
                                            dependent_on_completion = true;
                                            break;
                                        }
                                    }
                                    if (!(sload_ptr = llvm::dyn_cast<llvm::LoadInst>(gep->getOperand(2))))
                                    {
                                        if (!(ssext_ptr = llvm::dyn_cast<llvm::SExtInst>(gep->getOperand(2))))
                                        {
                                            errs() << "Loop2 operand does pointer arithmetic 64 bit\n";
                                            dependent_on_completion = true;
                                            break;
                                        }
                                        if (!(sload_ptr = llvm::dyn_cast<llvm::LoadInst>(ssext_ptr->getOperand(0))))
                                        {
                                            errs() << "Loop2 operand does pointer arithmetic 32 bit\n";
                                            dependent_on_completion = true;
                                            break;
                                        }
                                    }
                                    if (fload_ptr->getPointerOperand() != iterators[loop.first].ptr || sload_ptr->getPointerOperand() != iterators[fusable].ptr)
                                    {
                                        errs() << "GetPointerInst relies on value that is not loop's iterator\n";
                                        dependent_on_completion = true;
                                        break;
                                    }
                                    sload_ptr->setOperand(0, fload_ptr->getPointerOperand());
                                }
                            }
                            else
                            {
                                for (auto &i : nongepstores)
                                {
                                    if (ptr->getPointerOperand() == i->getPointerOperand())
                                    {
                                        errs() << "Load in second loop depends on non-array store in first\n";
                                        dependent_on_completion = true;
                                        break;
                                    }
                                }
                            }
                        }
                        if (dependent_on_completion) break;
                    }
                }
                if (dependent_on_completion) continue;
                // Point bbs in second loop to first loops latch
                for (auto &bb : loops[fusable])
                {
                    auto term = bb->getTerminator();
                    auto branch = llvm::dyn_cast<BranchInst>(term);
                    if (branch->isConditional())
                        continue;
                    errs() << "Successor: " << branch->getSuccessor(0) << " Fusee: " << fusable->getLoopLatch() << '\n';
                    if (branch->getSuccessor(0) == fusable->getLoopLatch())
                    {
                        errs() << "Basic block in loop 2 now points to " << loop.first->getLoopLatch() << *loop.first->getLoopLatch() << '\n';
                        branch->setOperand(0, loop.first->getLoopLatch());
                    }
                }

                // Point all branches to the loop latch to the next loops first non-header bb
                for (auto &bb: loop.second)
                {
                    auto term = bb->getTerminator();
                    auto branch = llvm::dyn_cast<BranchInst>(term);
                    if (branch->isConditional()) continue;
                    if (branch->getSuccessor(0) == loop.first->getLoopLatch())
                    {
                        branch->setOperand(0, loops[fusable][1]);
                    }
                }
                // Make loop exit to where next loop exits
                auto head_term = loop.second[0]->getTerminator();
                auto head_branch = llvm::dyn_cast<BranchInst>(head_term);
                auto next_term = loops[fusable][0]->getTerminator();
                auto next_branch = llvm::dyn_cast<BranchInst>(next_term);
                head_branch->setOperand(1, next_branch->getOperand(1));

                //update information of new loop
                for (auto &bb : loops[fusable])
                {
                    if (li.isLoopHeader(bb)) continue;
                    if (bb = fusable->getLoopLatch()) continue;
                    loop.second.push_back(bb);
                }

                
                loops.erase(loops.find(fusable));
            }
            llvm::EliminateUnreachableBlocks(F);
            return PreservedAnalyses::all();
        }
    };
}

// Code from HW2 to register pass with llvm
extern "C" ::llvm::PassPluginLibraryInfo LLVM_ATTRIBUTE_WEAK llvmGetPassPluginInfo()
{
    return {
        LLVM_PLUGIN_API_VERSION, "LoopOpt", "v0.1",
        [](PassBuilder &PB)
        {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, FunctionPassManager &FPM,
                   ArrayRef<PassBuilder::PipelineElement>)
                {
                    if (Name == "LoopOpt")
                    {
                        FPM.addPass(FusionPass());
                        return true;
                    }
                    return false;
                });
        }};
}