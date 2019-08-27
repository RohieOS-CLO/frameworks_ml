#
# Copyright (C) 2018 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

layout = BoolScalar("layout", False) # NHWC

# TEST 1: dilation set to 1 (default)
i1 = Input("op1", "TENSOR_FLOAT32", "{1, 3, 3, 1}")
f1 = Parameter("op2", "TENSOR_FLOAT32", "{1, 2, 2, 1}", [.25, .25, .25, .25])
b1 = Parameter("op3", "TENSOR_FLOAT32", "{1}", [0])
o1 = Output("op4", "TENSOR_FLOAT32", "{1, 2, 2, 1}")
Model().Operation("CONV_2D", i1, f1, b1, 0, 0, 0, 0, 1, 1, 0, layout, 1, 1).To(o1)

# Additional data type
quant8 = DataTypeConverter().Identify({
    i1: ("TENSOR_QUANT8_ASYMM", 0.5, 0),
    f1: ("TENSOR_QUANT8_ASYMM", 0.125, 0),
    b1: ("TENSOR_INT32", 0.0625, 0),
    o1: ("TENSOR_QUANT8_ASYMM", 0.125, 0)
})

# Instantiate an example
example = Example({
    i1: [1.0, 1.0, 1.0, 1.0, 0.5, 1.0, 1.0, 1.0, 1.0],
    o1: [.875, .875, .875, .875]
}).AddNchw(i1, o1, layout).AddVariations("relaxed", quant8, "float16")


# TEST 2: dilation set to 3
i2 = Input("op1", "TENSOR_FLOAT32", "{1, 9, 9, 1}")
f2 = Parameter("op2", "TENSOR_FLOAT32", "{1, 3, 3, 1}", [1, 2, 3, 4, 5, 6, 7, 8, 9])
b2 = Parameter("op3", "TENSOR_FLOAT32", "{1}", [0])
o2 = Output("op4", "TENSOR_FLOAT32", "{1, 3, 3, 1}")
Model().Operation("CONV_2D", i2, f2, b2, 0, 0, 0, 0, 1, 1, 0, layout, 3, 3).To(o2)

# Additional data type
quant8 = DataTypeConverter().Identify({
    i2: ("TENSOR_QUANT8_ASYMM", 0.5, 0),
    f2: ("TENSOR_QUANT8_ASYMM", 0.125, 0),
    b2: ("TENSOR_INT32", 0.0625, 0),
    o2: ("TENSOR_QUANT8_ASYMM", 0.125, 0)
})

# Instantiate an example
example = Example({
    i2: [0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 1, 1, 1, 0, 0, 0,
         0, 0, 0, 1, 1, 1, 0, 0, 0,
         0, 0, 0, 1, 1, 1, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0],
    o2: [5, 5, 5, 5, 5, 5, 5, 5, 5]
}).AddNchw(i2, o2, layout).AddVariations("relaxed", quant8, "float16")

# TEST 3: same as test 1 but with implicit VALID padding
i1 = Input("op1", "TENSOR_FLOAT32", "{1, 3, 3, 1}")
f1 = Parameter("op2", "TENSOR_FLOAT32", "{1, 2, 2, 1}", [.25, .25, .25, .25])
b1 = Parameter("op3", "TENSOR_FLOAT32", "{1}", [0])
o1 = Output("op4", "TENSOR_FLOAT32", "{1, 2, 2, 1}")
Model().Operation("CONV_2D", i1, f1, b1, 2, 1, 1, 0, layout, 1, 1).To(o1)

# Additional data type
quant8 = DataTypeConverter().Identify({
    i1: ("TENSOR_QUANT8_ASYMM", 0.5, 0),
    f1: ("TENSOR_QUANT8_ASYMM", 0.125, 0),
    b1: ("TENSOR_INT32", 0.0625, 0),
    o1: ("TENSOR_QUANT8_ASYMM", 0.125, 0)
})

# Instantiate an example
example = Example({
    i1: [1.0, 1.0, 1.0, 1.0, 0.5, 1.0, 1.0, 1.0, 1.0],
    o1: [.875, .875, .875, .875]
}, name="valid_padding").AddNchw(i1, o1, layout).AddVariations("relaxed", quant8, "float16")


# TEST 4: same as test 2 but with implicit VALID padding
i2 = Input("op1", "TENSOR_FLOAT32", "{1, 9, 9, 1}")
f2 = Parameter("op2", "TENSOR_FLOAT32", "{1, 3, 3, 1}", [1, 2, 3, 4, 5, 6, 7, 8, 9])
b2 = Parameter("op3", "TENSOR_FLOAT32", "{1}", [0])
o2 = Output("op4", "TENSOR_FLOAT32", "{1, 3, 3, 1}")
Model().Operation("CONV_2D", i2, f2, b2, 2, 1, 1, 0, layout, 3, 3).To(o2)

# Additional data type
quant8 = DataTypeConverter().Identify({
    i2: ("TENSOR_QUANT8_ASYMM", 0.5, 0),
    f2: ("TENSOR_QUANT8_ASYMM", 0.125, 0),
    b2: ("TENSOR_INT32", 0.0625, 0),
    o2: ("TENSOR_QUANT8_ASYMM", 0.125, 0)
})

# Instantiate an example
example = Example({
    i2: [0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 1, 1, 1, 0, 0, 0,
         0, 0, 0, 1, 1, 1, 0, 0, 0,
         0, 0, 0, 1, 1, 1, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0],
    o2: [5, 5, 5, 5, 5, 5, 5, 5, 5]
}, name="valid_padding").AddNchw(i2, o2, layout).AddVariations("relaxed", quant8, "float16")


# TEST 5: dilation set to 3, SAME padding
i3 = Input("op1", "TENSOR_FLOAT32", "{1, 6, 6, 1}")
f3 = Parameter("op2", "TENSOR_FLOAT32", "{1, 2, 2, 1}", [1, 2, 3, 4])
b3 = Parameter("op3", "TENSOR_FLOAT32", "{1}", [0])
o3 = Output("op4", "TENSOR_FLOAT32", "{1, 3, 3, 1}")
Model().Operation("CONV_2D", i3, f3, b3, 1, 2, 2, 0, layout, 3, 3).To(o3)

# Additional data type
quant8 = DataTypeConverter().Identify({
    i3: ("TENSOR_QUANT8_ASYMM", 0.5, 0),
    f3: ("TENSOR_QUANT8_ASYMM", 0.125, 0),
    b3: ("TENSOR_INT32", 0.0625, 0),
    o3: ("TENSOR_QUANT8_ASYMM", 0.125, 0)
})

# Instantiate an example
example = Example({
    i3: [0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0,
         0, 0, 4, 3, 0, 0,
         0, 0, 2, 1, 0, 0,
         0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0],
    o3: [16, 0, 9, 0, 0, 0, 4, 0, 1]
}).AddNchw(i3, o3, layout).AddVariations("relaxed", quant8, "float16")
