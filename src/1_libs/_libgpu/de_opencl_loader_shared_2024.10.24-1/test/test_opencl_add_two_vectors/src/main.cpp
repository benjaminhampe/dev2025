#include <CL/cl.h>          // OpenCL API header
#include <iostream>         // For printing output
#include <vector>           // Modern C++ dynamic arrays
#include <string>           // For build log handling

static const char* kernelSource = R"(
__kernel                     // Like main(), but executed tens of thousands of times in parallel on the GPU, each invocation with its own thread ID.
void vec_add(
    __global const float* A, // Pointer into the GPU's global memory address space (VRAM). Allocated via clCreateBuffer; not a host/RAM pointer.
    __global const float* B, // Same: device global memory, readable by all parallel work-items.
    __global float* C        // Output buffer in device global memory where each thread writes its result.
)
{
    // Each GPU thread (work-item) gets a unique index in the global NDRange.
    int id = get_global_id(0);  // 1D thread index for this invocation.

    // Each thread computes one element of the output vector independently.
    C[id] = A[id] + B[id];
}
)";

int main() {
    cl_int err;

    // ------------------------------------------------------------
    // 1. Query OpenCL platforms (NVIDIA, AMD, Intel, POCL, etc.)
    // ------------------------------------------------------------
    cl_uint numPlatforms = 0;
    err = clGetPlatformIDs(0, nullptr, &numPlatforms);
    if (err != CL_SUCCESS || numPlatforms == 0) {
        std::cout << "No OpenCL platforms found.\n";
        return 1;
    }

    cl_platform_id platform = nullptr;
    clGetPlatformIDs(1, &platform, nullptr); // Pick the first platform

    // ------------------------------------------------------------
    // 2. Query a device (prefer GPU, fallback to CPU)
    // ------------------------------------------------------------
    cl_device_id device = nullptr;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);
    if (err != CL_SUCCESS) {
        std::cout << "No GPU found, trying CPU...\n";
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, nullptr);
        if (err != CL_SUCCESS) {
            std::cout << "No OpenCL device available.\n";
            return 1;
        }
    }

    // Print device name
    char deviceName[256];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
    std::cout << "Using device: " << deviceName << "\n";

    // ------------------------------------------------------------
    // 3. Create an OpenCL context
    // A context owns memory buffers, programs, and command queues.
    // ------------------------------------------------------------
    cl_context context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);

    // ------------------------------------------------------------
    // 4. Create a command queue
    // clCreateCommandQueueWithProperties is the modern API.
    // Even on OpenCL 1.2 drivers (Windows), this function exists.
    // ------------------------------------------------------------
    cl_queue_properties props[] = { 0 }; // No special properties
    cl_command_queue queue = clCreateCommandQueueWithProperties(context, device, props, &err);

    // ------------------------------------------------------------
    // 5. Create and build the OpenCL program from source
    // ------------------------------------------------------------
    cl_program program = clCreateProgramWithSource(context, 1, &kernelSource, nullptr, &err);

    // Build the program (compile + link)
    err = clBuildProgram(program, 1, &device, nullptr, nullptr, nullptr);

    // If build fails, print the compiler log
    if (err != CL_SUCCESS) {
        size_t logSize = 0;
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &logSize);

        std::string log(logSize, '\0');
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, log.data(), nullptr);

        std::cout << "Build error:\n" << log << "\n";
        return 1;
    }

    // ------------------------------------------------------------
    // 6. Create the kernel function
    // ------------------------------------------------------------
    cl_kernel kernel = clCreateKernel(program, "vec_add", &err);

    // ------------------------------------------------------------
    // 7. Prepare host-side data using std::vector
    // ------------------------------------------------------------
    const int N = 1024;
    std::vector<float> A(N), B(N), C(N);

    for (int i = 0; i < N; i++) {
        A[i] = float(i);
        B[i] = float(2 * i);
    }

    // ------------------------------------------------------------
    // 8. Create OpenCL buffers (GPU memory)
    // CL_MEM_COPY_HOST_PTR copies data immediately into the buffer.
    // ------------------------------------------------------------
    cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(float) * N, A.data(), &err);

    cl_mem bufB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(float) * N, B.data(), &err);

    cl_mem bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY,
                                 sizeof(float) * N, nullptr, &err);

    // ------------------------------------------------------------
    // 9. Set kernel arguments
    // ------------------------------------------------------------
    clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufB);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufC);

    // ------------------------------------------------------------
    // 10. Execute the kernel
    // globalSize = number of GPU threads (work-items)
    // Each work-item computes one element of C.
    // ------------------------------------------------------------
    size_t globalSize = N;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, nullptr, &globalSize, nullptr,
                                 0, nullptr, nullptr);

    // ------------------------------------------------------------
    // 11. Read back the results from GPU to CPU
    // ------------------------------------------------------------
    clEnqueueReadBuffer(queue, bufC, CL_TRUE, 0,
                        sizeof(float) * N, C.data(),
                        0, nullptr, nullptr);

    // ------------------------------------------------------------
    // 12. Verify correctness
    // ------------------------------------------------------------
    bool ok = true;
    for (int i = 0; i < N; i++) {
        if (C[i] != A[i] + B[i]) {
            ok = false;
            break;
        }
    }

    std::cout << "Compute test: " << (ok ? "SUCCESS" : "FAILURE") << "\n";
    std::cout << "Sample output: C[10] = " << C[10] << "\n";

    // ------------------------------------------------------------
    // 13. Cleanup
    // ------------------------------------------------------------
    clReleaseMemObject(bufA);
    clReleaseMemObject(bufB);
    clReleaseMemObject(bufC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}
