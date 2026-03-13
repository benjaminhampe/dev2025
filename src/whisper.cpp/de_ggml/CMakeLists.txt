set(LIB_NAME de_ggml)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
set(GGML_VULKAN ON) # ggml: use Vulkan
# set(GGML_VULKAN_CHECK_RESULTS            "ggml: run Vulkan op checks"                      OFF)
# set(GGML_VULKAN_DEBUG                    "ggml: enable Vulkan debug output"                OFF)
# set(GGML_VULKAN_MEMORY_DEBUG             "ggml: enable Vulkan memory debug output"         OFF)
# set(GGML_VULKAN_SHADER_DEBUG_INFO        "ggml: enable Vulkan shader debug info"           OFF)
# set(GGML_VULKAN_VALIDATE                 "ggml: enable Vulkan validation"                  OFF)
# set(GGML_VULKAN_RUN_TESTS                "ggml: run Vulkan tests"                          OFF)
set(GGML_OPENMP 1) # "ggml: use OpenMP"
set(GGML_OPENCL OFF) # "ggml: use OpenCL"
set(GGML_OPENCL_PROFILING OFF) # "ggml: use OpenCL profiling (increases overhead)"
set(GGML_OPENCL_EMBED_KERNELS ON) # "ggml: embed kernels"
# set(GGML_OPENCL_USE_ADRENO_KERNELS     "ggml: use optimized kernels for Adreno"          ON)
# set(GGML_BLAS                          "ggml: use BLAS"                                  ${GGML_BLAS_DEFAULT})
# set(GGML_BLAS_VENDOR ${GGML_BLAS_VENDOR_DEFAULT} CACHE STRING "ggml: BLAS library vendor")
set(GGML_LLAMAFILE ON) #                        "ggml: use LLAMAFILE" ${GGML_LLAMAFILE_DEFAULT}
# set(GGML_CPU_HBM OFF) # "ggml: use memkind for CPU HBM"
set(GGML_CPU_REPACK ON) # "ggml: use runtime weight conversion of Q4_0 to Q4_X_X"
# set(GGML_CPU_KLEIDIAI     "ggml: use KleidiAI optimized kernels if applicable" OFF)
# set(GGML_SSE42            "ggml: enable SSE 4.2"          ${INS_ENB})
# set(GGML_AVX              "ggml: enable AVX"              ${INS_ENB})
# set(GGML_AVX_VNNI         "ggml: enable AVX-VNNI"         OFF)
# set(GGML_AVX2             "ggml: enable AVX2"             ${INS_ENB})
# set(GGML_BMI2             "ggml: enable BMI2"             ${INS_ENB})
# set(GGML_AVX512           "ggml: enable AVX512F"          OFF)
# set(GGML_AVX512_VBMI      "ggml: enable AVX512-VBMI"      OFF)
# set(GGML_AVX512_VNNI      "ggml: enable AVX512-VNNI"      OFF)
# set(GGML_AVX512_BF16      "ggml: enable AVX512-BF16"      OFF)
# set(GGML_ACCELERATE                    "ggml: enable Accelerate framework"               ON)
# if (NOT MSVC)
# # in MSVC F16C and FMA is implied with AVX2/AVX512
# set(GGML_FMA          "ggml: enable FMA"              ${INS_ENB})
# set(GGML_F16C         "ggml: enable F16C"             ${INS_ENB})
# # MSVC does not seem to support AMX
# set(GGML_AMX_TILE     "ggml: enable AMX-TILE"         OFF)
# set(GGML_AMX_INT8     "ggml: enable AMX-INT8"         OFF)
# set(GGML_AMX_BF16     "ggml: enable AMX-BF16"         OFF)
# endif()
	
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)

    set(LIB_HEADERS 
        ${LIB_HEADER_DIR}/ggml.h	
        ${LIB_HEADER_DIR}/ggml-alloc.h
        ${LIB_HEADER_DIR}/ggml-backend.h
        ${LIB_HEADER_DIR}/ggml-blas.h
        # ${LIB_HEADER_DIR}/ggml-cann.h
        ${LIB_HEADER_DIR}/ggml-cpp.h
        ${LIB_HEADER_DIR}/ggml-cpu.h
        # ${LIB_HEADER_DIR}/ggml-cuda.h
        # ${LIB_HEADER_DIR}/ggml-hexagon.h
        # ${LIB_HEADER_DIR}/ggml-metal.h
        ${LIB_HEADER_DIR}/ggml-opencl.h
        ${LIB_HEADER_DIR}/ggml-opt.h
        # ${LIB_HEADER_DIR}/ggml-rpc.h
        # ${LIB_HEADER_DIR}/ggml-sycl.h
        # ${LIB_HEADER_DIR}/ggml-virtgpu.h
        ${LIB_HEADER_DIR}/ggml-vulkan.h
        # ${LIB_HEADER_DIR}/ggml-webgpu.h
        # ${LIB_HEADER_DIR}/ggml-zdnn.h
        # ${LIB_HEADER_DIR}/ggml-zendnn.h
        ${LIB_HEADER_DIR}/gguf.h
    )

    set(LIB_SOURCES 
        ${LIB_SOURCE_DIR}/ggml.cpp
        ${LIB_SOURCE_DIR}/gguf.cpp
        ${LIB_SOURCE_DIR}/ggml-alloc.c
        ${LIB_SOURCE_DIR}/ggml-common.h
        ${LIB_SOURCE_DIR}/ggml.c
        ${LIB_SOURCE_DIR}/ggml-impl.h
        ${LIB_SOURCE_DIR}/ggml-opt.cpp
        ${LIB_SOURCE_DIR}/ggml-quants.c
        ${LIB_SOURCE_DIR}/ggml-quants.h
        ${LIB_SOURCE_DIR}/ggml-threading.cpp
        ${LIB_SOURCE_DIR}/ggml-threading.h
		
        ${LIB_SOURCE_DIR}/ggml-backend-dl.cpp # GGML_BACKEND_SHARED
        ${LIB_SOURCE_DIR}/ggml-backend-dl.h # GGML_BACKEND_SHARED
        ${LIB_SOURCE_DIR}/ggml-backend-reg.cpp # GGML_BACKEND_SHARED
        ${LIB_SOURCE_DIR}/ggml-backend-impl.h
        ${LIB_SOURCE_DIR}/ggml-backend.cpp

        # ${LIB_SOURCE_DIR}/ggml-blas/CMakeLists.txt
        # ${LIB_SOURCE_DIR}/ggml-blas/ggml-blas.cpp
		
        # ${LIB_SOURCE_DIR}/ggml-cann/aclnn_ops.cpp
        # ${LIB_SOURCE_DIR}/ggml-cann/aclnn_ops.h
        # ${LIB_SOURCE_DIR}/ggml-cann/acl_tensor.cpp
        # ${LIB_SOURCE_DIR}/ggml-cann/acl_tensor.h
        # ${LIB_SOURCE_DIR}/ggml-cann/CMakeLists.txt
        # ${LIB_SOURCE_DIR}/ggml-cann/common.h
        # ${LIB_SOURCE_DIR}/ggml-cann/ggml-cann.cpp

        # ${LIB_SOURCE_DIR}/ggml-cpu/amx/amx.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/amx/amx.h
        # ${LIB_SOURCE_DIR}/ggml-cpu/amx/common.h
        # ${LIB_SOURCE_DIR}/ggml-cpu/amx/mmq.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/amx/mmq.h

        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/arm/cpu-feats.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/arm/quants.c
        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/arm/repack.cpp

        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/loongarch/quants.c

        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/powerpc/cpu-feats.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/powerpc/quants.c

        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/riscv/cpu-feats.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/riscv/quants.c
        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/riscv/repack.cpp

        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/s390/cpu-feats.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/s390/quants.c

        # ${LIB_SOURCE_DIR}/ggml-cpu/arch/wasm/quants.c

        ${LIB_SOURCE_DIR}/ggml-cpu/arch/x86/cpu-feats.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/arch/x86/quants.c
        ${LIB_SOURCE_DIR}/ggml-cpu/arch/x86/repack.cpp
		
        ${LIB_SOURCE_DIR}/ggml-cpu/arch-fallback.h
		
        ${LIB_SOURCE_DIR}/ggml-cpu/binary-ops.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/binary-ops.h

        # ${LIB_SOURCE_DIR}/ggml-cpu/cmake/FindSIMD.cmake
        # ${LIB_SOURCE_DIR}/ggml-cpu/CMakeLists.txt
        ${LIB_SOURCE_DIR}/ggml-cpu/common.h
        ${LIB_SOURCE_DIR}/ggml-cpu/ggml-cpu-impl.h
        ${LIB_SOURCE_DIR}/ggml-cpu/ggml-cpu.c
        ${LIB_SOURCE_DIR}/ggml-cpu/ggml-cpu.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/hbm.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/hbm.h

        # ${LIB_SOURCE_DIR}/ggml-cpu/kleidiai/kernels.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/kleidiai/kernels.h
        # ${LIB_SOURCE_DIR}/ggml-cpu/kleidiai/kleidiai.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/kleidiai/kleidiai.h

        ${LIB_SOURCE_DIR}/ggml-cpu/ops.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/ops.h
        ${LIB_SOURCE_DIR}/ggml-cpu/quants.c
        ${LIB_SOURCE_DIR}/ggml-cpu/quants.h
        ${LIB_SOURCE_DIR}/ggml-cpu/repack.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/repack.h
        ${LIB_SOURCE_DIR}/ggml-cpu/simd-gemm.h
        ${LIB_SOURCE_DIR}/ggml-cpu/simd-mappings.h

        # ${LIB_SOURCE_DIR}/ggml-cpu/spacemit/ime.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/spacemit/ime.h
        # ${LIB_SOURCE_DIR}/ggml-cpu/spacemit/ime1_kernels.cpp
        # ${LIB_SOURCE_DIR}/ggml-cpu/spacemit/ime_kernels.h
        ${LIB_SOURCE_DIR}/ggml-cpu/traits.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/traits.h
        ${LIB_SOURCE_DIR}/ggml-cpu/unary-ops.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/unary-ops.h
        ${LIB_SOURCE_DIR}/ggml-cpu/vec.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/vec.h

        # ${LIB_SOURCE_DIR}/ggml-hip	# AMD = the ROCm/HIP backend that accelerates ggml on AMD GPUs. (Heterogeneous‑compute Interface for Portability 
        # ${LIB_SOURCE_DIR}/ggml-hip/CMakeLists.txt

        # ${LIB_SOURCE_DIR}/ggml-musa	# Moore Threads GPUs — a Chinese GPU vendor.
        # ${LIB_SOURCE_DIR}/ggml-musa/CMakeLists.txt
        # ${LIB_SOURCE_DIR}/ggml-musa/mudnn.cu
        # ${LIB_SOURCE_DIR}/ggml-musa/mudnn.cuh
    )
	
	set(LIB_CPU_LLAMAFILE_SOURCES
	    ${LIB_SOURCE_DIR}/ggml-cpu/llamafile/sgemm.cpp
        ${LIB_SOURCE_DIR}/ggml-cpu/llamafile/sgemm.h
	)
	
	set(LIB_VULKAN_SOURCES
        ${LIB_SOURCE_DIR}/ggml-vulkan/ggml-vulkan.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/ggml-vulkan-shaders.hpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/abs.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/acc.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/add.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/add1.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/add_id.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/arange.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/argmax.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/argsort.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/argsort_large.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/ceil.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/clamp.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/concat.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/contig_copy.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/conv2d_dw.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/conv2d_mm.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/conv_transpose_1d.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/copy.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/copy_from_quant.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/copy_to_quant.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/copy_transpose.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/cos.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/count_equal.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/count_experts.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/cumsum.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/cumsum_multipass1.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/cumsum_multipass2.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_f32.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq1_m.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq1_s.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq2_s.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq2_xs.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq2_xxs.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq3_s.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq3_xxs.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq4_nl.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_iq4_xs.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_mxfp4.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q2_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q3_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q4_0.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q4_1.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q4_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q5_0.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q5_1.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q5_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q6_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/dequant_q8_0.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/diag.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/diag_mask_inf.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/div.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/exp.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/fill.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/flash_attn.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/flash_attn_cm1.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/flash_attn_cm2.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/flash_attn_mask_opt.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/flash_attn_split_k_reduce.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/floor.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/geglu.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/geglu_erf.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/geglu_quick.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/gelu.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/gelu_erf.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/gelu_quick.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/get_rows.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/get_rows_quant.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/group_norm.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/hardsigmoid.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/hardswish.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/im2col.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/im2col_3d.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/l2_norm.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/leaky_relu.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/log.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/multi_add.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_split_k_reduce.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vecq.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_iq1_m.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_iq1_s.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_iq2_s.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_iq2_xs.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_iq2_xxs.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_iq3_s.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_iq3_xxs.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_nc.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_p021.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_q2_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_q3_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_q4_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_q5_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mat_vec_q6_k.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mm.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mmq.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/mul_mm_cm2.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/neg.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/norm.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/opt_step_adamw.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/opt_step_sgd.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/pad.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/pool2d.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/quantize_q8_1.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/reglu.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/relu.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/repeat.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/repeat_back.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/rms_norm.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/rms_norm_back.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/rms_norm_partials.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/roll.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/rope_multi.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/rope_neox.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/rope_norm.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/rope_vision.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/round.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/scale.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/sigmoid.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/silu.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/silu_back.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/sin.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/softplus.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/soft_max.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/soft_max_back.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/soft_max_large1.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/soft_max_large2.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/soft_max_large3.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/solve_tri.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/sqrt.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/square.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/ssm_conv.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/ssm_scan.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/step.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/sub.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/sum_rows.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/swiglu.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/swiglu_oai.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/tanh.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/timestep_embedding.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/topk_argsort.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/topk_moe.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/topk_nary_search.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/tri.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/trunc.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/upscale.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/wkv6.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/wkv7.comp.cpp
        ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled/xielu.comp.cpp
		
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/abs.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/acc.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/add.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/add1.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/add_id.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/arange.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/argmax.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/argsort.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/argsort_large.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/ceil.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/clamp.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/CMakeLists.txt
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/concat.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/contig_copy.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/conv2d_dw.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/conv2d_mm.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/conv_transpose_1d.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/copy.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/copy_from_quant.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/copy_to_quant.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/copy_transpose.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/cos.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/count_equal.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/count_experts.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/cumsum.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/cumsum_multipass1.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/cumsum_multipass2.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_f32.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_funcs.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_funcs_cm2.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_head.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq1_m.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq1_s.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq2_s.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq2_xs.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq2_xxs.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq3_s.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq3_xxs.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq4_nl.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_iq4_xs.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_mxfp4.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q2_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q3_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q4_0.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q4_1.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q4_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q5_0.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q5_1.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q5_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q6_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/dequant_q8_0.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/diag.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/diag_mask_inf.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/div.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/exp.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/feature-tests
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/feature-tests/bfloat16.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/feature-tests/coopmat.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/feature-tests/coopmat2.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/feature-tests/integer_dot.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/fill.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/flash_attn.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/flash_attn_base.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/flash_attn_cm1.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/flash_attn_cm2.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/flash_attn_mask_opt.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/flash_attn_split_k_reduce.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/floor.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/geglu.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/geglu_erf.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/geglu_quick.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/gelu.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/gelu_erf.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/gelu_quick.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/generic_binary_head.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/generic_head.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/generic_unary_head.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/get_rows.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/get_rows_quant.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/glu_head.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/glu_main.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/group_norm.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/hardsigmoid.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/hardswish.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/im2col.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/im2col_3d.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/l2_norm.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/leaky_relu.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/log.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/multi_add.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_split_k_reduce.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vecq.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vecq_funcs.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_base.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_iface.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_iq1_m.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_iq1_s.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_iq2_s.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_iq2_xs.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_iq2_xxs.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_iq3_s.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_iq3_xxs.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_nc.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_p021.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_q2_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_q3_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_q4_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_q5_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mat_vec_q6_k.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mm.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mmq.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mmq_funcs.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mmq_shmem_types.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mm_cm2.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mm_funcs.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/mul_mm_id_funcs.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/neg.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/norm.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/opt_step_adamw.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/opt_step_sgd.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/pad.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/pool2d.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/quantize_q8_1.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/reglu.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/relu.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/repeat.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/repeat_back.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rms_norm.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rms_norm_back.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rms_norm_partials.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/roll.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rope_funcs.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rope_head.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rope_multi.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rope_neox.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rope_norm.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rope_params.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rope_vision.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/round.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/rte.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/scale.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/sigmoid.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/silu.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/silu_back.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/sin.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/softplus.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/soft_max.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/soft_max_back.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/soft_max_large1.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/soft_max_large2.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/soft_max_large3.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/soft_max_large_common.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/solve_tri.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/sqrt.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/square.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/ssm_conv.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/ssm_scan.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/step.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/sub.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/sum_rows.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/sum_rows.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/swiglu.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/swiglu_oai.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/tanh.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/timestep_embedding.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/topk_argsort.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/topk_moe.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/topk_nary_search.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/tri.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/trunc.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/types.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/upscale.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/utils.glsl
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/vulkan-shaders-gen.cpp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/wkv6.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/wkv7.comp
        # ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders/xielu.comp

    )
	
	set(LIB_OPENCL_SOURCES
        # ${LIB_SOURCE_DIR}/ggml-opencl/CMakeLists.txt
        ${LIB_SOURCE_DIR}/ggml-opencl/ggml-opencl.cpp
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/add.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/add_id.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/argsort.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/clamp.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/concat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/conv2d.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/conv2d_f16_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/cpy.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/cvt.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/diag_mask_inf.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/div.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/embed_kernel.py
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/expm1.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/fill.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/flash_attn_f16.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/flash_attn_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/flash_attn_f32_f16.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/gelu.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/gemm_moe_mxfp4_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/gemv_moe_mxfp4_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/gemv_noshuffle.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/gemv_noshuffle_general.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/gemv_noshuffle_general_q8_0_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/get_rows.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/glu.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/group_norm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/im2col_f16.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/im2col_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mean.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mat_Ab_Bi_8x4.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mat_f16_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mm_f16_f32_kq_kqv.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mm_f16_f32_l4_lm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mm_f32_f32_l4_lm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mm_q4_0_f32_l4_lm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mm_q4_1_f32_l4_lm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mm_q6_k_f32_l4_lm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mm_q8_0_f32_8x4.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mm_q8_0_f32_l4_lm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_f16_f16.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_f16_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_f16_f32_1row.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_f16_f32_l4.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_f32_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_id_mxfp4_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_id_mxfp4_f32_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_id_q4_0_f32_8x_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_id_q8_0_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_id_q8_0_f32_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_mxfp4_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_mxfp4_f32_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q4_0_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q4_0_f32_1d_16x_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q4_0_f32_1d_8x_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q4_0_f32_8x_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q4_0_f32_v.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q4_1_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q4_1_f32_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q4_k_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q6_k_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q6_k_f32_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q8_0_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/mul_mv_q8_0_f32_flat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/norm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/pad.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/relu.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/repeat.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/rms_norm.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/rope.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/scale.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/set_rows.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/sigmoid.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/silu.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/softmax_4_f16.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/softmax_4_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/softmax_f16.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/softmax_f32.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/softplus.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/solve_tri.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/sqr.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/sqrt.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/ssm_conv.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/sub.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/sum_rows.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/tanh.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/transpose.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/tri.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/tsembd.cl
        # ${LIB_SOURCE_DIR}/ggml-opencl/kernels/upscale.cl	
	)
	
    # if (WIN32)
        # enable_language(RC)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.h)
        # list(APPEND LIB_SOURCES ${CMAKE_CURRENT_LIST_DIR}/res/resource.rc)
    # endif()

    if (BUILD_SHARED)
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} SHARED "")
        # target_compile_definitions(${LIB_NAME} PUBLIC GGML_BACKEND_SHARED=1)
        # target_compile_definitions(${LIB_NAME} PRIVATE GGML_BACKEND_BUILD=1)
    else()
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
        add_library(${LIB_NAME} STATIC "")
        # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SHARED=OFF)
    endif()

    # For building lib.
    target_compile_definitions(${LIB_NAME} PUBLIC GGML_CPU=ON) # whisper.cpp needs it
    target_compile_definitions(${LIB_NAME} PUBLIC GGML_USE_CPU=ON) # whisper.cpp needs it
    target_compile_definitions(${LIB_NAME} PRIVATE GGML_BACKEND_BUILD=ON)
    target_compile_definitions(${LIB_NAME} PUBLIC GGML_BACKEND=ON)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_BACKEND_DL=OFF)
    # add_compile_options(-U GGML_BACKEND_DL)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_BACKEND_SHARED=OFF)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_OPENCL_FORCE_STATIC=ON)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_VULKAN_FORCE_STATIC=ON)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_CUDA_FORCE_STATIC=ON)

    if (GGML_CPU_REPACK)
        target_compile_definitions(${LIB_NAME} PRIVATE GGML_USE_CPU_REPACK) # GGML_CPU_NAME
    endif()

    if (GGML_LLAMAFILE) # GGML_CPU
        message("${BoldCyan}[benni] LIB: ${LIB_NAME} - WITH CPU LLAMAFILE${ColourReset}")
        target_compile_definitions(${LIB_NAME} PRIVATE GGML_USE_LLAMAFILE)
        list(APPEND LIB_SOURCES ${LIB_CPU_LLAMAFILE_SOURCES})
    endif()

    if (GGML_OPENMP)
        message("${BoldCyan}[benni] LIB: ${LIB_NAME} - WITH CPU OPENMP${ColourReset}")
        target_compile_definitions(${LIB_NAME} PRIVATE GGML_OPENMP_ENABLED=1)
        target_compile_definitions(${LIB_NAME} PRIVATE GGML_USE_OPENMP=1)
        target_link_libraries(${LIB_NAME} PRIVATE omp) # OpenMP::OpenMP_C OpenMP::OpenMP_CXX
    endif()

    if (GGML_OPENBLAS)
        message("${BoldCyan}[benni] LIB: ${LIB_NAME} - WITH CPU OPENBLAS${ColourReset}")
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_BLAS=1)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_USE_OPENBLAS=1)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_OPENBLAS=1)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_BLAS_VENDOR="OpenBLAS")
    endif()

    if (GGML_OPENCL)
        message("${BoldCyan}[benni] LIB: ${LIB_NAME} - WITH GPU OPENCL${ColourReset}")
        list(APPEND LIB_SOURCES ${LIB_OPENCL_SOURCES})
        target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ggml-opencl)
        target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ggml-opencl/autogenerated)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_OPENCL=ON)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_USE_OPENCL=ON)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_CLBLAST=ON)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_USE_CLBLAST=ON)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_OPENCL_EMBED_KERNELS=ON)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_OPENCL_SOA_Q=ON)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_OPENCL_TARGET_VERSION=300)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_OPENCL_PROFILING=ON)
        target_link_libraries(${LIB_NAME} PUBLIC de_opencl_loader_shared)
        target_link_libraries(${LIB_NAME} PUBLIC de_opencl_CLBlast_shared)
    endif()

    if (GGML_VULKAN)
        message("${BoldCyan}[benni] LIB: ${LIB_NAME} - WITH GPU VULKAN${ColourReset}")
        list(APPEND LIB_SOURCES ${LIB_VULKAN_SOURCES})
        target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ggml-vulkan)
        target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ggml-vulkan/vulkan-shaders-compiled)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_VULKAN=1)
        target_compile_definitions(${LIB_NAME} PUBLIC GGML_USE_VULKAN=1)
        # target_compile_definitions(${LIB_NAME} PUBLIC GGML_VULKAN_DEBUG=1)
        target_link_libraries(${LIB_NAME} PUBLIC de_vulkan_loader_shared)
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES})

    target_compile_definitions(${LIB_NAME} PRIVATE GGML_VERSION_MAJOR=0)
    target_compile_definitions(${LIB_NAME} PRIVATE GGML_VERSION_MINOR=9)
    target_compile_definitions(${LIB_NAME} PRIVATE GGML_VERSION_PATCH=7)
    target_compile_definitions(${LIB_NAME} PRIVATE GGML_VERSION_BASE="0.9.7")
    target_compile_definitions(${LIB_NAME} PRIVATE GGML_VERSION="0.9.7")
    target_compile_definitions(${LIB_NAME} PRIVATE GGML_INSTALL_VERSION="0.9.7")

    target_compile_definitions(${LIB_NAME} PRIVATE GGML_BUILD_COMMIT="benni")
    target_compile_definitions(${LIB_NAME} PRIVATE GGML_COMMIT="benni")

    # target_compile_definitions(${LIB_NAME} PRIVATE GGML_BACKEND_DL=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE GGML_BACKEND_BUILD=1)
    # target_compile_definitions(${LIB_NAME} PRIVATE GGML_BACKEND_SHARED=1)

    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_CPU_X86=1)
    target_compile_definitions(${LIB_NAME} PUBLIC GGML_CPU_X86_64=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_CPU_ARM=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_CPU_ARM64=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_CPU_WASM=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_CPU_RISCV=1)

    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_SSE2=1) # -msse2
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_SSE3=1) # 		-msse3
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_SSSE3=1) # 		-mssse3
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_SSE41=1) # 		-msse4.1
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_SSE42=1) # 		-msse4.2
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_AVX=1) # 			-mavx
    target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_AVX2=1) # 		-mavx2
    target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_FMA=1) # 			-mfma
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_AVX512F=1) # 		-mavx512f
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_AVX512BW=1) #  	-mavx512bw
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_AVX512DQ=1) #  	-mavx512dq
    # target_compile_definitions(${LIB_NAME} PUBLIC GGML_SIMD_AVX512VL=1) #  	-mavx512vl

    # ggml-cuda → NVIDIA
    # ggml-hip → AMD ROCm
    # ggml-vulkan → Vulkan GPUs
    # ggml-metal → Apple GPUs
    # ggml-opencl → generic OpenCL GPUs

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ggml-cpu)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/ggml-cpu/arch)

endif()

# add_subdirectory(test)
