set(LIB_NAME de_opencv_dnn4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/dnn/all_layers.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/dict.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/dnn.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/dnn.inl.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/layer.details.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/layer.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/layer_reg.private.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/shape_utils.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/utils
        ${LIB_HEADER_DIR}/opencv2/dnn/utils/debug_utils.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/utils/inference_engine.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn/version.hpp
        ${LIB_HEADER_DIR}/opencv2/dnn.hpp
    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/backend.cpp
        ${LIB_SOURCE_DIR}/backend.hpp

        ${LIB_SOURCE_DIR}/caffe/caffe_importer.cpp
        ${LIB_SOURCE_DIR}/caffe/caffe_io.cpp
        ${LIB_SOURCE_DIR}/caffe/caffe_io.hpp
        ${LIB_SOURCE_DIR}/caffe/caffe_shrinker.cpp
        ${LIB_SOURCE_DIR}/caffe/glog_emulator.hpp
        ${LIB_SOURCE_DIR}/caffe/opencv-caffe.proto

        ${LIB_SOURCE_DIR}/cuda/activations.cu
        ${LIB_SOURCE_DIR}/cuda/activation_eltwise.cu
        ${LIB_SOURCE_DIR}/cuda/array.hpp
        ${LIB_SOURCE_DIR}/cuda/atomics.hpp
        ${LIB_SOURCE_DIR}/cuda/bbox_utils.hpp
        ${LIB_SOURCE_DIR}/cuda/bias_activation.cu
        ${LIB_SOURCE_DIR}/cuda/bias_activation_eltwise.cu
        ${LIB_SOURCE_DIR}/cuda/bias_eltwise_activation.cu
        ${LIB_SOURCE_DIR}/cuda/block_stride_range.hpp
        ${LIB_SOURCE_DIR}/cuda/concat.cu
        ${LIB_SOURCE_DIR}/cuda/crop_and_resize.cu
        ${LIB_SOURCE_DIR}/cuda/detection_output.cu
        ${LIB_SOURCE_DIR}/cuda/eltwise_activation.cu
        ${LIB_SOURCE_DIR}/cuda/eltwise_ops.cu
        ${LIB_SOURCE_DIR}/cuda/execution.hpp
        ${LIB_SOURCE_DIR}/cuda/fill_copy.cu
        ${LIB_SOURCE_DIR}/cuda/fp_conversion.cu
        ${LIB_SOURCE_DIR}/cuda/functors.hpp
        ${LIB_SOURCE_DIR}/cuda/grid_nms.cu
        ${LIB_SOURCE_DIR}/cuda/grid_stride_range.hpp
        ${LIB_SOURCE_DIR}/cuda/index_helpers.hpp
        ${LIB_SOURCE_DIR}/cuda/kernel_dispatcher.hpp
        ${LIB_SOURCE_DIR}/cuda/limits.hpp
        ${LIB_SOURCE_DIR}/cuda/math.hpp
        ${LIB_SOURCE_DIR}/cuda/max_unpooling.cu
        ${LIB_SOURCE_DIR}/cuda/memory.hpp
        ${LIB_SOURCE_DIR}/cuda/mvn.cu
        ${LIB_SOURCE_DIR}/cuda/normalize.cu
        ${LIB_SOURCE_DIR}/cuda/padding.cu
        ${LIB_SOURCE_DIR}/cuda/permute.cu
        ${LIB_SOURCE_DIR}/cuda/prior_box.cu
        ${LIB_SOURCE_DIR}/cuda/region.cu
        ${LIB_SOURCE_DIR}/cuda/resize.cu
        ${LIB_SOURCE_DIR}/cuda/roi_pooling.cu
        ${LIB_SOURCE_DIR}/cuda/scale_shift.cu
        ${LIB_SOURCE_DIR}/cuda/shortcut.cu
        ${LIB_SOURCE_DIR}/cuda/slice.cu
        ${LIB_SOURCE_DIR}/cuda/types.hpp
        ${LIB_SOURCE_DIR}/cuda/vector_traits.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/init.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cublas.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/error.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/event.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/memory.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/nvcc_defs.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/pointer.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/span.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/stream.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/tensor.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/tensor_ops.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/workspace.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/activation.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/convolution.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/cudnn.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/lrn.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/pooling.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/recurrent.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/softmax.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/transform.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/csl/cudnn/transpose_convolution.hpp

        ${LIB_SOURCE_DIR}/cuda4dnn/cxx_utils/is_iterator.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/cxx_utils/resizable_static_array.hpp

        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/activations.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/activation_eltwise.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/bias_activation.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/bias_activation_eltwise.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/bias_eltwise_activation.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/concat.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/crop_and_resize.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/detection_output.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/eltwise_activation.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/eltwise_ops.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/fill_copy.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/fp_conversion.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/grid_nms.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/max_unpooling.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/mvn.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/normalize.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/padding.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/permute.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/prior_box.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/region.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/resize.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/roi_pooling.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/scale_shift.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/shortcut.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/kernels/slice.hpp

        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/activation.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/batch_norm.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/concat.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/const.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/convolution.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/crop_and_resize.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/depth_space_ops.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/detection_output.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/eltwise.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/group_norm.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/inner_product.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/instance_norm.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/layer_norm.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/lrn.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/matmul.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/matmul_broadcast.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/max_unpooling.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/mvn.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/normalize_bbox.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/padding.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/permute.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/pooling.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/prior_box.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/recurrent_cells.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/region.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/reorg.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/reshape.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/resize.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/roi_pooling.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/scale_shift.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/shortcut.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/shuffle_channel.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/slice.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/softmax.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/split.hpp
        ${LIB_SOURCE_DIR}/cuda4dnn/primitives/transpose_convolution.hpp

        ${LIB_SOURCE_DIR}/darknet/darknet_importer.cpp
        ${LIB_SOURCE_DIR}/darknet/darknet_io.cpp
        ${LIB_SOURCE_DIR}/darknet/darknet_io.hpp
        ${LIB_SOURCE_DIR}/debug_utils.cpp
        ${LIB_SOURCE_DIR}/dnn.cpp
        ${LIB_SOURCE_DIR}/dnn_common.hpp
        ${LIB_SOURCE_DIR}/dnn_params.cpp
        ${LIB_SOURCE_DIR}/dnn_read.cpp
        ${LIB_SOURCE_DIR}/dnn_utils.cpp
        ${LIB_SOURCE_DIR}/factory.hpp
        ${LIB_SOURCE_DIR}/graph_simplifier.cpp
        ${LIB_SOURCE_DIR}/graph_simplifier.hpp
        ${LIB_SOURCE_DIR}/halide_scheduler.cpp
        ${LIB_SOURCE_DIR}/halide_scheduler.hpp
        ${LIB_SOURCE_DIR}/ie_ngraph.cpp
        ${LIB_SOURCE_DIR}/ie_ngraph.hpp
        ${LIB_SOURCE_DIR}/init.cpp

        ${LIB_SOURCE_DIR}/int8layers/batch_norm_layer.cpp
        ${LIB_SOURCE_DIR}/int8layers/convolution_layer.cpp
        ${LIB_SOURCE_DIR}/int8layers/elementwise_layers.cpp
        ${LIB_SOURCE_DIR}/int8layers/eltwise_layer.cpp
        ${LIB_SOURCE_DIR}/int8layers/fully_connected_layer.cpp
        ${LIB_SOURCE_DIR}/int8layers/layers_common.hpp
        ${LIB_SOURCE_DIR}/int8layers/layers_common.simd.hpp
        ${LIB_SOURCE_DIR}/int8layers/layers_rvp052.cpp
        ${LIB_SOURCE_DIR}/int8layers/layers_rvp052.hpp
        ${LIB_SOURCE_DIR}/int8layers/pooling_layer.cpp
        ${LIB_SOURCE_DIR}/int8layers/quantization_utils.cpp
        ${LIB_SOURCE_DIR}/int8layers/scale_layer.cpp
        ${LIB_SOURCE_DIR}/int8layers/softmax_layer.cpp
        ${LIB_SOURCE_DIR}/layer.cpp

        ${LIB_SOURCE_DIR}/layers/accum_layer.cpp
        ${LIB_SOURCE_DIR}/layers/arg_layer.cpp
        ${LIB_SOURCE_DIR}/layers/attention_layer.cpp
        ${LIB_SOURCE_DIR}/layers/batch_norm_layer.cpp
        ${LIB_SOURCE_DIR}/layers/blank_layer.cpp
        ${LIB_SOURCE_DIR}/layers/concat_layer.cpp
        ${LIB_SOURCE_DIR}/layers/const_layer.cpp
        ${LIB_SOURCE_DIR}/layers/convolution_layer.cpp
        ${LIB_SOURCE_DIR}/layers/correlation_layer.cpp

        ${LIB_SOURCE_DIR}/layers/cpu_kernels/convolution.cpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/convolution.hpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/conv_block.simd.hpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/conv_depthwise.cpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/conv_depthwise.simd.hpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/conv_winograd_f63.cpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/conv_winograd_f63.dispatch.cpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/conv_winograd_f63.simd.hpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/fast_gemm.cpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/fast_gemm.hpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/fast_gemm_kernels.default.hpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/fast_gemm_kernels.simd.hpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/fast_norm.cpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/fast_norm.hpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/softmax.cpp
        ${LIB_SOURCE_DIR}/layers/cpu_kernels/softmax.hpp

        ${LIB_SOURCE_DIR}/layers/crop_and_resize_layer.cpp
        ${LIB_SOURCE_DIR}/layers/cumsum_layer.cpp
        ${LIB_SOURCE_DIR}/layers/depth_space_ops_layer.cpp
        ${LIB_SOURCE_DIR}/layers/detection_output_layer.cpp
        ${LIB_SOURCE_DIR}/layers/einsum_layer.cpp
        ${LIB_SOURCE_DIR}/layers/elementwise_layers.cpp
        ${LIB_SOURCE_DIR}/layers/eltwise_layer.cpp
        ${LIB_SOURCE_DIR}/layers/expand_layer.cpp
        ${LIB_SOURCE_DIR}/layers/flatten_layer.cpp
        ${LIB_SOURCE_DIR}/layers/flow_warp_layer.cpp
        ${LIB_SOURCE_DIR}/layers/fully_connected_layer.cpp
        ${LIB_SOURCE_DIR}/layers/gather_elements_layer.cpp
        ${LIB_SOURCE_DIR}/layers/gather_layer.cpp
        ${LIB_SOURCE_DIR}/layers/gemm_layer.cpp
        ${LIB_SOURCE_DIR}/layers/group_norm_layer.cpp
        ${LIB_SOURCE_DIR}/layers/instance_norm_layer.cpp
        ${LIB_SOURCE_DIR}/layers/layers_common.cpp
        ${LIB_SOURCE_DIR}/layers/layers_common.hpp
        ${LIB_SOURCE_DIR}/layers/layers_common.simd.hpp
        ${LIB_SOURCE_DIR}/layers/layer_norm.cpp
        ${LIB_SOURCE_DIR}/layers/lrn_layer.cpp
        ${LIB_SOURCE_DIR}/layers/matmul_layer.cpp
        ${LIB_SOURCE_DIR}/layers/max_unpooling_layer.cpp
        ${LIB_SOURCE_DIR}/layers/mvn_layer.cpp
        ${LIB_SOURCE_DIR}/layers/nary_eltwise_layers.cpp
        ${LIB_SOURCE_DIR}/layers/normalize_bbox_layer.cpp
        ${LIB_SOURCE_DIR}/layers/not_implemented_layer.cpp
        ${LIB_SOURCE_DIR}/layers/padding_layer.cpp
        ${LIB_SOURCE_DIR}/layers/permute_layer.cpp
        ${LIB_SOURCE_DIR}/layers/pooling_layer.cpp
        ${LIB_SOURCE_DIR}/layers/prior_box_layer.cpp
        ${LIB_SOURCE_DIR}/layers/proposal_layer.cpp
        ${LIB_SOURCE_DIR}/layers/randomnormallike_layer.cpp
        ${LIB_SOURCE_DIR}/layers/recurrent_layers.cpp
        ${LIB_SOURCE_DIR}/layers/reduce_layer.cpp
        ${LIB_SOURCE_DIR}/layers/region_layer.cpp
        ${LIB_SOURCE_DIR}/layers/reorg_layer.cpp
        ${LIB_SOURCE_DIR}/layers/reshape_layer.cpp
        ${LIB_SOURCE_DIR}/layers/resize_layer.cpp
        ${LIB_SOURCE_DIR}/layers/scale_layer.cpp
        ${LIB_SOURCE_DIR}/layers/scatterND_layer.cpp
        ${LIB_SOURCE_DIR}/layers/scatter_layer.cpp
        ${LIB_SOURCE_DIR}/layers/shuffle_channel_layer.cpp
        ${LIB_SOURCE_DIR}/layers/slice_layer.cpp
        ${LIB_SOURCE_DIR}/layers/softmax_layer.cpp
        ${LIB_SOURCE_DIR}/layers/split_layer.cpp
        ${LIB_SOURCE_DIR}/layers/tile_layer.cpp
        ${LIB_SOURCE_DIR}/layers/topk_layer.cpp
        ${LIB_SOURCE_DIR}/layer_factory.cpp
        ${LIB_SOURCE_DIR}/layer_internals.hpp
        ${LIB_SOURCE_DIR}/legacy_backend.cpp
        ${LIB_SOURCE_DIR}/legacy_backend.hpp
        ${LIB_SOURCE_DIR}/math_utils.hpp
        ${LIB_SOURCE_DIR}/model.cpp
        ${LIB_SOURCE_DIR}/net.cpp
        ${LIB_SOURCE_DIR}/net_cann.cpp
        ${LIB_SOURCE_DIR}/net_impl.cpp
        ${LIB_SOURCE_DIR}/net_impl.hpp
        ${LIB_SOURCE_DIR}/net_impl_backend.cpp
        ${LIB_SOURCE_DIR}/net_impl_fuse.cpp
        ${LIB_SOURCE_DIR}/net_openvino.cpp
        ${LIB_SOURCE_DIR}/net_quantization.cpp
        ${LIB_SOURCE_DIR}/nms.cpp
        ${LIB_SOURCE_DIR}/nms.inl.hpp

        ${LIB_SOURCE_DIR}/ocl4dnn/include/common.hpp
        ${LIB_SOURCE_DIR}/ocl4dnn/include/default_kernel_config.hpp
        ${LIB_SOURCE_DIR}/ocl4dnn/include/math_functions.hpp
        ${LIB_SOURCE_DIR}/ocl4dnn/include/ocl4dnn.hpp

        ${LIB_SOURCE_DIR}/ocl4dnn/src/common.cpp
        ${LIB_SOURCE_DIR}/ocl4dnn/src/math_functions.cpp
        ${LIB_SOURCE_DIR}/ocl4dnn/src/ocl4dnn_conv_spatial.cpp
        ${LIB_SOURCE_DIR}/ocl4dnn/src/ocl4dnn_inner_product.cpp
        ${LIB_SOURCE_DIR}/ocl4dnn/src/ocl4dnn_lrn.cpp
        ${LIB_SOURCE_DIR}/ocl4dnn/src/ocl4dnn_pool.cpp
        ${LIB_SOURCE_DIR}/ocl4dnn/src/ocl4dnn_softmax.cpp

        ${LIB_SOURCE_DIR}/onnx/onnx_graph_simplifier.cpp
        ${LIB_SOURCE_DIR}/onnx/onnx_graph_simplifier.hpp
        ${LIB_SOURCE_DIR}/onnx/onnx_importer.cpp
        ${LIB_SOURCE_DIR}/onnx/opencv-onnx.proto

        ${LIB_SOURCE_DIR}/opencl/activations.cl
        ${LIB_SOURCE_DIR}/opencl/batchnorm.cl
        ${LIB_SOURCE_DIR}/opencl/col2im.cl
        ${LIB_SOURCE_DIR}/opencl/concat.cl
        ${LIB_SOURCE_DIR}/opencl/conv_layer_spatial.cl
        ${LIB_SOURCE_DIR}/opencl/conv_spatial_helper.cl
        ${LIB_SOURCE_DIR}/opencl/detection_output.cl
        ${LIB_SOURCE_DIR}/opencl/dummy.cl
        ${LIB_SOURCE_DIR}/opencl/eltwise.cl
        ${LIB_SOURCE_DIR}/opencl/gemm_buffer.cl
        ${LIB_SOURCE_DIR}/opencl/gemm_image.cl
        ${LIB_SOURCE_DIR}/opencl/im2col.cl
        ${LIB_SOURCE_DIR}/opencl/lrn.cl
        ${LIB_SOURCE_DIR}/opencl/math.cl
        ${LIB_SOURCE_DIR}/opencl/matvec_mul.cl
        ${LIB_SOURCE_DIR}/opencl/mvn.cl
        ${LIB_SOURCE_DIR}/opencl/ocl4dnn_lrn.cl
        ${LIB_SOURCE_DIR}/opencl/ocl4dnn_pooling.cl
        ${LIB_SOURCE_DIR}/opencl/permute.cl
        ${LIB_SOURCE_DIR}/opencl/pooling.cl
        ${LIB_SOURCE_DIR}/opencl/prior_box.cl
        ${LIB_SOURCE_DIR}/opencl/region.cl
        ${LIB_SOURCE_DIR}/opencl/slice.cl
        ${LIB_SOURCE_DIR}/opencl/softmax.cl
        ${LIB_SOURCE_DIR}/opencl/softmax_loss.cl
        ${LIB_SOURCE_DIR}/op_cann.cpp
        ${LIB_SOURCE_DIR}/op_cann.hpp
        ${LIB_SOURCE_DIR}/op_cuda.cpp
        ${LIB_SOURCE_DIR}/op_cuda.hpp
        ${LIB_SOURCE_DIR}/op_halide.cpp
        ${LIB_SOURCE_DIR}/op_halide.hpp
        ${LIB_SOURCE_DIR}/op_inf_engine.cpp
        ${LIB_SOURCE_DIR}/op_inf_engine.hpp
        ${LIB_SOURCE_DIR}/op_timvx.cpp
        ${LIB_SOURCE_DIR}/op_timvx.hpp
        ${LIB_SOURCE_DIR}/op_vkcom.cpp
        ${LIB_SOURCE_DIR}/op_vkcom.hpp
        ${LIB_SOURCE_DIR}/op_webnn.cpp
        ${LIB_SOURCE_DIR}/op_webnn.hpp
        ${LIB_SOURCE_DIR}/plugin_api.hpp
        ${LIB_SOURCE_DIR}/plugin_wrapper.impl.hpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/registry.cpp

        ${LIB_SOURCE_DIR}/tensorflow/attr_value.proto
        ${LIB_SOURCE_DIR}/tensorflow/function.proto
        ${LIB_SOURCE_DIR}/tensorflow/graph.proto
        ${LIB_SOURCE_DIR}/tensorflow/op_def.proto
        ${LIB_SOURCE_DIR}/tensorflow/tensor.proto
        ${LIB_SOURCE_DIR}/tensorflow/tensor_shape.proto
        ${LIB_SOURCE_DIR}/tensorflow/tf_graph_simplifier.cpp
        ${LIB_SOURCE_DIR}/tensorflow/tf_graph_simplifier.hpp
        ${LIB_SOURCE_DIR}/tensorflow/tf_importer.cpp
        ${LIB_SOURCE_DIR}/tensorflow/tf_io.cpp
        ${LIB_SOURCE_DIR}/tensorflow/tf_io.hpp
        ${LIB_SOURCE_DIR}/tensorflow/types.proto
        ${LIB_SOURCE_DIR}/tensorflow/versions.proto

        ${LIB_SOURCE_DIR}/tflite/builtin_op_data.h
        ${LIB_SOURCE_DIR}/tflite/schema.fbs
        ${LIB_SOURCE_DIR}/tflite/tflite_importer.cpp

        # ${LIB_SOURCE_DIR}/torch/COPYRIGHT.txt
        ${LIB_SOURCE_DIR}/torch/THDiskFile.cpp
        ${LIB_SOURCE_DIR}/torch/THDiskFile.h
        ${LIB_SOURCE_DIR}/torch/THFile.cpp
        ${LIB_SOURCE_DIR}/torch/THFile.h
        ${LIB_SOURCE_DIR}/torch/THFilePrivate.h
        ${LIB_SOURCE_DIR}/torch/THGeneral.cpp
        ${LIB_SOURCE_DIR}/torch/THGeneral.h
        ${LIB_SOURCE_DIR}/torch/torch_importer.cpp

        ${LIB_SOURCE_DIR}/vkcom/include/buffer.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/command.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/context.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/fence.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/op_base.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/op_conv.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/op_matmul.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/op_naryeltwise.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/pipeline.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/tensor.hpp
        ${LIB_SOURCE_DIR}/vkcom/include/vkcom.hpp

        ${LIB_SOURCE_DIR}/vkcom/shader/conv_1x1_fast.comp
        ${LIB_SOURCE_DIR}/vkcom/shader/conv_1x1_fast_spv.cpp
        ${LIB_SOURCE_DIR}/vkcom/shader/conv_depthwise.comp
        ${LIB_SOURCE_DIR}/vkcom/shader/conv_depthwise_3x3.comp
        ${LIB_SOURCE_DIR}/vkcom/shader/conv_depthwise_3x3_spv.cpp
        ${LIB_SOURCE_DIR}/vkcom/shader/conv_depthwise_spv.cpp
        ${LIB_SOURCE_DIR}/vkcom/shader/conv_implicit_gemm.comp
        ${LIB_SOURCE_DIR}/vkcom/shader/conv_implicit_gemm_spv.cpp
        ${LIB_SOURCE_DIR}/vkcom/shader/gemm.comp
        ${LIB_SOURCE_DIR}/vkcom/shader/gemm_spv.cpp
        ${LIB_SOURCE_DIR}/vkcom/shader/nary_eltwise_binary_forward.comp
        ${LIB_SOURCE_DIR}/vkcom/shader/nary_eltwise_binary_forward_spv.cpp
        # ${LIB_SOURCE_DIR}/vkcom/shader/spirv_generator.py
        ${LIB_SOURCE_DIR}/vkcom/shader/spv_shader.cpp
        ${LIB_SOURCE_DIR}/vkcom/shader/spv_shader.hpp

        ${LIB_SOURCE_DIR}/vkcom/src/buffer.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/command.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/context.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/fence.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/internal.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/internal.hpp
        ${LIB_SOURCE_DIR}/vkcom/src/op_base.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/op_conv.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/op_matmul.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/op_naryEltwise.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/pipeline.cpp
        ${LIB_SOURCE_DIR}/vkcom/src/tensor.cpp

        ${LIB_SOURCE_DIR}/vkcom/vulkan/function_list.inl.hpp
        ${LIB_SOURCE_DIR}/vkcom/vulkan/vk_functions.cpp
        ${LIB_SOURCE_DIR}/vkcom/vulkan/vk_functions.hpp
        ${LIB_SOURCE_DIR}/vkcom/vulkan/vk_loader.cpp
        ${LIB_SOURCE_DIR}/vkcom/vulkan/vk_loader.hpp

        # ${LIB_SOURCE_DIR}/webnn/README.md

        # ${LIB_SOURCE_DIR}/_benni/int8layers/layers_common.avx2.cpp
        # ${LIB_SOURCE_DIR}/_benni/int8layers/layers_common.avx512_skx.cpp
        # ${LIB_SOURCE_DIR}/_benni/int8layers/layers_common.lasx.cpp
        # ${LIB_SOURCE_DIR}/_benni/int8layers/layers_common.neon.cpp
        # ${LIB_SOURCE_DIR}/_benni/int8layers/layers_common.rvv.cpp
        ${LIB_SOURCE_DIR}/_benni/int8layers/layers_common.simd_declarations.hpp

        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_block.avx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_block.avx2.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_block.neon.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_block.neon_fp16.cpp
        ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_block.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_depthwise.avx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_depthwise.avx2.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_depthwise.lasx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_depthwise.rvv.cpp
        ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_depthwise.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_winograd_f63.avx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_winograd_f63.avx2.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_winograd_f63.neon.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_winograd_f63.neon_fp16.cpp
        ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/conv_winograd_f63.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/fast_gemm_kernels.avx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/fast_gemm_kernels.avx2.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/fast_gemm_kernels.lasx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/fast_gemm_kernels.neon.cpp
        ${LIB_SOURCE_DIR}/_benni/layers/cpu_kernels/fast_gemm_kernels.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/layers/layers_common.avx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/layers_common.avx2.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/layers_common.avx512_skx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/layers_common.lasx.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/layers_common.neon.cpp
        # ${LIB_SOURCE_DIR}/_benni/layers/layers_common.rvv.cpp
        ${LIB_SOURCE_DIR}/_benni/layers/layers_common.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/layers/layers_common.sve.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_dnn.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_dnn.hpp

    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC JSON_DLL_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/_benni)

    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_MINIGMP=1)
    target_compile_definitions(${LIB_NAME} PRIVATE __OPENCV_BUILD=1)

    # target_compile_options(${LIB_NAME} PRIVATE -Wall -Wconversion -Wshadow -Wextra)
    # not yet ready for -Wsign-conversion
    # target_compile_options(${LIB_NAME} PRIVATE -Wpedantic)
    # target_compile_options(${LIB_NAME} PRIVATE -Werror=conversion)

    # check_type_size(lconv LCONV_SIZE LANGUAGE CXX) # Should be 88 bytes?

    # target_compile_definitions(${LIB_NAME} PUBLIC JSONCPP_NO_LOCALE_SUPPORT=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_core4130)
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_imgproc4130)
    # target_link_libraries(${LIB_NAME} PUBLIC de_opencl_loader_shared)

endif()

# add_subdirectory(test)
