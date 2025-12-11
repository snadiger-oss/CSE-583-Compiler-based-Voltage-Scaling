# Build the DVFS LLVM pass as a plugin
add_llvm_pass_plugin(DVFS
    DVFSInstrumentPass.cpp
)

# Include json.hpp and any shared headers from the main include directory
target_include_directories(DVFS PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
