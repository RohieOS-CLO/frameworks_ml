// clang-format off
// Generated file (from: batch_to_space_relaxed.mod.py). Do not edit
#include "../../TestGenerated.h"

namespace batch_to_space_relaxed {
// Generated batch_to_space_relaxed test
#include "generated/examples/batch_to_space_relaxed.example.cpp"
// Generated model constructor
#include "generated/models/batch_to_space_relaxed.model.cpp"
} // namespace batch_to_space_relaxed

TEST_F(GeneratedTests, batch_to_space_relaxed) {
    execute(batch_to_space_relaxed::CreateModel,
            batch_to_space_relaxed::is_ignored,
            batch_to_space_relaxed::examples);
}

