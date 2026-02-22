set(LIB_NAME de_opencv_gapi4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/gapi/core.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/cpu/core.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/cpu/gcpukernel.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/cpu/imgproc.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/cpu/ot.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/cpu/stereo.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/cpu/video.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/fluid/core.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/fluid/gfluidbuffer.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/fluid/gfluidkernel.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/fluid/imgproc.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/garg.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/garray.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gasync_context.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gcall.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gcommon.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gcompiled.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gcompiled_async.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gcompoundkernel.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gcomputation.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gcomputation_async.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gframe.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gkernel.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gmat.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gmetaarg.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gopaque.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gproto.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/gpu/core.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gpu/ggpukernel.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gpu/imgproc.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gscalar.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gstreaming.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gtransform.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gtyped.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/gtype_traits.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/imgproc.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/infer/bindings_ie.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/infer/bindings_onnx.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/infer/bindings_ov.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/infer/ie.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/infer/onnx.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/infer/ov.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/infer/parsers.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/infer/workload_type.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/infer.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/media.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/oak/infer.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/oak/oak.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/ocl/core.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/ocl/goclkernel.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/ocl/imgproc.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/opencv_includes.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/operators.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/ot.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/own/assert.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/own/convert.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/own/cvdefs.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/own/exports.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/own/mat.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/own/saturate.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/own/scalar.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/own/types.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/plaidml/core.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/plaidml/gplaidmlkernel.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/plaidml/plaidml.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/pysrc/python_stream_source.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/python/python.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/render/render.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/render/render_types.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/render.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/rmat.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/s11n/base.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/s11n.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/stereo.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/cap.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/desync.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/format.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/gstreamer/gstreamerpipeline.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/gstreamer/gstreamersource.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/meta.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/onevpl/accel_types.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/onevpl/cfg_params.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/onevpl/data_provider_interface.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/onevpl/default.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/onevpl/device_selector_interface.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/onevpl/source.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/queue_source.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/source.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/streaming/sync.hpp

        ${LIB_HEADER_DIR}/opencv2/gapi/util/any.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/util/compiler_hints.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/util/copy_through_move.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/util/optional.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/util/throw.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/util/type_traits.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/util/util.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/util/variant.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi/video.hpp
        ${LIB_HEADER_DIR}/opencv2/gapi.hpp
    )
    set(LIB_SOURCES
        # ${LIB_SOURCE_DIR}/_benni/backends/fluid/gfluidcore_func.avx2.cpp
        ${LIB_SOURCE_DIR}/_benni/backends/fluid/gfluidcore_func.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/backends/fluid/gfluidcore_func.sse4_1.cpp
        # ${LIB_SOURCE_DIR}/_benni/backends/fluid/gfluidimgproc_func.avx2.cpp
        ${LIB_SOURCE_DIR}/_benni/backends/fluid/gfluidimgproc_func.simd_declarations.hpp
        # ${LIB_SOURCE_DIR}/_benni/backends/fluid/gfluidimgproc_func.sse4_1.cpp

        ${LIB_SOURCE_DIR}/3rdparty/vasot/include/vas/common.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/include/vas/ot.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/common/exception.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/common/prof.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/kalman_filter/kalman_filter_no_opencv.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/kalman_filter/kalman_filter_no_opencv.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/mtt/hungarian_wrap.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/mtt/hungarian_wrap.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/mtt/objects_associator.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/mtt/objects_associator.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/mtt/rgb_histogram.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/mtt/rgb_histogram.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/object_tracker.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/prof_def.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/short_term_imageless_tracker.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/short_term_imageless_tracker.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/tracker.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/tracker.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/tracklet.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/tracklet.hpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/zero_term_imageless_tracker.cpp
        ${LIB_SOURCE_DIR}/3rdparty/vasot/src/components/ot/zero_term_imageless_tracker.hpp
        ${LIB_SOURCE_DIR}/api/garray.cpp
        ${LIB_SOURCE_DIR}/api/gbackend.cpp
        ${LIB_SOURCE_DIR}/api/gbackend_priv.hpp
        ${LIB_SOURCE_DIR}/api/gcall.cpp
        ${LIB_SOURCE_DIR}/api/gcall_priv.hpp
        ${LIB_SOURCE_DIR}/api/gcommon.cpp
        ${LIB_SOURCE_DIR}/api/gcomputation.cpp
        ${LIB_SOURCE_DIR}/api/gcomputation_priv.hpp
        ${LIB_SOURCE_DIR}/api/gframe.cpp
        ${LIB_SOURCE_DIR}/api/ginfer.cpp
        ${LIB_SOURCE_DIR}/api/gkernel.cpp
        ${LIB_SOURCE_DIR}/api/gmat.cpp
        ${LIB_SOURCE_DIR}/api/gnode.cpp
        ${LIB_SOURCE_DIR}/api/gnode.hpp
        ${LIB_SOURCE_DIR}/api/gnode_priv.hpp
        ${LIB_SOURCE_DIR}/api/gopaque.cpp
        ${LIB_SOURCE_DIR}/api/gorigin.cpp
        ${LIB_SOURCE_DIR}/api/gorigin.hpp
        ${LIB_SOURCE_DIR}/api/gproto.cpp
        ${LIB_SOURCE_DIR}/api/gproto_priv.hpp
        ${LIB_SOURCE_DIR}/api/grunarg.cpp
        ${LIB_SOURCE_DIR}/api/gscalar.cpp
        ${LIB_SOURCE_DIR}/api/kernels_core.cpp
        ${LIB_SOURCE_DIR}/api/kernels_imgproc.cpp
        ${LIB_SOURCE_DIR}/api/kernels_nnparsers.cpp
        ${LIB_SOURCE_DIR}/api/kernels_ot.cpp
        ${LIB_SOURCE_DIR}/api/kernels_stereo.cpp
        ${LIB_SOURCE_DIR}/api/kernels_streaming.cpp
        ${LIB_SOURCE_DIR}/api/kernels_video.cpp
        ${LIB_SOURCE_DIR}/api/media.cpp
        ${LIB_SOURCE_DIR}/api/ocv_mask_creator.hpp
        ${LIB_SOURCE_DIR}/api/operators.cpp
        ${LIB_SOURCE_DIR}/api/render.cpp
        ${LIB_SOURCE_DIR}/api/render_ocv.cpp
        ${LIB_SOURCE_DIR}/api/render_ocv.hpp
        ${LIB_SOURCE_DIR}/api/render_priv.hpp
        ${LIB_SOURCE_DIR}/api/rmat.cpp
        ${LIB_SOURCE_DIR}/api/s11n.cpp
        ${LIB_SOURCE_DIR}/backends/common/gbackend.hpp
        ${LIB_SOURCE_DIR}/backends/common/gcompoundbackend.cpp
        ${LIB_SOURCE_DIR}/backends/common/gcompoundkernel.cpp
        ${LIB_SOURCE_DIR}/backends/common/gmetabackend.cpp
        ${LIB_SOURCE_DIR}/backends/common/gmetabackend.hpp
        ${LIB_SOURCE_DIR}/backends/common/serialization.cpp
        ${LIB_SOURCE_DIR}/backends/common/serialization.hpp
        ${LIB_SOURCE_DIR}/backends/cpu/gcpubackend.cpp
        ${LIB_SOURCE_DIR}/backends/cpu/gcpubackend.hpp
        ${LIB_SOURCE_DIR}/backends/cpu/gcpucore.cpp
        ${LIB_SOURCE_DIR}/backends/cpu/gcpuimgproc.cpp
        ${LIB_SOURCE_DIR}/backends/cpu/gcpukernel.cpp
        ${LIB_SOURCE_DIR}/backends/cpu/gcpuot.cpp
        ${LIB_SOURCE_DIR}/backends/cpu/gcpustereo.cpp
        ${LIB_SOURCE_DIR}/backends/cpu/gcpuvideo.cpp
        ${LIB_SOURCE_DIR}/backends/cpu/gnnparsers.cpp
        ${LIB_SOURCE_DIR}/backends/cpu/gnnparsers.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidbackend.cpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidbackend.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidbuffer.cpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidbuffer_priv.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidcore.cpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidcore_func.dispatch.cpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidcore_func.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidcore_func.simd.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidcore_simd_sse41.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidimgproc.cpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidimgproc_func.dispatch.cpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidimgproc_func.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidimgproc_func.simd.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidimgproc_simd_avx2.hpp
        ${LIB_SOURCE_DIR}/backends/fluid/gfluidutils.hpp
        ${LIB_SOURCE_DIR}/backends/ie/bindings_ie.cpp

        ${LIB_SOURCE_DIR}/backends/ie/giebackend/giewrapper.cpp
        ${LIB_SOURCE_DIR}/backends/ie/giebackend/giewrapper.hpp
        ${LIB_SOURCE_DIR}/backends/ie/giebackend.cpp
        ${LIB_SOURCE_DIR}/backends/ie/giebackend.hpp
        ${LIB_SOURCE_DIR}/backends/ie/util.hpp
        ${LIB_SOURCE_DIR}/backends/oak/goak.cpp
        ${LIB_SOURCE_DIR}/backends/oak/goakbackend.cpp
        ${LIB_SOURCE_DIR}/backends/oak/goak_memory_adapters.cpp
        ${LIB_SOURCE_DIR}/backends/oak/oak_memory_adapters.hpp
        ${LIB_SOURCE_DIR}/backends/ocl/goclbackend.cpp
        ${LIB_SOURCE_DIR}/backends/ocl/goclbackend.hpp
        ${LIB_SOURCE_DIR}/backends/ocl/goclcore.cpp
        ${LIB_SOURCE_DIR}/backends/ocl/goclcore.hpp
        ${LIB_SOURCE_DIR}/backends/ocl/goclimgproc.cpp
        ${LIB_SOURCE_DIR}/backends/ocl/goclimgproc.hpp
        ${LIB_SOURCE_DIR}/backends/ocl/goclkernel.cpp
        ${LIB_SOURCE_DIR}/backends/onnx/bindings_onnx.cpp
        ${LIB_SOURCE_DIR}/backends/onnx/coreml_ep.cpp
        ${LIB_SOURCE_DIR}/backends/onnx/coreml_ep.hpp
        ${LIB_SOURCE_DIR}/backends/onnx/dml_ep.cpp
        ${LIB_SOURCE_DIR}/backends/onnx/dml_ep.hpp
        ${LIB_SOURCE_DIR}/backends/onnx/gonnxbackend.cpp
        ${LIB_SOURCE_DIR}/backends/onnx/gonnxbackend.hpp
        ${LIB_SOURCE_DIR}/backends/ov/bindings_ov.cpp
        ${LIB_SOURCE_DIR}/backends/ov/govbackend.cpp
        ${LIB_SOURCE_DIR}/backends/ov/govbackend.hpp
        ${LIB_SOURCE_DIR}/backends/ov/util.hpp
        ${LIB_SOURCE_DIR}/backends/plaidml/gplaidmlbackend.cpp
        ${LIB_SOURCE_DIR}/backends/plaidml/gplaidmlbackend.hpp
        ${LIB_SOURCE_DIR}/backends/plaidml/gplaidmlcore.cpp
        ${LIB_SOURCE_DIR}/backends/plaidml/plaidml_util.hpp
        ${LIB_SOURCE_DIR}/backends/python/gpythonbackend.cpp
        ${LIB_SOURCE_DIR}/backends/render/ft_render.cpp
        ${LIB_SOURCE_DIR}/backends/render/ft_render.hpp
        ${LIB_SOURCE_DIR}/backends/render/ft_render_priv.hpp
        ${LIB_SOURCE_DIR}/backends/render/grenderocv.cpp
        ${LIB_SOURCE_DIR}/backends/streaming/gstreamingbackend.cpp
        ${LIB_SOURCE_DIR}/backends/streaming/gstreamingbackend.hpp
        ${LIB_SOURCE_DIR}/backends/streaming/gstreamingkernel.hpp
        ${LIB_SOURCE_DIR}/compiler/gcompiled.cpp
        ${LIB_SOURCE_DIR}/compiler/gcompiled_priv.hpp
        ${LIB_SOURCE_DIR}/compiler/gcompiler.cpp
        ${LIB_SOURCE_DIR}/compiler/gcompiler.hpp
        ${LIB_SOURCE_DIR}/compiler/gislandmodel.cpp
        ${LIB_SOURCE_DIR}/compiler/gislandmodel.hpp
        ${LIB_SOURCE_DIR}/compiler/gmodel.cpp
        ${LIB_SOURCE_DIR}/compiler/gmodel.hpp
        ${LIB_SOURCE_DIR}/compiler/gmodelbuilder.cpp
        ${LIB_SOURCE_DIR}/compiler/gmodelbuilder.hpp
        ${LIB_SOURCE_DIR}/compiler/gmodel_priv.hpp
        ${LIB_SOURCE_DIR}/compiler/gobjref.hpp
        ${LIB_SOURCE_DIR}/compiler/gstreaming.cpp
        ${LIB_SOURCE_DIR}/compiler/gstreaming_priv.hpp
        ${LIB_SOURCE_DIR}/compiler/passes/dump_dot.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/exec.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/helpers.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/helpers.hpp
        ${LIB_SOURCE_DIR}/compiler/passes/intrin.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/islands.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/kernels.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/meta.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/passes.hpp
        ${LIB_SOURCE_DIR}/compiler/passes/pattern_matching.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/pattern_matching.hpp
        ${LIB_SOURCE_DIR}/compiler/passes/perform_substitution.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/streaming.cpp
        ${LIB_SOURCE_DIR}/compiler/passes/transformations.cpp
        ${LIB_SOURCE_DIR}/compiler/transactions.hpp
        ${LIB_SOURCE_DIR}/executor/conc_queue.hpp
        ${LIB_SOURCE_DIR}/executor/gabstractexecutor.cpp
        ${LIB_SOURCE_DIR}/executor/gabstractexecutor.hpp
        ${LIB_SOURCE_DIR}/executor/gabstractstreamingexecutor.cpp
        ${LIB_SOURCE_DIR}/executor/gabstractstreamingexecutor.hpp
        ${LIB_SOURCE_DIR}/executor/gasync.cpp
        ${LIB_SOURCE_DIR}/executor/gexecutor.cpp
        ${LIB_SOURCE_DIR}/executor/gexecutor.hpp
        ${LIB_SOURCE_DIR}/executor/gstreamingexecutor.cpp
        ${LIB_SOURCE_DIR}/executor/gstreamingexecutor.hpp
        ${LIB_SOURCE_DIR}/executor/gtbbexecutor.cpp
        ${LIB_SOURCE_DIR}/executor/gtbbexecutor.hpp
        ${LIB_SOURCE_DIR}/executor/gthreadedexecutor.cpp
        ${LIB_SOURCE_DIR}/executor/gthreadedexecutor.hpp
        ${LIB_SOURCE_DIR}/executor/last_value.hpp
        ${LIB_SOURCE_DIR}/executor/thread_pool.cpp
        ${LIB_SOURCE_DIR}/executor/thread_pool.hpp
        ${LIB_SOURCE_DIR}/logger.hpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/pysrc/python_stream_source.cpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamerenv.cpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamerenv.hpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamerpipeline.cpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamerpipeline_priv.hpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamerptr.hpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamersource.cpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamersource_priv.hpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamer_buffer_utils.cpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamer_buffer_utils.hpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamer_media_adapter.cpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamer_media_adapter.hpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamer_pipeline_facade.cpp
        ${LIB_SOURCE_DIR}/streaming/gstreamer/gstreamer_pipeline_facade.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/accel_policy_cpu.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/accel_policy_cpu.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/accel_policy_dx11.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/accel_policy_dx11.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/accel_policy_interface.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/accel_policy_va_api.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/accel_policy_va_api.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/dx11_alloc_resource.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/dx11_alloc_resource.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/base_frame_adapter.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/base_frame_adapter.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/cpu_frame_adapter.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/cpu_frame_adapter.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/dx11_frame_adapter.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/dx11_frame_adapter.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/surface.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/surface.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/surface_pool.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/surface/surface_pool.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/utils/elastic_barrier.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/utils/shared_lock.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/accelerators/utils/shared_lock.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/cfg_params.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/cfg_params_parser.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/cfg_params_parser.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/cfg_param_device_selector.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/cfg_param_device_selector.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/data_provider_defines.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/data_provider_dispatcher.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/data_provider_dispatcher.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/data_provider_interface_exception.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/default.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/demux/async_mfp_demux_data_provider.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/demux/async_mfp_demux_data_provider.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/device_selector_interface.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/decode/decode_engine_legacy.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/decode/decode_engine_legacy.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/decode/decode_session.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/decode/decode_session.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/engine_session.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/engine_session.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/preproc_dispatcher.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/preproc_dispatcher.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/preproc_engine.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/preproc_engine.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/preproc_session.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/preproc_session.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/utils.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/utils.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc/vpp_preproc_defines.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc_defines.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc_engine_interface.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/preproc_engine_interface.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/processing_engine_base.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/processing_engine_base.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/transcode/transcode_engine_legacy.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/transcode/transcode_engine_legacy.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/transcode/transcode_session.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/engine/transcode/transcode_session.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/file_data_provider.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/file_data_provider.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/onevpl_export.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/source.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/source_priv.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/source_priv.hpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/utils.cpp
        ${LIB_SOURCE_DIR}/streaming/onevpl/utils.hpp
        ${LIB_SOURCE_DIR}/streaming/queue_source.cpp
        ${LIB_SOURCE_DIR}/utils/itt.cpp
        ${LIB_SOURCE_DIR}/utils/itt.hpp
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
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/3rdparty/vasot/include)

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
    target_link_libraries(${LIB_NAME} PUBLIC de_cv_ade)
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_calib3d4130)
    target_link_libraries(${LIB_NAME} PUBLIC de_opencv_video4130)
    target_link_libraries(${LIB_NAME} PUBLIC ws2_32)

endif()

# add_subdirectory(test)
