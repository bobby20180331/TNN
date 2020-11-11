// Tencent is pleased to support the open source community by making TNN available.
//
// Copyright (C) 2020 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include <algorithm>
#include <cmath>
#include "tnn/device/cpu/acc/cpu_layer_acc.h"
#include "tnn/utils/dims_vector_utils.h"
#include "tnn/utils/dims_offset_utils.h"

namespace TNN_NS {

DECLARE_CPU_ACC(StrideSliceV2, LAYER_STRIDED_SLICE_V2);

Status CpuStrideSliceV2LayerAcc::Reshape(const std::vector<Blob *> &inputs, const std::vector<Blob *> &outputs) {
    return TNN_OK;
}

Status CpuStrideSliceV2LayerAcc::Forward(const std::vector<Blob *> &inputs, const std::vector<Blob *> &outputs) {
    auto layer_param = dynamic_cast<StrideSliceV2LayerParam *>(param_);
    if (!layer_param) {
        LOGE("Error: StrideSliceLayerParam is nil\n");
        return Status(TNNERR_MODEL_ERR, "Error: StrideSliceLayerParam is nil");
    }

    Blob *input_blob  = inputs[0];
    Blob *output_blob = outputs[0];

    auto begins = layer_param->begins;
    auto ends = layer_param->ends;
    auto strides = layer_param->strides;
    auto axes = layer_param->axes;

    DimsVector input_dims = input_blob->GetBlobDesc().dims;
    DimsVector output_dims = output_blob->GetBlobDesc().dims;
    int output_count = DimsVectorUtils::Count(output_dims);

    if (output_blob->GetBlobDesc().data_type != DATA_TYPE_INT8) {
        float *input_data  = static_cast<float *>(input_blob->GetHandle().base);
        float *output_data = static_cast<float *>(output_blob->GetHandle().base);
        for(int offset = 0; offset < output_count; ++offset) {
            DimsVector output_index = DimsOffsetUtils::ConvertOffsetToIndex(output_dims, offset);
            DimsVector input_index;
            int axes_index = 0;
            for(int i = 0; i < output_index.size(); ++i) {
                if(axes_index < axes.size() && i == axes[axes_index]) {
                    input_index.push_back(begins[axes_index] + output_index[i] * strides[axes_index]);
                    ++axes_index;
                } else {
                    input_index.push_back(output_index[i]);
                }
            }
            int in_offset = DimsOffsetUtils::ConvertIndexToOffset(input_dims, input_index);
            output_data[offset] = input_data[in_offset];
        }
    } else {
        ASSERT(0);
    }
    return TNN_OK;
}

REGISTER_CPU_ACC(StrideSliceV2, LAYER_STRIDED_SLICE_V2);

}  // namespace TNN_NS
