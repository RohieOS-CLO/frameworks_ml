/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "SVDF.h"

#include "NeuralNetworksWrapper.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

using ::testing::FloatNear;
using ::testing::Matcher;

namespace android {
namespace nn {
namespace wrapper {

namespace {

std::vector<Matcher<float>> ArrayFloatNear(const std::vector<float>& values,
                                           float max_abs_error=1.e-6) {
  std::vector<Matcher<float>> matchers;
  matchers.reserve(values.size());
  for (const float& v : values) {
    matchers.emplace_back(FloatNear(v, max_abs_error));
  }
  return matchers;
}

}  // namespace

using ::testing::ElementsAreArray;

static float svdf_input[] = {0.12609188,  -0.46347019, -0.89598465,
                             0.12609188,  -0.46347019, -0.89598465,

                             0.14278367,  -1.64410412, -0.75222826,
                             0.14278367,  -1.64410412, -0.75222826,

                             0.49837467,  0.19278903,  0.26584083,
                             0.49837467,  0.19278903,  0.26584083,

                             -0.11186574, 0.13164264,  -0.05349274,
                             -0.11186574, 0.13164264,  -0.05349274,

                             -0.68892461, 0.37783599,  0.18263303,
                             -0.68892461, 0.37783599,  0.18263303,

                             -0.81299269, -0.86831826, 1.43940818,
                             -0.81299269, -0.86831826, 1.43940818,

                             -1.45006323, -0.82251364, -1.69082689,
                             -1.45006323, -0.82251364, -1.69082689,

                             0.03966608,  -0.24936394, -0.77526885,
                             0.03966608,  -0.24936394, -0.77526885,

                             0.11771342,  -0.23761693, -0.65898693,
                             0.11771342,  -0.23761693, -0.65898693,

                             -0.89477462, 1.67204106,  -0.53235275,
                             -0.89477462, 1.67204106,  -0.53235275};

static float svdf_golden_output[] = {
    0.014899,    -0.0517661, -0.143725, -0.00271883,
    0.014899,    -0.0517661, -0.143725, -0.00271883,

    0.068281,    -0.162217,  -0.152268, 0.00323521,
    0.068281,    -0.162217,  -0.152268, 0.00323521,

    -0.0317821,  -0.0333089, 0.0609602, 0.0333759,
    -0.0317821,  -0.0333089, 0.0609602, 0.0333759,

    -0.00623099, -0.077701,  -0.391193, -0.0136691,
    -0.00623099, -0.077701,  -0.391193, -0.0136691,

    0.201551,    -0.164607,  -0.179462, -0.0592739,
    0.201551,    -0.164607,  -0.179462, -0.0592739,

    0.0886511,   -0.0875401, -0.269283, 0.0281379,
    0.0886511,   -0.0875401, -0.269283, 0.0281379,

    -0.201174,   -0.586145,  -0.628624, -0.0330412,
    -0.201174,   -0.586145,  -0.628624, -0.0330412,

    -0.0839096,  -0.299329,  0.108746,  0.109808,
    -0.0839096,  -0.299329,  0.108746,  0.109808,

    0.419114,    -0.237824,  -0.422627, 0.175115,
    0.419114,    -0.237824,  -0.422627, 0.175115,

    0.36726,     -0.522303,  -0.456502, -0.175475,
    0.36726,     -0.522303,  -0.456502, -0.175475};

#define FOR_ALL_INPUT_AND_WEIGHT_TENSORS(ACTION) \
  ACTION(Input)                                  \
  ACTION(WeightsFeature)                         \
  ACTION(WeightsTime)                            \
  ACTION(Bias)

// For all output and intermediate states
#define FOR_ALL_OUTPUT_TENSORS(ACTION) \
  ACTION(State)                        \
  ACTION(Output)

// Derived class of SingleOpModel, which is used to test SVDF TFLite op.
class SVDFOpModel {
 public:
  SVDFOpModel(uint32_t batches, uint32_t units, uint32_t input_size,
              uint32_t memory_size)
      : batches_(batches),
        units_(units),
        input_size_(input_size),
        memory_size_(memory_size) {
    std::vector<std::vector<uint32_t>> input_shapes{
        {batches_, input_size_},  // Input tensor
        {units_, input_size_},    // weights_feature tensor
        {units_, memory_size_},   // weights_time tensor
        {units_}                  // bias tensor
    };
    std::vector<uint32_t> inputs;
    auto it = input_shapes.begin();

    // Input and weights
#define AddInput(X)                                   \
  OperandType X##OpndTy(Type::TENSOR_FLOAT32, *it++); \
  inputs.push_back(model_.addOperand(&X##OpndTy));

    FOR_ALL_INPUT_AND_WEIGHT_TENSORS(AddInput);

#undef AddInput

    // Parameters
    OperandType RankParamTy(Type::INT32, {});
    inputs.push_back(model_.addOperand(&RankParamTy));
    OperandType ActivationParamTy(Type::INT32, {});
    inputs.push_back(model_.addOperand(&ActivationParamTy));

    // Output and other intermediate state
    std::vector<std::vector<uint32_t>> output_shapes{{batches_, (memory_size_ - 1) * units_},
                                                     {batches_, units_}};
    std::vector<uint32_t> outputs;

    auto it2 = output_shapes.begin();

#define AddOutput(X)                                   \
  OperandType X##OpndTy(Type::TENSOR_FLOAT32, *it2++); \
  outputs.push_back(model_.addOperand(&X##OpndTy));

    FOR_ALL_OUTPUT_TENSORS(AddOutput);

#undef AddOutput

    Input_.insert(Input_.end(), batches_ * input_size_, 0.f);

    auto multiAll = [](const std::vector<uint32_t> &dims) -> uint32_t {
        uint32_t sz = 1;
        for(uint32_t d:dims) { sz *= d; }
        return sz;
    };

    it2 = output_shapes.begin();

#define ReserveOutput(X) X##_.insert(X##_.end(), multiAll(*it2++), 0.f);

    FOR_ALL_OUTPUT_TENSORS(ReserveOutput);

    model_.addOperation(ANEURALNETWORKS_SVDF, inputs, outputs);
    model_.setInputsAndOutputs(inputs, outputs);
  }

  void Invoke() {
    ASSERT_TRUE(model_.isValid());

    Compilation compilation(&model_);
    compilation.compile();
    Request request(&compilation);
#define SetInputOrWeight(X)                                                  \
  ASSERT_EQ(request.setInput(SVDF::k##X##Tensor, X##_.data(), sizeof(X##_)), \
            Result::NO_ERROR);

    FOR_ALL_INPUT_AND_WEIGHT_TENSORS(SetInputOrWeight);

#undef SetInputOrWeight

#define SetOutput(X)                                                          \
  ASSERT_EQ(request.setOutput(SVDF::k##X##Tensor, X##_.data(), sizeof(X##_)), \
            Result::NO_ERROR);

    FOR_ALL_OUTPUT_TENSORS(SetOutput);

#undef SetOutput

    int rank = 1;
    ASSERT_EQ(request.setInput(SVDF::kRankParam, &rank, sizeof(rank)),
              Result::NO_ERROR);

    int activation = ActivationFn::kActivationNone;
    ASSERT_EQ(request.setInput(SVDF::kActivationParam, &activation,
                               sizeof(activation)),
              Result::NO_ERROR);

    ASSERT_EQ(request.compute(), Result::NO_ERROR);
  }

#define DefineSetter(X)                          \
  void Set##X(const std::vector<float>& f) {     \
    X##_.insert(X##_.end(), f.begin(), f.end()); \
  }

  FOR_ALL_INPUT_AND_WEIGHT_TENSORS(DefineSetter);

#undef DefineSetter

  void SetInput(int offset, float* begin, float* end) {
    for (; begin != end; begin++, offset++) {
      Input_[offset] = *begin;
    }
  }

  // Resets the state of SVDF op by filling it with 0's.
  void ResetState() { std::fill(State_.begin(), State_.end(), 0.f); }

  // Extracts the output tensor from the SVDF op.
  const std::vector<float>& GetOutput() const { return Output_; }

  int input_size() const { return input_size_; }
  int num_units() const { return units_; }
  int num_batches() const { return batches_; }

 private:
  Model model_;

  const uint32_t batches_;
  const uint32_t units_;
  const uint32_t input_size_;
  const uint32_t memory_size_;

#define DefineTensor(X) std::vector<float> X##_;

  FOR_ALL_INPUT_AND_WEIGHT_TENSORS(DefineTensor);
  FOR_ALL_OUTPUT_TENSORS(DefineTensor);

#undef DefineTensor
};

TEST(SVDFOpTest, BlackBoxTest) {
  SVDFOpModel svdf(/*batches=*/2, /*units=*/4, /*input_size=*/3,
                   /*memory_size=*/10);
  svdf.SetWeightsFeature({-0.31930989, -0.36118156, 0.0079667, 0.37613347,
                          0.22197971, 0.12416199, 0.27901134, 0.27557442,
                          0.3905206, -0.36137494, -0.06634006, -0.10640851});

  svdf.SetWeightsTime(
      {-0.31930989, 0.37613347,  0.27901134,  -0.36137494, -0.36118156,
       0.22197971,  0.27557442,  -0.06634006, 0.0079667,   0.12416199,

       0.3905206,   -0.10640851, -0.0976817,  0.15294972,  0.39635518,
       -0.02702999, 0.39296314,  0.15785322,  0.21931258,  0.31053296,

       -0.36916667, 0.38031587,  -0.21580373, 0.27072677,  0.23622236,
       0.34936687,  0.18174365,  0.35907319,  -0.17493086, 0.324846,

       -0.10781813, 0.27201805,  0.14324132,  -0.23681851, -0.27115166,
       -0.01580888, -0.14943552, 0.15465137,  0.09784451,  -0.0337657});

  svdf.ResetState();
  const int svdf_num_batches = svdf.num_batches();
  const int svdf_input_size = svdf.input_size();
  const int svdf_num_units = svdf.num_units();
  const int input_sequence_size =
      sizeof(svdf_input) / sizeof(float) / (svdf_input_size * svdf_num_batches);
  // Going over each input batch, setting the input tensor, invoking the SVDF op
  // and checking the output with the expected golden values.
  for (int i = 0; i < input_sequence_size; i++) {
    float* batch_start = svdf_input + i * svdf_input_size * svdf_num_batches;
    float* batch_end = batch_start + svdf_input_size * svdf_num_batches;
    svdf.SetInput(0, batch_start, batch_end);

    svdf.Invoke();

    float* golden_start =
        svdf_golden_output + i * svdf_num_units * svdf_num_batches;
    float* golden_end = golden_start + svdf_num_units * svdf_num_batches;
    std::vector<float> expected;
    expected.insert(expected.end(), golden_start, golden_end);

    EXPECT_THAT(svdf.GetOutput(), ElementsAreArray(ArrayFloatNear(expected)));
  }
}

}  // namespace wrapper
}  // namespace nn
}  // namespace android
