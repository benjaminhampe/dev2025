set(LIB_NAME de_opencv_core4130)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/opencv2/opencv.hpp	
        ${LIB_HEADER_DIR}/opencv2/core.hpp
        ${LIB_HEADER_DIR}/opencv2/custom_hal.hpp
        ${LIB_HEADER_DIR}/opencv2/cvconfig.h
        ${LIB_HEADER_DIR}/opencv2/cv_cpu_config.h
        ${LIB_HEADER_DIR}/opencv2/opencv_data_config.hpp
        ${LIB_HEADER_DIR}/opencv2/opencv_modules.hpp
        ${LIB_HEADER_DIR}/opencv2/opencv_tests_config.hpp

        ${LIB_HEADER_DIR}/opencv2/core/affine.hpp
        ${LIB_HEADER_DIR}/opencv2/core/async.hpp
        ${LIB_HEADER_DIR}/opencv2/core/base.hpp
        ${LIB_HEADER_DIR}/opencv2/core/bindings_utils.hpp
        ${LIB_HEADER_DIR}/opencv2/core/bufferpool.hpp
        ${LIB_HEADER_DIR}/opencv2/core/check.hpp
        ${LIB_HEADER_DIR}/opencv2/core/core.hpp
        ${LIB_HEADER_DIR}/opencv2/core/core_c.h

        ${LIB_HEADER_DIR}/opencv2/core/cuda/block.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/border_interpolate.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/color.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/common.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/cuda_compat.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/datamov_utils.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/dynamic_smem.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/emulation.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/filters.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/funcattrib.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/functional.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/limits.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/reduce.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/saturate_cast.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/scan.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/simd_functions.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/transform.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/type_traits.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/utility.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/vec_distance.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/vec_math.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/vec_traits.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/warp.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/warp_reduce.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/warp_shuffle.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda.inl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda_stream_accessor.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda_types.hpp

        ${LIB_HEADER_DIR}/opencv2/core/cuda/detail/color_detail.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/detail/reduce.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/detail/reduce_key_val.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/detail/transform_detail.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/detail/type_traits_detail.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cuda/detail/vec_distance_detail.hpp

        ${LIB_HEADER_DIR}/opencv2/core/cvdef.h
        ${LIB_HEADER_DIR}/opencv2/core/cvstd.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cvstd.inl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cvstd_wrapper.hpp
        ${LIB_HEADER_DIR}/opencv2/core/cv_cpu_dispatch.h
        ${LIB_HEADER_DIR}/opencv2/core/cv_cpu_helper.h

        ${LIB_HEADER_DIR}/opencv2/core/detail/async_promise.hpp
        ${LIB_HEADER_DIR}/opencv2/core/detail/dispatch_helper.impl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/detail/exception_ptr.hpp
        ${LIB_HEADER_DIR}/opencv2/core/directx.hpp
        ${LIB_HEADER_DIR}/opencv2/core/dualquaternion.hpp
        ${LIB_HEADER_DIR}/opencv2/core/dualquaternion.inl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/eigen.hpp
        ${LIB_HEADER_DIR}/opencv2/core/fast_math.hpp

        ${LIB_HEADER_DIR}/opencv2/core/hal/hal.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/interface.h
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_avx.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_avx512.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_cpp.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_forward.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_lasx.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_legacy_ops.h
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_lsx.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_math.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_msa.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_neon.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_rvv071.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_rvv_scalable.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_sse.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_sse_em.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_vsx.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/intrin_wasm.hpp
        ${LIB_HEADER_DIR}/opencv2/core/hal/msa_macros.h
        ${LIB_HEADER_DIR}/opencv2/core/hal/simd_utils.impl.hpp

        ${LIB_HEADER_DIR}/opencv2/core/llapi/llapi.h
        ${LIB_HEADER_DIR}/opencv2/core/mat.hpp
        ${LIB_HEADER_DIR}/opencv2/core/mat.inl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/matx.hpp
        ${LIB_HEADER_DIR}/opencv2/core/matx.inl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/neon_utils.hpp
        ${LIB_HEADER_DIR}/opencv2/core/ocl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/ocl_genbase.hpp

        ${LIB_HEADER_DIR}/opencv2/core/opencl/ocl_defs.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/opencl_info.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/opencl_svm.hpp

        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/autogenerated/opencl_clblas.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/autogenerated/opencl_clfft.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/autogenerated/opencl_core.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/autogenerated/opencl_core_wrappers.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/autogenerated/opencl_gl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/autogenerated/opencl_gl_wrappers.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_clblas.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_clfft.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_core.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_core_wrappers.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_gl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_gl_wrappers.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_svm_20.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_svm_definitions.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opencl/runtime/opencl_svm_hsa_extension.hpp
        ${LIB_HEADER_DIR}/opencv2/core/opengl.hpp

        ${LIB_HEADER_DIR}/opencv2/core/openvx/ovx_defs.hpp

        ${LIB_HEADER_DIR}/opencv2/core/operations.hpp
        ${LIB_HEADER_DIR}/opencv2/core/optim.hpp
        ${LIB_HEADER_DIR}/opencv2/core/ovx.hpp
        ${LIB_HEADER_DIR}/opencv2/core/persistence.hpp

        ${LIB_HEADER_DIR}/opencv2/core/parallel/backend/parallel_for.openmp.hpp
        ${LIB_HEADER_DIR}/opencv2/core/parallel/backend/parallel_for.tbb.hpp
        ${LIB_HEADER_DIR}/opencv2/core/parallel/parallel_backend.hpp

        ${LIB_HEADER_DIR}/opencv2/core/private.hpp
        ${LIB_HEADER_DIR}/opencv2/core/private/cuda_stubs.hpp
        ${LIB_HEADER_DIR}/opencv2/core/private/cv_cpu_include_simd_declarations.hpp
        ${LIB_HEADER_DIR}/opencv2/core/private.cuda.hpp

        ${LIB_HEADER_DIR}/opencv2/core/quaternion.hpp
        ${LIB_HEADER_DIR}/opencv2/core/quaternion.inl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/saturate.hpp
        ${LIB_HEADER_DIR}/opencv2/core/simd_intrinsics.hpp
        ${LIB_HEADER_DIR}/opencv2/core/softfloat.hpp
        ${LIB_HEADER_DIR}/opencv2/core/sse_utils.hpp
        ${LIB_HEADER_DIR}/opencv2/core/traits.hpp
        ${LIB_HEADER_DIR}/opencv2/core/types.hpp
        ${LIB_HEADER_DIR}/opencv2/core/types_c.h
        ${LIB_HEADER_DIR}/opencv2/core/utility.hpp
        ${LIB_HEADER_DIR}/opencv2/core/va_intel.hpp
        ${LIB_HEADER_DIR}/opencv2/core/version.hpp
        ${LIB_HEADER_DIR}/opencv2/core/vsx_utils.hpp

        ${LIB_HEADER_DIR}/opencv2/core/utils/allocator_stats.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/allocator_stats.impl.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/buffer_area.private.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/configuration.private.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/filesystem.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/filesystem.private.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/fp_control.private.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/fp_control_utils.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/instrumentation.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/lock.private.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/logger.defines.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/logger.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/logtag.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/plugin_loader.private.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/tls.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/trace.hpp
        ${LIB_HEADER_DIR}/opencv2/core/utils/trace.private.hpp

    )
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/algorithm.cpp
        ${LIB_SOURCE_DIR}/alloc.cpp
        ${LIB_SOURCE_DIR}/arithm.cpp
        ${LIB_SOURCE_DIR}/arithm.dispatch.cpp
        ${LIB_SOURCE_DIR}/arithm.simd.hpp
        ${LIB_SOURCE_DIR}/arithm_ipp.hpp
        ${LIB_SOURCE_DIR}/array.cpp
        ${LIB_SOURCE_DIR}/async.cpp
        ${LIB_SOURCE_DIR}/batch_distance.cpp
        ${LIB_SOURCE_DIR}/bindings_utils.cpp
        ${LIB_SOURCE_DIR}/bufferpool.impl.hpp
        ${LIB_SOURCE_DIR}/buffer_area.cpp
        ${LIB_SOURCE_DIR}/channels.cpp
        ${LIB_SOURCE_DIR}/check.cpp
        ${LIB_SOURCE_DIR}/command_line_parser.cpp
        ${LIB_SOURCE_DIR}/conjugate_gradient.cpp
        ${LIB_SOURCE_DIR}/convert.dispatch.cpp
        ${LIB_SOURCE_DIR}/convert.hpp
        ${LIB_SOURCE_DIR}/convert.simd.hpp
        ${LIB_SOURCE_DIR}/convert_c.cpp
        ${LIB_SOURCE_DIR}/convert_scale.dispatch.cpp
        ${LIB_SOURCE_DIR}/convert_scale.simd.hpp
        ${LIB_SOURCE_DIR}/copy.cpp
        ${LIB_SOURCE_DIR}/count_non_zero.dispatch.cpp
        ${LIB_SOURCE_DIR}/count_non_zero.simd.hpp

        ${LIB_SOURCE_DIR}/cuda/gpu_mat.cu
        ${LIB_SOURCE_DIR}/cuda/gpu_mat_nd.cu
        ${LIB_SOURCE_DIR}/cuda_gpu_mat.cpp
        ${LIB_SOURCE_DIR}/cuda_gpu_mat_nd.cpp
        ${LIB_SOURCE_DIR}/cuda_host_mem.cpp
        ${LIB_SOURCE_DIR}/cuda_info.cpp
        ${LIB_SOURCE_DIR}/cuda_stream.cpp
        ${LIB_SOURCE_DIR}/datastructs.cpp
        ${LIB_SOURCE_DIR}/directx.cpp
        ${LIB_SOURCE_DIR}/directx.inc.hpp
        ${LIB_SOURCE_DIR}/downhill_simplex.cpp
        ${LIB_SOURCE_DIR}/dxt.cpp
        ${LIB_SOURCE_DIR}/glob.cpp
        ${LIB_SOURCE_DIR}/gl_core_3_1.cpp
        ${LIB_SOURCE_DIR}/gl_core_3_1.hpp
        ${LIB_SOURCE_DIR}/hal_internal.cpp
        ${LIB_SOURCE_DIR}/hal_internal.hpp
        ${LIB_SOURCE_DIR}/hal_replacement.hpp
        ${LIB_SOURCE_DIR}/has_non_zero.dispatch.cpp
        ${LIB_SOURCE_DIR}/has_non_zero.simd.hpp
        ${LIB_SOURCE_DIR}/intel_gpu_gemm.inl.hpp
        ${LIB_SOURCE_DIR}/kmeans.cpp
        ${LIB_SOURCE_DIR}/lapack.cpp
        ${LIB_SOURCE_DIR}/lda.cpp
        ${LIB_SOURCE_DIR}/logger.cpp
        ${LIB_SOURCE_DIR}/lpsolver.cpp
        ${LIB_SOURCE_DIR}/lut.cpp
        ${LIB_SOURCE_DIR}/mathfuncs.cpp
        ${LIB_SOURCE_DIR}/mathfuncs.hpp
        ${LIB_SOURCE_DIR}/mathfuncs_core.dispatch.cpp
        ${LIB_SOURCE_DIR}/mathfuncs_core.simd.hpp
        ${LIB_SOURCE_DIR}/matmul.dispatch.cpp
        ${LIB_SOURCE_DIR}/matmul.simd.hpp
        ${LIB_SOURCE_DIR}/matrix.cpp
        ${LIB_SOURCE_DIR}/matrix_c.cpp
        ${LIB_SOURCE_DIR}/matrix_decomp.cpp
        ${LIB_SOURCE_DIR}/matrix_expressions.cpp
        ${LIB_SOURCE_DIR}/matrix_iterator.cpp
        ${LIB_SOURCE_DIR}/matrix_operations.cpp
        ${LIB_SOURCE_DIR}/matrix_sparse.cpp
        ${LIB_SOURCE_DIR}/matrix_transform.cpp
        ${LIB_SOURCE_DIR}/matrix_wrap.cpp
        ${LIB_SOURCE_DIR}/mean.dispatch.cpp
        ${LIB_SOURCE_DIR}/mean.simd.hpp
        ${LIB_SOURCE_DIR}/merge.dispatch.cpp
        ${LIB_SOURCE_DIR}/merge.simd.hpp
        ${LIB_SOURCE_DIR}/minmax.cpp
        ${LIB_SOURCE_DIR}/norm.dispatch.cpp
        ${LIB_SOURCE_DIR}/norm.simd.hpp
        ${LIB_SOURCE_DIR}/ocl.cpp
        ${LIB_SOURCE_DIR}/ocl_disabled.impl.hpp

        ${LIB_SOURCE_DIR}/opencl/arithm.cl
        ${LIB_SOURCE_DIR}/opencl/convert.cl
        ${LIB_SOURCE_DIR}/opencl/copymakeborder.cl
        ${LIB_SOURCE_DIR}/opencl/copyset.cl
        ${LIB_SOURCE_DIR}/opencl/cvtclr_dx.cl
        ${LIB_SOURCE_DIR}/opencl/fft.cl
        ${LIB_SOURCE_DIR}/opencl/flip.cl
        ${LIB_SOURCE_DIR}/opencl/gemm.cl
        ${LIB_SOURCE_DIR}/opencl/halfconvert.cl
        ${LIB_SOURCE_DIR}/opencl/inrange.cl
        ${LIB_SOURCE_DIR}/opencl/intel_gemm.cl
        ${LIB_SOURCE_DIR}/opencl/lut.cl
        ${LIB_SOURCE_DIR}/opencl/meanstddev.cl
        ${LIB_SOURCE_DIR}/opencl/minmaxloc.cl
        ${LIB_SOURCE_DIR}/opencl/mixchannels.cl
        ${LIB_SOURCE_DIR}/opencl/mulspectrums.cl
        ${LIB_SOURCE_DIR}/opencl/normalize.cl
        ${LIB_SOURCE_DIR}/opencl/reduce.cl
        ${LIB_SOURCE_DIR}/opencl/reduce2.cl
        ${LIB_SOURCE_DIR}/opencl/repeat.cl

        ${LIB_SOURCE_DIR}/opencl/runtime/autogenerated/opencl_clblas_impl.hpp
        ${LIB_SOURCE_DIR}/opencl/runtime/autogenerated/opencl_clfft_impl.hpp
        ${LIB_SOURCE_DIR}/opencl/runtime/autogenerated/opencl_core_impl.hpp
        ${LIB_SOURCE_DIR}/opencl/runtime/autogenerated/opencl_core_static_impl.hpp
        ${LIB_SOURCE_DIR}/opencl/runtime/autogenerated/opencl_gl_impl.hpp
        ${LIB_SOURCE_DIR}/opencl/runtime/autogenerated/opencl_gl_static_impl.hpp

        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/common.py

        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/filter/opencl_clblas_functions.list
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/filter/opencl_clfft_functions.list
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/filter/opencl_core_functions.list
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/filter/opencl_gl_functions.list
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/generate.sh
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/parser_cl.py
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/parser_clblas.py
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/parser_clfft.py

        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_clblas.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_clblas_impl.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_clfft.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_clfft_impl.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_core.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_core_impl.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_core_wrappers.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_gl.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_gl_impl.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/opencl_gl_wrappers.hpp.in
        # ${LIB_SOURCE_DIR}/opencl/runtime/generator/template/static_impl.hpp.in
        ${LIB_SOURCE_DIR}/opencl/runtime/opencl_clblas.cpp
        ${LIB_SOURCE_DIR}/opencl/runtime/opencl_clfft.cpp
        ${LIB_SOURCE_DIR}/opencl/runtime/opencl_core.cpp
        ${LIB_SOURCE_DIR}/opencl/runtime/runtime_common.hpp
        ${LIB_SOURCE_DIR}/opencl/set_identity.cl
        ${LIB_SOURCE_DIR}/opencl/split_merge.cl
        ${LIB_SOURCE_DIR}/opencl/transpose.cl
        ${LIB_SOURCE_DIR}/opengl.cpp
        ${LIB_SOURCE_DIR}/out.cpp
        ${LIB_SOURCE_DIR}/ovx.cpp

        ${LIB_SOURCE_DIR}/parallel/factory_parallel.hpp
        ${LIB_SOURCE_DIR}/parallel/parallel.cpp
        ${LIB_SOURCE_DIR}/parallel/parallel.hpp
        ${LIB_SOURCE_DIR}/parallel/parallel_openmp.cpp
        ${LIB_SOURCE_DIR}/parallel/parallel_tbb.cpp
        ${LIB_SOURCE_DIR}/parallel/plugin_parallel_api.hpp
        ${LIB_SOURCE_DIR}/parallel/plugin_parallel_wrapper.impl.hpp
        ${LIB_SOURCE_DIR}/parallel/registry_parallel.hpp
        ${LIB_SOURCE_DIR}/parallel/registry_parallel.impl.hpp
        ${LIB_SOURCE_DIR}/parallel.cpp
        ${LIB_SOURCE_DIR}/parallel_impl.cpp
        ${LIB_SOURCE_DIR}/parallel_impl.hpp
        ${LIB_SOURCE_DIR}/pca.cpp
        ${LIB_SOURCE_DIR}/persistence.cpp
        ${LIB_SOURCE_DIR}/persistence.hpp
        ${LIB_SOURCE_DIR}/persistence_base64_encoding.cpp
        ${LIB_SOURCE_DIR}/persistence_base64_encoding.hpp
        ${LIB_SOURCE_DIR}/persistence_impl.hpp
        ${LIB_SOURCE_DIR}/persistence_json.cpp
        ${LIB_SOURCE_DIR}/persistence_types.cpp
        ${LIB_SOURCE_DIR}/persistence_xml.cpp
        ${LIB_SOURCE_DIR}/persistence_yml.cpp
        ${LIB_SOURCE_DIR}/precomp.hpp
        ${LIB_SOURCE_DIR}/rand.cpp
        ${LIB_SOURCE_DIR}/softfloat.cpp
        ${LIB_SOURCE_DIR}/split.dispatch.cpp
        ${LIB_SOURCE_DIR}/split.simd.hpp
        ${LIB_SOURCE_DIR}/stat.dispatch.cpp
        ${LIB_SOURCE_DIR}/stat.hpp
        ${LIB_SOURCE_DIR}/stat.simd.hpp
        ${LIB_SOURCE_DIR}/stat_c.cpp
        ${LIB_SOURCE_DIR}/stl.cpp
        ${LIB_SOURCE_DIR}/sum.dispatch.cpp
        ${LIB_SOURCE_DIR}/sum.simd.hpp
        ${LIB_SOURCE_DIR}/system.cpp
        ${LIB_SOURCE_DIR}/tables.cpp
        ${LIB_SOURCE_DIR}/trace.cpp
        ${LIB_SOURCE_DIR}/types.cpp
        ${LIB_SOURCE_DIR}/umatrix.cpp
        ${LIB_SOURCE_DIR}/umatrix.hpp

        ${LIB_SOURCE_DIR}/utils/datafile.cpp
        ${LIB_SOURCE_DIR}/utils/filesystem.cpp
        ${LIB_SOURCE_DIR}/utils/logtagconfig.hpp
        ${LIB_SOURCE_DIR}/utils/logtagconfigparser.cpp
        ${LIB_SOURCE_DIR}/utils/logtagconfigparser.hpp
        ${LIB_SOURCE_DIR}/utils/logtagmanager.cpp
        ${LIB_SOURCE_DIR}/utils/logtagmanager.hpp
        ${LIB_SOURCE_DIR}/utils/plugin_loader.impl.hpp
        ${LIB_SOURCE_DIR}/utils/samples.cpp
        ${LIB_SOURCE_DIR}/va_intel.cpp
        ${LIB_SOURCE_DIR}/va_wrapper.impl.hpp

        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_core.cpp
        ${LIB_SOURCE_DIR}/_benni/opencl_kernels_core.hpp
        ${LIB_SOURCE_DIR}/_benni/arithm.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/convert.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/convert_scale.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/count_non_zero.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/has_non_zero.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/mathfuncs_core.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/matmul.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/mean.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/merge.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/norm.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/split.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/stat.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/sum.simd_declarations.hpp
        ${LIB_SOURCE_DIR}/_benni/version_string.inc
    )

    # if(WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED_LIBS)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        target_compile_definitions(${LIB_NAME} PUBLIC CVAPI_EXPORTS=1)
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

    target_link_libraries(${LIB_NAME} PUBLIC de_opencl)
    target_link_libraries(${LIB_NAME} PUBLIC de_zlib)
    target_link_libraries(${LIB_NAME} PUBLIC pthread)

endif()

# add_subdirectory(test)
