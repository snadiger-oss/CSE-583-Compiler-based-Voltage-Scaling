add_llvm_pass_plugin(Slack
    SlackAnalysisPass.cpp
    SlackEnergyPass.cpp
)

target_include_directories(Slack PRIVATE ${CMAKE_SOURCE_DIR}/include)
