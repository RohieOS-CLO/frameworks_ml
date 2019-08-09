// Generated from softmax_quant8_1.mod.py
// DO NOT EDIT
// clang-format off
#include "TestHarness.h"

using namespace test_helper;

namespace generated_tests::softmax_quant8_1 {

const TestModel& get_test_model() {
    static TestModel model = {
        .expectedMultinomialDistributionTolerance = 0,
        .inputIndexes = {0},
        .isRelaxed = false,
        .operands = {{
                .channelQuant = {},
                .data = TestBuffer::createFromVector<uint8_t>({1, 2, 10, 20}),
                .dimensions = {1, 4},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::MODEL_INPUT,
                .numberOfConsumers = 1,
                .scale = 0.5f,
                .type = TestOperandType::TENSOR_QUANT8_ASYMM,
                .zeroPoint = 0
            }, {
                .channelQuant = {},
                .data = TestBuffer::createFromVector<float>({1e-05f}),
                .dimensions = {},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::CONSTANT_COPY,
                .numberOfConsumers = 1,
                .scale = 0.0f,
                .type = TestOperandType::FLOAT32,
                .zeroPoint = 0
            }, {
                .channelQuant = {},
                .data = TestBuffer::createFromVector<uint8_t>({64, 64, 64, 64}),
                .dimensions = {1, 4},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::MODEL_OUTPUT,
                .numberOfConsumers = 0,
                .scale = 0.00390625f,
                .type = TestOperandType::TENSOR_QUANT8_ASYMM,
                .zeroPoint = 0
            }},
        .operations = {{
                .inputs = {0, 1},
                .outputs = {2},
                .type = TestOperationType::SOFTMAX
            }},
        .outputIndexes = {2}
    };
    return model;
}

}  // namespace generated_tests::softmax_quant8_1

namespace generated_tests::softmax_quant8_1 {

const TestModel& get_test_model_all_inputs_as_internal() {
    static TestModel model = {
        .expectedMultinomialDistributionTolerance = 0,
        .inputIndexes = {3},
        .isRelaxed = false,
        .operands = {{
                .channelQuant = {},
                .data = TestBuffer::createFromVector<uint8_t>({}),
                .dimensions = {1, 4},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::TEMPORARY_VARIABLE,
                .numberOfConsumers = 1,
                .scale = 0.5f,
                .type = TestOperandType::TENSOR_QUANT8_ASYMM,
                .zeroPoint = 0
            }, {
                .channelQuant = {},
                .data = TestBuffer::createFromVector<float>({1e-05f}),
                .dimensions = {},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::CONSTANT_COPY,
                .numberOfConsumers = 1,
                .scale = 0.0f,
                .type = TestOperandType::FLOAT32,
                .zeroPoint = 0
            }, {
                .channelQuant = {},
                .data = TestBuffer::createFromVector<uint8_t>({64, 64, 64, 64}),
                .dimensions = {1, 4},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::MODEL_OUTPUT,
                .numberOfConsumers = 0,
                .scale = 0.00390625f,
                .type = TestOperandType::TENSOR_QUANT8_ASYMM,
                .zeroPoint = 0
            }, {
                .channelQuant = {},
                .data = TestBuffer::createFromVector<uint8_t>({1, 2, 10, 20}),
                .dimensions = {1, 4},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::MODEL_INPUT,
                .numberOfConsumers = 1,
                .scale = 0.5f,
                .type = TestOperandType::TENSOR_QUANT8_ASYMM,
                .zeroPoint = 0
            }, {
                .channelQuant = {},
                .data = TestBuffer::createFromVector<uint8_t>({0}),
                .dimensions = {1},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::CONSTANT_COPY,
                .numberOfConsumers = 1,
                .scale = 0.5f,
                .type = TestOperandType::TENSOR_QUANT8_ASYMM,
                .zeroPoint = 0
            }, {
                .channelQuant = {},
                .data = TestBuffer::createFromVector<int32_t>({0}),
                .dimensions = {},
                .isIgnored = false,
                .lifetime = TestOperandLifeTime::CONSTANT_COPY,
                .numberOfConsumers = 1,
                .scale = 0.0f,
                .type = TestOperandType::INT32,
                .zeroPoint = 0
            }},
        .operations = {{
                .inputs = {3, 4, 5},
                .outputs = {0},
                .type = TestOperationType::ADD
            }, {
                .inputs = {0, 1},
                .outputs = {2},
                .type = TestOperationType::SOFTMAX
            }},
        .outputIndexes = {2}
    };
    return model;
}

}  // namespace generated_tests::softmax_quant8_1

