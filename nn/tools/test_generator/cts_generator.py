#!/usr/bin/python3

# Copyright 2018, The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""CTS testcase generator

Implements CTS test backend. Invoked by ml/nn/runtime/test/specs/generate_tests.sh;
See that script for details on how this script is used.

"""

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
import os
import sys
import traceback

import test_generator as tg

# Take a model from command line
def ParseCmdLine():
    parser = tg.ArgumentParser()
    parser.add_argument("-e", "--example", help="the output example file or directory")
    args = tg.ParseArgs(parser)
    tg.FileNames.InitializeFileLists(args.spec, args.example, args.test)

# Write headers for generated files, which are boilerplate codes only related to filenames
def InitializeFiles(example_fd, test_fd):
    specFileBase = os.path.basename(tg.FileNames.specFile)
    fileHeader = """\
// Generated from {spec_file}
// DO NOT EDIT
// clang-format off
#include "{header}"
"""
    if test_fd is not None:
        print(fileHeader.format(spec_file=specFileBase, header="TestGenerated.h"), file=test_fd)
    if example_fd is not None:
        print(fileHeader.format(spec_file=specFileBase, header="TestHarness.h"), file=example_fd)
        print("using namespace test_helper;\n", file=example_fd)

def IndentedStr(s, indent):
    return ("\n" + " " * indent).join(s.split('\n'))

def ToCpp(var, indent=0):
    """Get the C++-style representation of a Python object.

    For Python dictionary, it will be mapped to C++ struct aggregate initialization:
        {
            .key0 = value0,
            .key1 = value1,
            ...
        }

    For Python list, it will be mapped to C++ list initalization:
        {value0, value1, ...}

    In both cases, value0, value1, ... are stringified by invoking this method recursively.
    """
    if isinstance(var, dict):
        if not var:
            return "{}"
        str_pair = lambda k, v: "    .%s = %s" % (k, ToCpp(v, indent + 4))
        agg_init = "{\n%s\n}" % (",\n".join(str_pair(k, var[k]) for k in sorted(var.keys())))
        return IndentedStr(agg_init, indent)
    elif isinstance(var, (list, tuple)):
        return "{%s}" % (", ".join(ToCpp(i, indent) for i in var))
    elif type(var) is bool:
        return "true" if var else "false"
    elif type(var) is float:
        return tg.PrettyPrintAsFloat(var)
    else:
        return str(var)

def GetSymmPerChannelQuantParams(extraParams):
    """Get the dictionary that corresponds to test_helper::TestSymmPerChannelQuantParams."""
    if extraParams is None or extraParams.hide:
        return {}
    else:
        return {"scales": extraParams.scales, "channelDim": extraParams.channelDim}

def GetOperandStruct(operand):
    """Get the dictionary that corresponds to test_helper::TestOperand."""
    return {
        "type": "TestOperandType::" + operand.type.type,
        "dimensions": operand.type.dimensions,
        "scale": operand.type.scale,
        "zeroPoint": operand.type.zeroPoint,
        "numberOfConsumers": len(operand.outs),
        "lifetime": "TestOperandLifeTime::" + operand.lifetime,
        "channelQuant": GetSymmPerChannelQuantParams(operand.type.extraParams),
        "isIgnored": isinstance(operand, tg.IgnoredOutput),
        "data": "TestBuffer::createFromVector<{cpp_type}>({data})".format(
            cpp_type=operand.type.GetCppTypeString(),
            data=operand.GetListInitialization(),
        )
    }

def GetOperationStruct(operation):
    """Get the dictionary that corresponds to test_helper::TestOperation."""
    return {
        "type": "TestOperationType::" + operation.optype,
        "inputs": [op.model_index for op in operation.ins],
        "outputs": [op.model_index for op in operation.outs],
    }

def GetModelStruct(example):
    """Get the dictionary that corresponds to test_helper::TestModel."""
    return {
        "operands": [GetOperandStruct(op) for op in example.model.operands],
        "operations": [GetOperationStruct(op) for op in example.model.operations],
        "inputIndexes": [op.model_index for op in example.model.GetInputs()],
        "outputIndexes": [op.model_index for op in example.model.GetOutputs()],
        "isRelaxed": example.model.isRelaxed,
        "expectedMultinomialDistributionTolerance": example.expectedMultinomialDistributionTolerance
    }

def DumpCtsExample(example, example_fd):
    assert example.model.compiled
    template = """\
namespace generated_tests::{spec_name} {{

const TestModel& get_{example_name}() {{
    static TestModel model = {aggregate_init};
    return model;
}}

}}  // namespace generated_tests::{spec_name}
"""
    print(template.format(
            spec_name=tg.FileNames.specName,
            example_name=str(example.examplesName),
            aggregate_init=ToCpp(GetModelStruct(example), indent=4),
        ), file=example_fd)


CTS_TEST_TEMPLATE_HEAD = """
namespace {namespace} {{

const ::test_helper::TestModel& get_{examples_name}();
"""

GENERATED_TEST_DEF = """
TEST_F(GeneratedTests, {test_name}) {{ execute(get_{examples_name}()); }}
"""

DYNAMIC_OUTPUT_SHAPE_TEST_DEF = """
TEST_F(DynamicOutputShapeTest, {test_name}) {{ execute(get_{examples_name}()); }}
"""

GENERATED_VALIDATION_TEST_DEF = """
TEST_F(GeneratedValidationTests, {test_name}) {{ execute(get_{examples_name}()); }}
"""

COMPLIANCE_TEST_DEF = """
TEST_AVAILABLE_SINCE({version}, {test_name}, {namespace}::get_{examples_name}());
"""

CTS_TEST_TEMPLATE_TAIL = """
}} // namespace {namespace}
"""

def DumpCtsTest(example, test_fd):
    namespace = "generated_tests::{spec_name}".format(spec_name=tg.FileNames.specName)

    test_template = CTS_TEST_TEMPLATE_HEAD

    if example.expectFailure:
        test_template += GENERATED_VALIDATION_TEST_DEF
    else:
        test_template += GENERATED_TEST_DEF
        if example.testDynamicOutputShape:
            test_template += DYNAMIC_OUTPUT_SHAPE_TEST_DEF

    test_template += CTS_TEST_TEMPLATE_TAIL

    if example.model.version is not None and not example.expectFailure:
        test_template += COMPLIANCE_TEST_DEF

    print(test_template.format(
        test_name=str(example.testName),
        namespace=namespace,
        examples_name=str(example.examplesName),
        version=example.model.version), file=test_fd)

if __name__ == '__main__':
    ParseCmdLine()
    tg.Run(InitializeFiles=InitializeFiles,
           DumpExample=DumpCtsExample,
           DumpTest=DumpCtsTest)
